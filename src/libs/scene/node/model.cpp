/*
 * Copyright (c) 2020-2023 The reone project contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "reone/scene/node/model.h"

#include "reone/graphics/animation.h"
#include "reone/graphics/context.h"
#include "reone/graphics/di/services.h"
#include "reone/graphics/mesh.h"
#include "reone/graphics/meshes.h"
#include "reone/graphics/shaderregistry.h"
#include "reone/graphics/uniforms.h"
#include "reone/scene/graph.h"
#include "reone/scene/node/emitter.h"
#include "reone/scene/node/light.h"
#include "reone/scene/node/mesh.h"
#include "reone/scene/types.h"
#include "reone/system/logutil.h"

using namespace reone::audio;
using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace scene {

static constexpr float kTransitionLength = 0.25f;

ModelSceneNode::ModelSceneNode(
    Model &model,
    ModelUsage usage,
    SceneGraph &sceneGraph,
    GraphicsServices &graphicsSvc,
    AudioServices &audioSvc,
    ResourceServices &resourceSvc) :
    SceneNode(
        SceneNodeType::Model,
        sceneGraph,
        graphicsSvc,
        audioSvc,
        resourceSvc),
    _model(&model),
    _usage(usage) {

    init();
}

void ModelSceneNode::init() {
    if (!_nodeByNumber.empty()) {
        return;
    }
    buildNodeTree(*_model->rootNode(), *this);
    computeAABB();
    _point = _aabb.isEmpty();
}

void ModelSceneNode::buildNodeTree(ModelNode &node, SceneNode &parent) {
    // Convert model node to scene node
    std::shared_ptr<ModelNodeSceneNode> sceneNode;
    if (node.isMesh()) {
        sceneNode = _sceneGraph.newMesh(*this, node);
    } else if (node.isLight()) {
        sceneNode = _sceneGraph.newLight(*this, node);
    } else if (node.isEmitter()) {
        sceneNode = _sceneGraph.newEmitter(node);
    } else {
        sceneNode = _sceneGraph.newDummy(node);
    }

    if (node.isSkinMesh()) {
        // Reparent skin meshes to prevent animation being applied twice
        glm::mat4 transform(node.parent()->absoluteTransform() * node.localTransform());
        sceneNode->setLocalTransform(std::move(transform));
        addChild(*sceneNode);
    } else {
        sceneNode->setLocalTransform(node.localTransform());
        parent.addChild(*sceneNode);
    }
    _nodeByNumber[node.number()] = sceneNode.get();
    _nodeByName[node.name()] = sceneNode.get();

    if (node.isReference()) {
        auto reference = node.reference();
        if (reference->model) {
            auto model = _sceneGraph.newModel(*reference->model, _usage);
            model->init();
            attach(node.name(), *model);
        }
    }
    for (auto &child : node.children()) {
        buildNodeTree(*child, *sceneNode);
    }
}

void ModelSceneNode::update(float dt) {
    // Optimization: skip invisible models
    if (!_enabled) {
        return;
    }
    SceneNode::update(dt);
    updateAnimations(dt);
}

void ModelSceneNode::drawLeafs(const std::vector<SceneNode *> &leafs) {
    for (auto &leaf : leafs) {
        static_cast<MeshSceneNode *>(leaf)->draw();
    }
}

void ModelSceneNode::drawAABB() {
    _graphicsSvc.context.withPolygonMode(PolygonMode::Line, [this]() {
        _graphicsSvc.uniforms.setGeneral([this](auto &u) {
            u.resetLocals();
            u.model = _absTransform;
            u.model *= glm::translate(_aabb.center());
            u.model *= glm::scale(0.5f * _aabb.size());
            u.modelInv = glm::inverse(u.model);
        });
        _graphicsSvc.context.useProgram(_graphicsSvc.shaderRegistry.get(ShaderProgramId::AABB));
        _graphicsSvc.meshes.box().draw();
    });
}

void ModelSceneNode::computeAABB() {
    _aabb.reset();

    for (auto &node : _nodeByNumber) {
        if (node.second->type() == SceneNodeType::Mesh) {
            auto &modelNode = node.second->modelNode();
            auto mesh = modelNode.mesh();
            if (!mesh || !mesh->mesh) {
                continue;
            }
            auto modelSpaceAABB = mesh->mesh->aabb() * modelNode.absoluteTransform();
            _aabb.expand(modelSpaceAABB);
        }
    }

    for (auto &attachment : _attachments) {
        if (attachment.second->type() == SceneNodeType::Model) {
            AABB modelSpaceAABB(attachment.second->aabb() * attachment.second->absoluteTransform() * _absTransformInv);
            _aabb.expand(modelSpaceAABB);
        }
    }
}

void ModelSceneNode::signalEvent(const std::string &name) {
    if (name == "detonate") {
        for (auto &node : _nodeByNumber) {
            if (node.second->type() == SceneNodeType::Emitter) {
                static_cast<EmitterSceneNode *>(node.second)->detonate();
            }
        }
    } else if (_animEventListener) {
        _animEventListener->onEventSignalled(name);
    }
}

void ModelSceneNode::attach(const std::string &parentName, SceneNode &node) {
    auto maybeParent = _nodeByName.find(parentName);
    if (maybeParent == _nodeByName.end()) {
        return;
    }
    auto parent = maybeParent->second;
    parent->addChild(node);

    _attachments.insert(std::make_pair(parentName, &node));

    computeAABB();
}

ModelNodeSceneNode *ModelSceneNode::getNodeByNumber(uint16_t number) {
    auto it = _nodeByNumber.find(number);
    return it != _nodeByNumber.end() ? it->second : nullptr;
}

ModelNodeSceneNode *ModelSceneNode::getNodeByName(const std::string &name) {
    auto it = _nodeByName.find(name);
    return it != _nodeByName.end() ? it->second : nullptr;
}

SceneNode *ModelSceneNode::getAttachment(const std::string &parentName) {
    auto parent = _model->getNodeByName(parentName);
    if (!parent) {
        return nullptr;
    }
    auto it = _attachments.find(parent->name());
    return it != _attachments.end() ? it->second : nullptr;
}

void ModelSceneNode::setDiffuseMap(Texture *texture) {
    for (auto &child : _children) {
        if (child->type() == SceneNodeType::Dummy || child->type() == SceneNodeType::Mesh) {
            static_cast<ModelNodeSceneNode *>(child)->setDiffuseMap(texture);
        }
    }
}

void ModelSceneNode::setEnvironmentMap(Texture *texture) {
    for (auto &child : _children) {
        if (child->type() == SceneNodeType::Dummy || child->type() == SceneNodeType::Mesh) {
            static_cast<ModelNodeSceneNode *>(child)->setEnvironmentMap(texture);
        }
    }
}

void ModelSceneNode::playAnimation(const std::string &name, graphics::LipAnimation *lipAnim, AnimationProperties properties) {
    auto anim = _model->getAnimation(name);
    if (anim) {
        playAnimation(*anim, lipAnim, std::move(properties));
    }
}

void ModelSceneNode::playAnimation(Animation &anim, LipAnimation *lipAnim, AnimationProperties properties) {
    if (properties.scale == 0.0f) {
        properties.scale = _model->animationScale();
    }

    // Return if same animation is already playing
    if (!_animChannels.empty() &&
        _animChannels[0].anim == &anim && _animChannels[0].lipAnim == lipAnim && _animChannels[0].properties == properties)
        return;

    AnimationBlendMode blendMode = getAnimationBlendMode(properties.flags);

    switch (blendMode) {
    case AnimationBlendMode::Single:
        // In Single mode, clear channels and add animation on top
        _animChannels.clear();
        _animChannels.push_front(AnimationChannel(anim, lipAnim, properties));
        break;

    case AnimationBlendMode::Blend: {
        // In Blend mode, if there is an animation on top, initiate
        // transition between old and new animations
        bool transition = false;
        if (!_animChannels.empty()) {
            _animChannels[0].freeze = true;
            _animChannels[0].transition = false;
            transition = true;
        }
        // Add animation on top
        _animChannels.push_front(AnimationChannel(anim, lipAnim, properties));
        if (transition) {
            _animChannels[0].transition = true;
            _animChannels[0].time = glm::max(0.0f, _animChannels[0].anim->transitionTime() - kTransitionLength);
        }
        while (_animChannels.size() > 2ll) {
            _animChannels.pop_back();
        }
        break;
    }

    case AnimationBlendMode::Overlay:
        // In Overlay mode, clear channels only if previous mode is not
        // Overlay and add animation on top
        if (_animBlendMode != AnimationBlendMode::Overlay) {
            _animChannels.clear();
        }
        _animChannels.push_front(AnimationChannel(anim, lipAnim, properties));
        break;

    default:
        break;
    }

    _animBlendMode = blendMode;

    // Optionally propagate animation to attachments
    if (properties.flags & AnimationFlags::propagate) {
        for (auto &attachment : _attachments) {
            if (attachment.second->type() == SceneNodeType::Model) {
                static_cast<ModelSceneNode *>(attachment.second)->playAnimation(anim, lipAnim, properties);
            }
        }
    }
}

ModelSceneNode::AnimationBlendMode ModelSceneNode::getAnimationBlendMode(int flags) {
    return (flags & AnimationFlags::blend) ? AnimationBlendMode::Blend : ((flags & AnimationFlags::overlay) ? AnimationBlendMode::Overlay : AnimationBlendMode::Single);
}

void ModelSceneNode::updateAnimations(float dt) {
    // Erase finished channels
    switch (_animBlendMode) {
    case AnimationBlendMode::Single:
    case AnimationBlendMode::Overlay: {
        auto channelsToErase = std::remove_if(_animChannels.begin(), _animChannels.end(), [](auto &channel) { return channel.finished && (channel.properties.flags & AnimationFlags::fireForget); });
        _animChannels.erase(channelsToErase, _animChannels.end());
        break;
    }
    case AnimationBlendMode::Blend:
        if (_animChannels.size() > 1ll && !_animChannels[0].transition) {
            _animChannels.pop_back();
        }
        if (!_animChannels.empty() && _animChannels[0].finished) {
            _animChannels.pop_front();
        }
        break;
    default:
        break;
    }

    if (_animChannels.empty()) {
        playAnimation("default", nullptr, AnimationProperties::fromFlags(AnimationFlags::loop));
        return;
    }

    for (auto &channel : _animChannels) {
        if (!channel.freeze) {
            updateAnimationChannel(channel, dt);
        }
    }

    // Apply states and compute bone transforms only when this model is not culled
    if (!_culled) {
        applyAnimationStates(*_model->rootNode());
    }
}

void ModelSceneNode::updateAnimationChannel(AnimationChannel &channel, float dt) {
    // Take length from the lip animation, if any
    float length = channel.lipAnim ? channel.lipAnim->length() : channel.anim->length();

    // Advance time
    float oldTime = channel.time;
    channel.time = glm::min(length, channel.time + channel.properties.speed * dt);

    // Clear transition flag if past transition time
    if (channel.transition && channel.time >= channel.anim->transitionTime()) {
        channel.transition = false;
    }

    // Signal events between previous and current time
    for (auto &event : channel.anim->events()) {
        if (event.time > oldTime && event.time <= channel.time) {
            signalEvent(event.name);
        }
    }

    // Compute animation states only when this model is not culled
    if (!_culled) {
        float time = channel.transition ? channel.anim->transitionTime() : channel.time;
        channel.stateByNodeNumber.clear();
        computeAnimationStates(channel, time, *_model->rootNode());
    }

    bool lastFrame = channel.time == length;
    if (lastFrame) {
        bool loop = channel.properties.flags & AnimationFlags::loop;
        if (loop) {
            channel.time = 0.0f;
        } else {
            channel.finished = true;
        }
    }
}

static bool doesNodeHaveAncestor(const ModelNode &node, const std::string &name) {
    if (name.empty()) {
        return true;
    }
    if (node.name() == name) {
        return true;
    }
    auto parent = node.parent();
    if (!parent) {
        return false;
    }
    return doesNodeHaveAncestor(*parent, name);
}

void ModelSceneNode::computeAnimationStates(AnimationChannel &channel, float time, const ModelNode &modelNode) {
    std::shared_ptr<ModelNode> animNode(channel.anim->getNodeByName(modelNode.name()));
    if (animNode && modelNode.isAnimated() && doesNodeHaveAncestor(modelNode, channel.anim->root())) {
        AnimationState state;
        state.flags = 0;

        glm::vec3 position(modelNode.restPosition());
        glm::quat orientation(modelNode.restOrientation());
        float scale = 1.0f;

        if (channel.lipAnim) {
            uint8_t leftShape, rightShape;
            float factor;
            if (channel.lipAnim->getKeyframes(time, leftShape, rightShape, factor)) {
                glm::vec3 animPosition;
                if (animNode->getPosition(leftShape, rightShape, factor, animPosition)) {
                    position += channel.properties.scale * animPosition;
                    state.flags |= AnimationStateFlags::transform;
                }
                glm::quat animOrientation;
                if (animNode->getOrientation(leftShape, rightShape, factor, animOrientation)) {
                    orientation = std::move(animOrientation);
                    state.flags |= AnimationStateFlags::transform;
                }
                float animScale;
                if (animNode->getScale(leftShape, rightShape, factor, animScale)) {
                    scale = animScale;
                    state.flags |= AnimationStateFlags::transform;
                }
            }
        } else {
            glm::vec3 animPosition;
            if (animNode->position().getByTime(time, animPosition)) {
                position += channel.properties.scale * animPosition;
                state.flags |= AnimationStateFlags::transform;
            }
            glm::quat animOrientation;
            if (animNode->orientation().getByTime(time, animOrientation)) {
                orientation = std::move(animOrientation);
                state.flags |= AnimationStateFlags::transform;
            }
            float animScale;
            if (animNode->scale().getByTime(time, animScale)) {
                scale = animScale;
                state.flags |= AnimationStateFlags::transform;
            }
        }
        if (state.flags & AnimationStateFlags::transform) {
            state.transform *= glm::scale(glm::vec3(scale));
            state.transform *= glm::translate(position);
            state.transform *= glm::mat4_cast(orientation);
        }
        float animAlpha;
        if (animNode->alpha().getByTime(time, animAlpha)) {
            state.flags |= AnimationStateFlags::alpha;
            state.alpha = animAlpha;
        }
        glm::vec3 animSelfIllum;
        if (animNode->selfIllumColor().getByTime(time, animSelfIllum)) {
            state.flags |= AnimationStateFlags::selfIllumColor;
            state.selfIllumColor = std::move(animSelfIllum);
        }
        glm::vec3 animColor;
        if (animNode->color().getByTime(time, animColor)) {
            state.flags |= AnimationStateFlags::color;
            state.color = std::move(animColor);
        }
        channel.stateByNodeNumber[modelNode.number()] = std::move(state);
    }

    for (auto &child : modelNode.children()) {
        computeAnimationStates(channel, time, *child);
    }
}

void ModelSceneNode::applyAnimationStates(const ModelNode &modelNode) {
    auto maybeSceneNode = _nodeByNumber.find(modelNode.number());
    if (maybeSceneNode != _nodeByNumber.end()) {
        auto sceneNode = maybeSceneNode->second;
        AnimationState combined;

        switch (_animBlendMode) {
        case AnimationBlendMode::Single:
        case AnimationBlendMode::Blend: {
            AnimationState state1;
            auto state1Iter = _animChannels[0].stateByNodeNumber.find(modelNode.number());
            if (state1Iter != _animChannels[0].stateByNodeNumber.end()) {
                state1 = state1Iter->second;
            }
            bool blend = _animBlendMode == AnimationBlendMode::Blend && _animChannels[0].transition && _animChannels.size() > 1ll;
            if (blend) {
                AnimationState state2;
                auto state2Iter = _animChannels[1].stateByNodeNumber.find(modelNode.number());
                if (state2Iter != _animChannels[1].stateByNodeNumber.end()) {
                    state2 = state2Iter->second;
                }
                if (state1.flags & AnimationStateFlags::transform && state2.flags & AnimationStateFlags::transform) {
                    float factor = glm::min(1.0f, _animChannels[0].time / _animChannels[0].anim->transitionTime());
                    glm::vec3 scale1, scale2, translation1, translation2, skew;
                    glm::quat orientation1, orientation2;
                    glm::vec4 perspective;
                    glm::decompose(state1.transform, scale1, orientation1, translation1, skew, perspective);
                    glm::decompose(state2.transform, scale2, orientation2, translation2, skew, perspective);
                    combined.flags |= AnimationStateFlags::transform;
                    combined.transform *= glm::scale(glm::mix(scale2, scale1, factor));
                    combined.transform *= glm::translate(glm::mix(translation2, translation1, factor));
                    combined.transform *= glm::mat4_cast(glm::slerp(orientation2, orientation1, factor));
                } else if (state1.flags & AnimationStateFlags::transform) {
                    combined.flags |= AnimationStateFlags::transform;
                    combined.transform = state1.transform;
                } else if (state2.flags & AnimationStateFlags::transform) {
                    combined.flags |= AnimationStateFlags::transform;
                    combined.transform = state2.transform;
                }
            } else if (state1.flags & AnimationStateFlags::transform) {
                combined.flags |= AnimationStateFlags::transform;
                combined.transform = state1.transform;
            }
            if (state1.flags & AnimationStateFlags::alpha) {
                combined.flags |= AnimationStateFlags::alpha;
                combined.alpha = state1.alpha;
            }
            if (state1.flags & AnimationStateFlags::selfIllumColor) {
                combined.flags |= AnimationStateFlags::selfIllumColor;
                combined.selfIllumColor = state1.selfIllumColor;
            }
            if (state1.flags & AnimationStateFlags::color) {
                combined.flags |= AnimationStateFlags::color;
                combined.color = state1.color;
            }
            break;
        }
        case AnimationBlendMode::Overlay:
            for (auto &channel : _animChannels) {
                auto maybeState = channel.stateByNodeNumber.find(modelNode.number());
                if (maybeState == channel.stateByNodeNumber.end()) {
                    continue;
                }
                const AnimationState &state = maybeState->second;
                if ((state.flags & AnimationStateFlags::transform) && !(combined.flags & AnimationStateFlags::transform)) {
                    combined.flags |= AnimationStateFlags::transform;
                    combined.transform = state.transform;
                }
                if ((state.flags & AnimationStateFlags::alpha) && !(combined.flags & AnimationStateFlags::alpha)) {
                    combined.flags |= AnimationStateFlags::alpha;
                    combined.alpha = state.alpha;
                }
                if ((state.flags & AnimationStateFlags::selfIllumColor) && !(combined.flags & AnimationStateFlags::selfIllumColor)) {
                    combined.flags |= AnimationStateFlags::selfIllumColor;
                    combined.selfIllumColor = state.selfIllumColor;
                }
                if ((state.flags & AnimationStateFlags::color) && !(combined.flags & AnimationStateFlags::color)) {
                    combined.flags |= AnimationStateFlags::color;
                    combined.color = state.color;
                }
            }
            break;
        default:
            break;
        }

        if (combined.flags & AnimationStateFlags::transform) {
            sceneNode->setLocalTransform(combined.transform);
        }
        if (combined.flags & AnimationStateFlags::alpha) {
            static_cast<MeshSceneNode *>(sceneNode)->setAlpha(combined.alpha);
        }
        if (combined.flags & AnimationStateFlags::selfIllumColor) {
            static_cast<MeshSceneNode *>(sceneNode)->setSelfIllumColor(combined.selfIllumColor);
        }
        if (combined.flags & AnimationStateFlags::color) {
            static_cast<LightSceneNode *>(sceneNode)->setColor(combined.color);
        }
    }

    for (auto &child : modelNode.children()) {
        applyAnimationStates(*child);
    }
}

void ModelSceneNode::pauseAnimation() {
    if (_animChannels.empty()) {
        return;
    }
    _animChannels.front().freeze = true;
}

void ModelSceneNode::resumeAnimation() {
    if (_animChannels.empty()) {
        return;
    }
    _animChannels.front().freeze = false;
}

void ModelSceneNode::setAnimationTime(float time) {
    if (_animChannels.empty()) {
        return;
    }
    auto &channel = _animChannels.front();
    channel.time = time;
    bool looped = (channel.properties.flags & AnimationFlags::loop) != 0;
    bool frozen = channel.freeze;
    if (looped) {
        channel.properties.flags ^= AnimationFlags::loop;
    }
    channel.freeze = false;
    updateAnimations(0.0f);
    channel.freeze = frozen;
    if (looped) {
        channel.properties.flags |= AnimationFlags::loop;
    }
}

bool ModelSceneNode::isAnimationFinished() const {
    return _animChannels.empty() || _animChannels.front().finished;
}

std::string ModelSceneNode::activeAnimationName() const {
    if (_animChannels.empty()) {
        return "";
    }
    const AnimationChannel &channel = _animChannels.front();
    return channel.anim->name();
}

void ModelSceneNode::setModel(Model &model) {
    _children.clear();

    _model = &model;

    _nodeByName.clear();
    _nodeByNumber.clear();
    _attachments.clear();

    _animChannels.clear();
    _animBlendMode = AnimationBlendMode::Single;

    buildNodeTree(*_model->rootNode(), *this);
    computeAABB();
}

} // namespace scene

} // namespace reone
