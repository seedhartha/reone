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

#include "channel.h"

#include <stdexcept>

#include "glm/common.hpp"

#include "../node/modelnodescenenode.h"
#include "../node/modelscenenode.h"
#include "../types.h"

using namespace std;

using namespace reone::render;

namespace reone {

namespace scene {

AnimationChannel::AnimationChannel(ModelSceneNode *modelSceneNode, set<string> ignoreNodes) :
    _modelSceneNode(modelSceneNode),
    _ignoreNodes(move(ignoreNodes)) {

    if (!modelSceneNode) {
        throw invalid_argument("modelSceneNode must not be null");
    }
}

void AnimationChannel::reset() {
    _animation.reset();
    _lipAnimation.reset();
    _time = 0.0f;
    _freeze = false;
    _finished = false;
}

void AnimationChannel::reset(shared_ptr<Animation> anim, AnimationProperties properties, shared_ptr<LipAnimation> lipAnim) {
    if (!anim) {
        throw invalid_argument("anim must not be null");
    }
    _animation = move(anim);
    _properties = move(properties);
    _lipAnimation = move(lipAnim);
    _time = 0.0f;
    _freeze = false;
    _finished = false;
}

void AnimationChannel::update(float dt) {
    if (!_animation || _freeze || _finished) return;

    float newTime, length;

    if (_properties.flags & AnimationFlags::syncLipAnim) {
        length = _lipAnimation->length();
        newTime = glm::min(_time + dt, length);

    } else {
        length = _animation->length();
        newTime = glm::min(_time + _properties.speed * dt, length);

        // Signal animation events between the previous time and the current time
        for (auto &event : _animation->events()) {
            if (_time < event.time && event.time <= newTime) {
                _modelSceneNode->signalEvent(event.name);
            }
        }
    }

    _stateByNumber.clear();
    computeSceneNodeStates(*_animation->rootNode());

    _time = newTime;

    if (_time == length) {
        bool loop = _properties.flags & AnimationFlags::loop;
        bool syncLipAnim = _properties.flags & AnimationFlags::syncLipAnim;
        if (loop && !syncLipAnim) {
            _time = 0.0f;
        } else {
            _finished = true;
        }
    }
}

void AnimationChannel::computeSceneNodeStates(const ModelNode &animNode) {
    if (_ignoreNodes.count(animNode.name()) == 0) {
        ModelNodeSceneNode *modelNodeSceneNode = _modelSceneNode->getModelNode(animNode.name());
        if (modelNodeSceneNode) {
            const ModelNode *modelNode = modelNodeSceneNode->modelNode();
            bool transformChanged = false;
            float scale = 1.0f;
            glm::vec3 translation(modelNode->position());
            glm::quat orientation(modelNode->orientation());

            if (_properties.flags & AnimationFlags::syncLipAnim) {
                uint8_t leftFrameIdx, rightFrameIdx;
                float interpolant;
                if (_lipAnimation->getKeyframes(_time, leftFrameIdx, rightFrameIdx, interpolant)) {
                    float animScale;
                    if (animNode.getScale(leftFrameIdx, rightFrameIdx, interpolant, animScale)) {
                        scale = animScale;
                        transformChanged = true;
                    }
                    glm::vec3 animTranslation;
                    if (animNode.getTranslation(leftFrameIdx, rightFrameIdx, interpolant, animTranslation, _properties.scale)) {
                        translation += animTranslation;
                        transformChanged = true;
                    }
                    glm::quat animOrientation;
                    if (animNode.getOrientation(leftFrameIdx, rightFrameIdx, interpolant, animOrientation)) {
                        orientation = move(animOrientation);
                        transformChanged = true;
                    }
                }
            } else {
                float animScale;
                if (animNode.getScale(_time, animScale)) {
                    scale = animScale;
                    transformChanged = true;
                }
                glm::vec3 animTranslation;
                if (animNode.getTranslation(_time, animTranslation, _properties.scale)) {
                    translation += animTranslation;
                    transformChanged = true;
                }
                glm::quat animOrientation;
                if (animNode.getOrientation(_time, animOrientation)) {
                    orientation = move(animOrientation);
                    transformChanged = true;
                }
            }

            SceneNodeState state;
            float alpha;
            if (animNode.getAlpha(_time, alpha)) {
                state.flags |= SceneNodeStateFlags::alpha;
                state.alpha = alpha;
            }
            glm::vec3 selfIllumColor;
            if (animNode.getSelfIllumColor(_time, selfIllumColor)) {
                state.flags |= SceneNodeStateFlags::selfIllum;
                state.selfIllumColor = move(selfIllumColor);
            }
            if (transformChanged) {
                glm::mat4 transform(1.0f);
                transform = glm::scale(transform, glm::vec3(scale));
                transform = glm::translate(transform, translation);
                transform *= glm::mat4_cast(orientation);
                state.flags |= SceneNodeStateFlags::transform;
                state.transform = move(transform);
            }
            _stateByNumber.insert(make_pair(modelNode->nodeNumber(), move(state)));
        }
    }

    for (auto &child : animNode.children()) {
        computeSceneNodeStates(*child);
    }
}

void AnimationChannel::freeze() {
    _freeze = true;
}

bool AnimationChannel::isSameAnimation(const Animation &anim, const AnimationProperties &properties, shared_ptr<LipAnimation> lipAnim) const {
    return _animation.get() == &anim && _properties == properties && _lipAnimation == lipAnim;
}

bool AnimationChannel::isActive() const {
    return _animation && !_finished;
}

bool AnimationChannel::isPastTransitionTime() const {
    return _animation && _time > _animation->transitionTime();
}

bool AnimationChannel::isFinished() const {
    return _animation && _finished;
}

float AnimationChannel::getTransitionTime() const {
    return _animation ? _animation->transitionTime() : 0.0f;
}

bool AnimationChannel::getSceneNodeStateByNumber(uint16_t nodeNumber, SceneNodeState &state) const {
    auto maybeState = _stateByNumber.find(nodeNumber);
    if (maybeState != _stateByNumber.end()) {
        state = maybeState->second;
        return true;
    }
    return false;
}

void AnimationChannel::setTime(float time) {
    _time = time;
}

} // namespace scene

} // namespace reone
