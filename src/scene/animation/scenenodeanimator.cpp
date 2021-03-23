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

#include "scenenodeanimator.h"

#include <stdexcept>

#include "../node/lightnode.h"
#include "../node/modelnodescenenode.h"
#include "../node/modelscenenode.h"
#include "../types.h"

using namespace std;

using namespace reone::render;

namespace reone {

namespace scene {

static constexpr float kTransitionDuration = 0.25f;

SceneNodeAnimator::SceneNodeAnimator(ModelSceneNode *sceneNode, set<string> ignoreNodes) :
    _sceneNode(sceneNode),
    _ignoreNodes(ignoreNodes) {

    if (!sceneNode) {
        throw invalid_argument("sceneNode must not be null");
    }
    for (int i = 0; i < kChannelCount; ++i) {
        _channels.push_back(AnimationChannel(sceneNode, ignoreNodes));
    }
}

void SceneNodeAnimator::update(float dt, bool visible) {
    // Regardless of the composition mode, when there is no active animation on
    // the first channel, start the default animation
    if (!_channels[0].isActive()) {
        playDefaultAnimation();
        return;
    }

    // In the Blend mode, if the animation on the first channel is past
    // transition time, stop animation on the second channel
    if (isInTransition() && _channels[0].isPastTransitionTime()) {
        _channels[1].reset();
        _transition = false;
    }

    // Update animation channels
    for (auto &channel : _channels) {
        channel.update(dt, visible);
    }

    if (visible) {
        // Compute and apply node states to the managed model
        _stateByNumber.clear();
        computeSceneNodeStates(*_sceneNode->model()->rootNode());
        applySceneNodeStates(*_sceneNode->model()->rootNode());
    }
}

void SceneNodeAnimator::playDefaultAnimation() {
    if (!_defaultAnimName.empty()) {
        playAnimation(_defaultAnimName, _defaultAnimProperties);
    }
}

bool SceneNodeAnimator::isInTransition() const {
    return _compositionMode == CompositionMode::Blend && _transition;
}

void SceneNodeAnimator::computeSceneNodeStates(ModelNode &modelNode, glm::mat4 parentTransform) {
    if (modelNode.skin()) return;

    SceneNodeState state;
    state.flags |= SceneNodeStateFlags::transform;

    glm::mat4 localTransform(modelNode.localTransform());

    if (isInTransition()) {
        float delta = 1.0f - (_channels[0].getTransitionTime() - _channels[0].time()) / _channels[0].getTransitionTime();

        // In the Blend mode, blend animations on the first two channels (only transforms)
        SceneNodeState channel0State, channel1State;
        bool hasChannel0State = _channels[0].getSceneNodeStateByNumber(modelNode.nodeNumber(), channel0State);
        bool hasChannel1State = _channels[1].getSceneNodeStateByNumber(modelNode.nodeNumber(), channel1State);
        if (hasChannel0State && (channel0State.flags & SceneNodeStateFlags::transform) &&
            hasChannel1State && (channel1State.flags & SceneNodeStateFlags::transform)) {

            glm::quat orientation0(glm::toQuat(channel0State.transform));
            glm::quat orientation1(glm::toQuat(channel1State.transform));
            localTransform = glm::translate(glm::mat4(1.0f), glm::vec3(channel0State.transform[3]));
            localTransform *= glm::mat4_cast(glm::slerp(orientation1, orientation0, delta));

        } else if (hasChannel0State && (channel0State.flags & SceneNodeStateFlags::transform)) {
            localTransform = move(channel0State.transform);

        } else if (hasChannel1State && (channel1State.flags & SceneNodeStateFlags::transform)) {
            localTransform = move(channel1State.transform);
        }
        if (hasChannel0State) {
            if (channel0State.flags & SceneNodeStateFlags::alpha) {
                state.flags |= SceneNodeStateFlags::alpha;
                state.alpha = channel0State.alpha;
            }
            if (channel0State.flags & SceneNodeStateFlags::selfIllum) {
                state.flags |= SceneNodeStateFlags::selfIllum;
                state.selfIllumColor = channel0State.selfIllumColor;
            }
            if (channel0State.flags & SceneNodeStateFlags::lightColor) {
                state.flags |= SceneNodeStateFlags::lightColor;
                state.lightColor = channel0State.lightColor;
            }
            if (channel0State.flags & SceneNodeStateFlags::lightMultiplier) {
                state.flags |= SceneNodeStateFlags::lightMultiplier;
                state.lightMultiplier = channel0State.lightMultiplier;
            }
            if (channel0State.flags & SceneNodeStateFlags::lightRadius) {
                state.flags |= SceneNodeStateFlags::lightRadius;
                state.lightRadius = channel0State.lightRadius;
            }
        }
    } else if (_compositionMode == CompositionMode::Overlay) {
        // In the Overlay mode, for each state component select the first animation channel to have state for the given node
        int componentsLeft = SceneNodeStateFlags::all;
        for (int i = kChannelCount - 1; i >= 0; --i) {
            SceneNodeState channelState;
            if (_channels[i].isActive() && _channels[i].getSceneNodeStateByNumber(modelNode.nodeNumber(), channelState)) {
                if ((channelState.flags & SceneNodeStateFlags::transform) && (componentsLeft & SceneNodeStateFlags::transform)) {
                    localTransform = move(channelState.transform);
                    componentsLeft &= ~SceneNodeStateFlags::transform;
                }
                if ((channelState.flags & SceneNodeStateFlags::alpha) && (componentsLeft & SceneNodeStateFlags::alpha)) {
                    state.flags |= SceneNodeStateFlags::alpha;
                    state.alpha = channelState.alpha;
                    componentsLeft &= ~SceneNodeStateFlags::alpha;
                }
                if ((channelState.flags & SceneNodeStateFlags::selfIllum) && (componentsLeft & SceneNodeStateFlags::selfIllum)) {
                    state.flags |= SceneNodeStateFlags::selfIllum;
                    state.selfIllumColor = move(channelState.selfIllumColor);
                    componentsLeft &= ~SceneNodeStateFlags::selfIllum;
                }
                if ((channelState.flags & SceneNodeStateFlags::lightColor) && (componentsLeft & SceneNodeStateFlags::lightColor)) {
                    state.flags |= SceneNodeStateFlags::lightColor;
                    state.lightColor = move(channelState.lightColor);
                    componentsLeft &= ~SceneNodeStateFlags::lightColor;
                }
                if ((channelState.flags & SceneNodeStateFlags::lightMultiplier) && (componentsLeft & SceneNodeStateFlags::lightMultiplier)) {
                    state.flags |= SceneNodeStateFlags::lightMultiplier;
                    state.lightMultiplier = channelState.lightMultiplier;
                    componentsLeft &= ~SceneNodeStateFlags::lightMultiplier;
                }
                if ((channelState.flags & SceneNodeStateFlags::lightRadius) && (componentsLeft & SceneNodeStateFlags::lightRadius)) {
                    state.flags |= SceneNodeStateFlags::lightRadius;
                    state.lightRadius = channelState.lightRadius;
                    componentsLeft &= ~SceneNodeStateFlags::lightRadius;
                }
            }
        }
    } else {
        // Otherwise, select animation on the first channel
        SceneNodeState channelState;
        if (_channels[0].getSceneNodeStateByNumber(modelNode.nodeNumber(), channelState)) {
            if (channelState.flags & SceneNodeStateFlags::transform) {
                localTransform = move(channelState.transform);
            }
            if (channelState.flags & SceneNodeStateFlags::alpha) {
                state.flags |= SceneNodeStateFlags::alpha;
                state.alpha = channelState.alpha;
            }
            if (channelState.flags & SceneNodeStateFlags::selfIllum) {
                state.flags |= SceneNodeStateFlags::selfIllum;
                state.selfIllumColor = move(channelState.selfIllumColor);
            }
            if (channelState.flags & SceneNodeStateFlags::lightColor) {
                state.flags |= SceneNodeStateFlags::lightColor;
                state.lightColor = move(channelState.lightColor);
            }
            if (channelState.flags & SceneNodeStateFlags::lightMultiplier) {
                state.flags |= SceneNodeStateFlags::lightMultiplier;
                state.lightMultiplier = channelState.lightMultiplier;
            }
            if (channelState.flags & SceneNodeStateFlags::lightRadius) {
                state.flags |= SceneNodeStateFlags::lightRadius;
                state.lightRadius = channelState.lightRadius;
            }
        }
    }

    glm::mat4 absTransform(parentTransform * localTransform);
    state.transform = absTransform;
    _stateByNumber.insert(make_pair(modelNode.nodeNumber(), move(state)));

    for (auto &child : modelNode.children()) {
        computeSceneNodeStates(*child, absTransform);
    }
}

void SceneNodeAnimator::applySceneNodeStates(ModelNode &modelNode) {
    // Do not apply transforms to skinned model nodes
    if (modelNode.skin()) return;

    auto maybeState = _stateByNumber.find(modelNode.nodeNumber());
    if (maybeState != _stateByNumber.end()) {
        const SceneNodeState &state = maybeState->second;
        ModelNodeSceneNode *sceneNode = _sceneNode->getModelNodeByIndex(modelNode.index());
        if (state.flags & SceneNodeStateFlags::transform) {
            sceneNode->setLocalTransform(state.transform);
            sceneNode->setBoneTransform(state.transform * modelNode.absoluteTransformInverse());
        }
        if (state.flags & SceneNodeStateFlags::alpha) {
            sceneNode->setAlpha(state.alpha);
        }
        if (state.flags & SceneNodeStateFlags::selfIllum) {
            sceneNode->setSelfIllumColor(state.selfIllumColor);
        }
        LightSceneNode *light = _sceneNode->getLightNodeByNumber(modelNode.nodeNumber());
        if (light) {
            if (state.flags & SceneNodeStateFlags::lightColor) {
                light->setColor(state.lightColor);
            }
            if (state.flags & SceneNodeStateFlags::lightMultiplier) {
                light->setMultiplier(state.lightMultiplier);
            }
            if (state.flags & SceneNodeStateFlags::lightRadius) {
                light->setRadius(state.lightRadius);
            }
        }
    }

    for (auto &child : modelNode.children()) {
        applySceneNodeStates(*child);
    }
}

void SceneNodeAnimator::playAnimation(const string &name, AnimationProperties properties) {
    shared_ptr<Model> model(_sceneNode->model());
    shared_ptr<Animation> anim(model->getAnimation(name));
    if (anim) {
        playAnimation(move(anim), move(properties));
    }
}

void SceneNodeAnimator::playAnimation(shared_ptr<Animation> anim, AnimationProperties properties, shared_ptr<LipAnimation> lipAnim) {
    if (!anim) return;

    _compositionMode = determineCompositionMode(properties.flags);

    // Clear composition flags
    properties.flags &= ~(AnimationFlags::blend | AnimationFlags::overlay);

    // If scale is 0.0, replace it with models scale
    if (properties.scale == 0.0f) {
        properties.scale = _sceneNode->model()->animationScale();
    }

    switch (_compositionMode) {
        case CompositionMode::Mono:
            if (!_channels[0].isSameAnimation(*anim, properties, lipAnim)) {
                // Play the specified animation on the first channel and stop animation on other channels
                _channels[0].reset(anim, properties, lipAnim);
                for (int i = 1; i < kChannelCount; ++i) {
                    _channels[i].reset();
                }
            }
            break;
        case CompositionMode::Blend:
            if (!_channels[0].isSameAnimation(*anim, properties, lipAnim)) {
                if (_channels[0].isActive()) {
                    // Play the specified animation on the first channel - previous animation is moved onto the second channel and is freezed
                    _channels[1] = _channels[0];
                    _channels[0].reset(anim, properties, lipAnim);
                    _channels[0].setTime(glm::max(0.0f, anim->transitionTime() - kTransitionDuration));
                    _channels[1].freeze();
                    _transition = true;
                } else {
                    _channels[0].reset(anim, properties, lipAnim);
                }
            }
            break;
        case CompositionMode::Overlay:
            // Play the specified animation on the first vacant channel, if any
            for (int i = 0; i < kChannelCount; ++i) {
                if (!_channels[i].isActive()) {
                    _channels[i].reset(anim, properties, lipAnim);
                    break;
                }
            }
            break;
        default:
            break;
    }
}

SceneNodeAnimator::CompositionMode SceneNodeAnimator::determineCompositionMode(int flags) const {
    return (flags & AnimationFlags::blend) ?
        CompositionMode::Blend :
        ((flags & AnimationFlags::overlay) ? CompositionMode::Overlay : CompositionMode::Mono);
}

bool SceneNodeAnimator::isAnimationFinished() const {
    return _channels[0].isFinished();
}

void SceneNodeAnimator::setDefaultAnimation(string name, AnimationProperties properties) {
    _defaultAnimName = move(name);
    _defaultAnimProperties = move(properties);
}

} // namespace scene

} // namespace reone
