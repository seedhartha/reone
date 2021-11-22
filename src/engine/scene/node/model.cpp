/*
 * Copyright (c) 2020-2021 The reone project contributors
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

#include "../../common/collectionutil.h"
#include "../../common/logutil.h"
#include "../../graphics/mesh/mesh.h"
#include "../../graphics/model/animation.h"

#include "../graph.h"
#include "../types.h"

#include "emitter.h"
#include "light.h"
#include "mesh.h"
#include "model.h"

using namespace std;

using namespace reone::graphics;

namespace reone {

namespace scene {

static constexpr float kTransitionLength = 0.25f;

ModelSceneNode::ModelSceneNode(
    shared_ptr<Model> model,
    ModelUsage usage,
    SceneGraph &sceneGraph,
    Context &context,
    Features &features,
    Materials &materials,
    Meshes &meshes,
    PBRIBL &pbrIbl,
    Shaders &shaders,
    Textures &textures,
    IAnimationEventListener *animEventListener) :
    SceneNode(
        SceneNodeType::Model,
        sceneGraph,
        context,
        meshes,
        shaders),
    _model(model),
    _usage(usage),
    _features(features),
    _materials(materials),
    _pbrIbl(pbrIbl),
    _textures(textures),
    _animEventListener(animEventListener) {

    _volumetric = true;

    buildNodeTree(_model->rootNode(), *this);
    computeAABB();
}

void ModelSceneNode::buildNodeTree(shared_ptr<ModelNode> node, SceneNode &parent) {
    // Convert model node to scene node
    shared_ptr<ModelNodeSceneNode> sceneNode;
    if (node->isMesh()) {
        sceneNode = newMeshSceneNode(node);
    } else if (node->isLight()) {
        sceneNode = newLightSceneNode(node);
    } else if (node->isEmitter()) {
        sceneNode = newEmitterSceneNode(node);
    } else {
        sceneNode = newDummySceneNode(node);
    }

    if (node->isSkinMesh()) {
        // Reparent skin meshes to prevent animation being applied twice
        glm::mat4 transform(node->parent()->absoluteTransform() * node->localTransform());
        sceneNode->setLocalTransform(move(transform));
        addChild(sceneNode);
    } else {
        sceneNode->setLocalTransform(node->localTransform());
        parent.addChild(sceneNode);
    }
    _nodeByNumber[node->number()] = sceneNode;
    _nodeByName[node->name()] = sceneNode;

    if (node->isReference()) {
        auto model = _sceneGraph.newModel(node->reference()->model, _usage, _animEventListener);
        attach(node->name(), move(model));
    }
    for (auto &child : node->children()) {
        buildNodeTree(child, *sceneNode);
    }
}

void ModelSceneNode::update(float dt) {
    // Optimization: skip invisible models
    if (!_visible)
        return;

    SceneNode::update(dt);
    updateAnimations(dt);
}

void ModelSceneNode::computeAABB() {
    _aabb.reset();

    for (auto &node : _nodeByNumber) {
        if (node.second->type() == SceneNodeType::Mesh) {
            auto &modelNode = node.second->modelNode();
            AABB modelSpaceAABB(modelNode.mesh()->mesh->aabb() * modelNode.absoluteTransform());
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

unique_ptr<DummySceneNode> ModelSceneNode::newDummySceneNode(shared_ptr<ModelNode> node) const {
    return make_unique<DummySceneNode>(move(node), _sceneGraph, _context, _meshes, _shaders);
}

unique_ptr<MeshSceneNode> ModelSceneNode::newMeshSceneNode(shared_ptr<ModelNode> node) const {
    return make_unique<MeshSceneNode>(*this, move(node), _sceneGraph, _context, _features, _materials, _meshes, _pbrIbl, _shaders, _textures);
}

unique_ptr<LightSceneNode> ModelSceneNode::newLightSceneNode(shared_ptr<ModelNode> node) const {
    return make_unique<LightSceneNode>(move(node), _sceneGraph, _context, _meshes, _shaders);
}

unique_ptr<EmitterSceneNode> ModelSceneNode::newEmitterSceneNode(shared_ptr<ModelNode> node) const {
    return make_unique<EmitterSceneNode>(move(node), _sceneGraph, _context, _meshes, _shaders);
}

void ModelSceneNode::signalEvent(const string &name) {
    debug(boost::format("Model '%s': event '%s' signalled") % _model->name() % name);

    if (name == "detonate") {
        for (auto &node : _nodeByNumber) {
            if (node.second->type() == SceneNodeType::Emitter) {
                static_pointer_cast<EmitterSceneNode>(node.second)->detonate();
            }
        }
    } else if (_animEventListener) {
        _animEventListener->onEventSignalled(name);
    }
}

void ModelSceneNode::attach(const string &parentName, shared_ptr<SceneNode> node) {
    auto maybeParent = _nodeByName.find(parentName);
    if (maybeParent == _nodeByName.end())
        return;

    shared_ptr<ModelNodeSceneNode> parent(maybeParent->second);
    parent->addChild(node);

    _attachments.insert(make_pair(parentName, node));

    computeAABB();
}

shared_ptr<ModelNodeSceneNode> ModelSceneNode::getNodeByNumber(uint16_t number) const {
    return getFromLookupOrNull(_nodeByNumber, number);
}

shared_ptr<ModelNodeSceneNode> ModelSceneNode::getNodeByName(const string &name) const {
    return getFromLookupOrNull(_nodeByName, name);
}

shared_ptr<SceneNode> ModelSceneNode::getAttachment(const string &parentName) const {
    auto parent = _model->getNodeByName(parentName);
    return parent ? getFromLookupOrNull(_attachments, parent->name()) : nullptr;
}

void ModelSceneNode::setDiffuseTexture(shared_ptr<Texture> texture) {
    for (auto &child : _children) {
        if (child->type() == SceneNodeType::Mesh) {
            static_pointer_cast<MeshSceneNode>(child)->setDiffuseTexture(texture);
        }
    }
}

void ModelSceneNode::setAppliedForce(glm::vec3 force) {
    for (auto &node : _nodeByNumber) {
        if (node.second->type() == SceneNodeType::Mesh) {
            static_pointer_cast<MeshSceneNode>(node.second)->setAppliedForce(force);
        }
    }
    for (auto &attachment : _attachments) {
        if (attachment.second->type() == SceneNodeType::Model) {
            static_pointer_cast<ModelSceneNode>(attachment.second)->setAppliedForce(force);
        }
    }
}

void ModelSceneNode::playAnimation(const string &name, AnimationProperties properties) {
    shared_ptr<Animation> anim(_model->getAnimation(name));
    if (anim) {
        playAnimation(anim, nullptr, move(properties));
    }
}

void ModelSceneNode::playAnimation(shared_ptr<Animation> anim, shared_ptr<LipAnimation> lipAnim, AnimationProperties properties) {
    if (properties.scale == 0.0f) {
        properties.scale = _model->animationScale();
    }

    // Return if same animation is already playing
    if (!_animChannels.empty() &&
        _animChannels[0].anim == anim && _animChannels[0].lipAnim == lipAnim && _animChannels[0].properties == properties)
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
                static_pointer_cast<ModelSceneNode>(attachment.second)->playAnimation(anim, lipAnim, properties);
            }
        }
    }
}

ModelSceneNode::AnimationBlendMode ModelSceneNode::getAnimationBlendMode(int flags) {
    return (flags & AnimationFlags::blend) ? AnimationBlendMode::Blend : ((flags & AnimationFlags::overlay) ? AnimationBlendMode::Overlay : AnimationBlendMode::Single);
}

void ModelSceneNode::updateAnimations(float dt) {
    if (_animChannels.empty())
        return;

    for (auto &channel : _animChannels) {
        if (!channel.freeze) {
            updateAnimationChannel(channel, dt);
        }
    }

    // Apply states and compute bone transforms only when this model is not culled
    if (!_culled) {
        applyAnimationStates(*_model->rootNode());
    }

    // Erase finished channels
    switch (_animBlendMode) {
    case AnimationBlendMode::Single:
    case AnimationBlendMode::Overlay: {
        auto channelsToErase = remove_if(_animChannels.begin(), _animChannels.end(), [](auto &channel) { return channel.finished; });
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
}

void ModelSceneNode::updateAnimationChannel(AnimationChannel &channel, float dt) {
    if (!channel.lipAnim && !channel.anim) {
        return;
    }

    // Take length from the lip animation, if any
    float length = channel.lipAnim ? channel.lipAnim->length() : channel.anim->length();

    // Advance time
    channel.time = glm::min(length, channel.time + channel.properties.speed * dt);

    // Clear transition flag if past transition time
    if (channel.transition && channel.time >= channel.anim->transitionTime()) {
        channel.transition = false;
    }

    // Signal events between previous and current time
    for (auto &event : channel.anim->events()) {
        if (event.time > channel.time && event.time <= channel.time) {
            signalEvent(event.name);
        }
    }

    // Loop or finish playing the animation
    if (channel.time == length) {
        bool loop = channel.properties.flags & AnimationFlags::loop;
        if (loop) {
            channel.time = 0.0f;
        } else {
            channel.finished = true;
        }
    }

    // Compute animation states only when this model is not culled
    if (!_culled) {
        float time = channel.transition ? channel.anim->transitionTime() : channel.time;
        channel.stateByNodeNumber.clear();
        computeAnimationStates(channel, time, *_model->rootNode());
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
    shared_ptr<ModelNode> animNode(channel.anim->getNodeByNumber(modelNode.number()));
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
                    orientation = move(animOrientation);
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
                orientation = move(animOrientation);
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
            state.selfIllumColor = move(animSelfIllum);
        }

        channel.stateByNodeNumber[modelNode.number()] = move(state);
    }

    for (auto &child : modelNode.children()) {
        computeAnimationStates(channel, time, *child);
    }
}

void ModelSceneNode::applyAnimationStates(const ModelNode &modelNode) {
    auto maybeSceneNode = _nodeByNumber.find(modelNode.number());
    if (maybeSceneNode != _nodeByNumber.end()) {
        shared_ptr<SceneNode> sceneNode(maybeSceneNode->second);
        AnimationState combined;

        switch (_animBlendMode) {
        case AnimationBlendMode::Single:
        case AnimationBlendMode::Blend: {
            auto state1 = getFromLookupOrElse(_animChannels[0].stateByNodeNumber, modelNode.number(), AnimationState());
            bool blend = _animBlendMode == AnimationBlendMode::Blend && _animChannels[0].transition && _animChannels.size() > 1ll;
            if (blend) {
                auto state2 = getFromLookupOrElse(_animChannels[1].stateByNodeNumber, modelNode.number(), AnimationState());
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
            break;
        }
        case AnimationBlendMode::Overlay:
            for (auto &channel : _animChannels) {
                auto maybeState = channel.stateByNodeNumber.find(modelNode.number());
                if (maybeState == channel.stateByNodeNumber.end())
                    continue;
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
            }
            break;
        default:
            break;
        }

        if (combined.flags & AnimationStateFlags::transform) {
            sceneNode->setLocalTransform(combined.transform);
        }
        if (combined.flags & AnimationStateFlags::alpha) {
            static_pointer_cast<MeshSceneNode>(sceneNode)->setAlpha(combined.alpha);
        }
        if (combined.flags & AnimationStateFlags::selfIllumColor) {
            static_pointer_cast<MeshSceneNode>(sceneNode)->setSelfIllumColor(combined.selfIllumColor);
        }
    }

    for (auto &child : modelNode.children()) {
        applyAnimationStates(*child);
    }
}

bool ModelSceneNode::isAnimationFinished() const {
    return _animChannels.empty();
}

string ModelSceneNode::getActiveAnimationName() const {
    if (_animChannels.empty())
        return "";

    const AnimationChannel &channel = _animChannels.front();
    if (!channel.anim || channel.finished)
        return "";

    return channel.anim->name();
}

void ModelSceneNode::setModel(shared_ptr<Model> model) {
    _children.clear();

    _model = move(model);

    _nodeByName.clear();
    _nodeByNumber.clear();
    _attachments.clear();

    _animChannels.clear();
    _animBlendMode = AnimationBlendMode::Single;

    buildNodeTree(_model->rootNode(), *this);
    computeAABB();
}

} // namespace scene

} // namespace reone
