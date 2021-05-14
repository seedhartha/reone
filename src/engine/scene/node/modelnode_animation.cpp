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

#include "modelnode.h"

#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtx/transform.hpp"

using namespace std;

using namespace reone::graphics;

namespace reone {

namespace scene {

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
    AnimationBlendMode blendMode = getAnimationBlendMode(properties.flags);

    switch (blendMode) {
        case AnimationBlendMode::Single:
            // In Single mode, add animation to the first channel
            resetAnimationChannel(_animChannels[0], anim, lipAnim, properties);
            break;

        case AnimationBlendMode::Blend: {
            // In Blend mode, if there is an unfinished animation in the first
            // channel, move it into the second channel and initiate transition
            bool transition = false;
            if (_animChannels[0].anim && !_animChannels[0].finished) {
                _animChannels[1] = _animChannels[0];
                _animChannels[1].transition = false;
                _animChannels[1].freeze = true;
                transition = true;
            }
            // Add animation to the first channel
            resetAnimationChannel(_animChannels[0], anim, lipAnim, properties);
            _animChannels[0].transition = transition;
            break;
        }

        case AnimationBlendMode::Overlay:
            // In Overlay mode, add animation to the first channel and move all
            // other channels down the stack. If current mode is not Overlay,
            // reset all other channels.
            if (_animBlendMode == AnimationBlendMode::Overlay) {
                for (int i = kNumAnimationChannels - 1; i > 0; --i) {
                    _animChannels[i] = _animChannels[i - 1];
                }
            } else {
                for (int i = 1; i < kNumAnimationChannels; ++i) {
                    resetAnimationChannel(_animChannels[i]);
                }
            }
            resetAnimationChannel(_animChannels[0], anim, lipAnim, properties);
            break;

        default:
            break;
    }

    _animBlendMode = blendMode;

    // Optionally propagate animation to attachments
    if (properties.flags & AnimationFlags::propagate) {
        for (auto &attachment : _attachmentByNodeId) {
            if (attachment.second->type() == SceneNodeType::Model) {
                static_pointer_cast<ModelSceneNode>(attachment.second)->playAnimation(anim, lipAnim, properties);
            }
        }
    }
}

ModelSceneNode::AnimationBlendMode ModelSceneNode::getAnimationBlendMode(int flags) {
    return (flags & AnimationFlags::blend) ?
        AnimationBlendMode::Blend :
        ((flags & AnimationFlags::overlay) ? AnimationBlendMode::Overlay : AnimationBlendMode::Single);
}

void ModelSceneNode::resetAnimationChannel(AnimationChannel &channel, shared_ptr<Animation> anim, shared_ptr<LipAnimation> lipAnim, AnimationProperties properties) {
    channel.anim = move(anim);
    channel.lipAnim = move(lipAnim);
    channel.properties = move(properties);
    channel.time = 0.0f;
    channel.stateById.clear();
    channel.finished = false;
    channel.transition = false;
    channel.freeze = false;
}

void ModelSceneNode::updateAnimations(float dt) {
    for (auto &channel : _animChannels) {
        updateAnimationChannel(channel, dt);
    }

    // Apply states and compute bone transforms only when this model is not culled
    if (!_culled) {
        applyAnimationStates(*_model->rootNode());
        computeBoneTransforms();
    }
}

void ModelSceneNode::updateAnimationChannel(AnimationChannel &channel, float dt) {
    // Do not update if there is no animation, freezed or a finished animation
    // in the channel
    if (!channel.anim || channel.freeze || channel.finished) return;

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
        channel.stateById.clear();
        computeAnimationStates(channel, time, *_model->rootNode());
    }
}

void ModelSceneNode::computeAnimationStates(AnimationChannel &channel, float time, const ModelNode &modelNode) {
    shared_ptr<ModelNode> animNode(channel.anim->getNodeById(modelNode.id()));
    if (animNode && _inanimateNodes.count(modelNode.id()) == 0) {
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

        channel.stateById.insert(make_pair(modelNode.id(), move(state)));
    }

    for (auto &child : modelNode.children()) {
        computeAnimationStates(channel, time, *child);
    }
}

void ModelSceneNode::applyAnimationStates(const ModelNode &modelNode) {
    auto maybeSceneNode = _nodeById.find(modelNode.id());
    if (maybeSceneNode != _nodeById.end()) {
        shared_ptr<SceneNode> sceneNode(maybeSceneNode->second);
        AnimationState combined;

        switch (_animBlendMode) {
            case AnimationBlendMode::Single:
            case AnimationBlendMode::Blend: {
                bool blend = _animBlendMode == AnimationBlendMode::Blend && _animChannels[0].transition;
                auto state1 = _animChannels[0].stateById.count(modelNode.id()) > 0 ? _animChannels[0].stateById[modelNode.id()] : AnimationState();
                auto state2 = _animChannels[1].stateById.count(modelNode.id()) > 0 ? _animChannels[1].stateById[modelNode.id()] : AnimationState();
                if (blend && state1.flags & AnimationStateFlags::transform && state2.flags & AnimationStateFlags::transform) {
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
                for (int i = 0; i < kNumAnimationChannels; ++i) {
                    auto maybeState = _animChannels[i].stateById.find(modelNode.id());
                    if (maybeState != _animChannels[i].stateById.end()) {
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
                }
                break;
            default:
                break;
        }

        if (combined.flags & AnimationStateFlags::transform) {
            sceneNode->setLocalTransform(combined.transform);
        } else if (!modelNode.isSkinMesh()) {
            sceneNode->setLocalTransform(modelNode.localTransform());
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

void ModelSceneNode::computeBoneTransforms() {
    for (auto &node : _nodeById) {
        glm::mat4 transform(1.0f);
        transform = node.second->absoluteTransform() * node.second->modelNode()->absoluteTransformInverse(); // make relative to the rest pose (world space)
        transform = _absTransformInv * transform; // world space to model space
        node.second->setBoneTransform(move(transform));
    }
}

bool ModelSceneNode::isAnimationFinished() const {
    return _animChannels[0].anim && _animChannels[0].finished;
}

} // namespace scene

} // namespace reone
