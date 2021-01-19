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

#include <stack>
#include <stdexcept>

#include "node/modelnodescenenode.h"
#include "node/modelscenenode.h"
#include "types.h"

using namespace std;

using namespace reone::render;

namespace reone {

namespace scene {

static constexpr float kTransitionDuration = 0.25f;

bool SceneNodeAnimator::AnimationChannel::isActive() const {
    return animation && !finished;
}

bool SceneNodeAnimator::AnimationChannel::isSameAnimation(const shared_ptr<Animation> &anim, int flags, float speed, float scale) const {
    return
        this->animation == anim &&
        this->flags == flags &&
        this->speed == speed &&
        this->scale == scale;
}

void SceneNodeAnimator::AnimationChannel::stopAnimation() {
    animation = nullptr;
}

void SceneNodeAnimator::AnimationChannel::setAnimation(const shared_ptr<Animation> &anim, int flags, float speed, float scale) {
    this->animation = anim;
    this->flags = flags;
    this->speed = speed;
    this->scale = scale;
    this->time = 0.0f;
    this->finished = false;
    this->freeze = false;
}

SceneNodeAnimator::SceneNodeAnimator(ModelSceneNode *modelSceneNode, const set<string> &skipNodes) :
    _modelSceneNode(modelSceneNode),
    _skipNodes(skipNodes) {

    if (!modelSceneNode) {
        throw invalid_argument("modelSceneNode must not be null");
    }
}

void SceneNodeAnimator::update(float dt) {
    // Regardless of the mode, when the animation in the first channel finishes
    // playing, start the default animation
    if (!_channels[0].isActive()) {
        playDefaultAnimation();
        return;
    }

    // In the Blend mode, if the animation in the first channel is past
    // transition time, stop animation in the second channel
    if (isInTransition() && _channels[0].time >= _channels[0].animation->transitionTime()) {
        _channels[1].stopAnimation();
        _transition = false;
    }

    // Update animation channels
    for (int i = 0; i < kChannelCount; ++i) {
        updateChannel(i, dt);
    }

    // Apply animation transforms to the managed scene node
    _absTransforms.clear();
    updateAbsoluteTransforms(_modelSceneNode->model()->rootNode());
    applyAnimationTransforms(_modelSceneNode->model()->rootNode());
}

void SceneNodeAnimator::updateChannel(int channel, float dt) {
    AnimationChannel &animChannel = _channels[channel];
    if (animChannel.isActive()) {
        animChannel.localTransforms.clear();
        updateLocalTransforms(animChannel, *animChannel.animation->rootNode());
        advanceTime(animChannel, dt);
    }
}

void SceneNodeAnimator::updateLocalTransforms(AnimationChannel &channel, ModelNode &animNode) {
    ModelNodeSceneNode *sceneNode = _modelSceneNode->getModelNode(animNode.name());
    if (sceneNode) {
        ModelNode *modelNode = sceneNode->modelNode();
        glm::vec3 position(modelNode->position());
        glm::quat orientation(modelNode->orientation());

        bool skip = _skipNodes.count(modelNode->name()) > 0;
        if (!skip) {
            float time = isInTransition() ? channel.animation->transitionTime() : channel.time;
            glm::vec3 animPosition(0.0f);
            if (animNode.getPosition(time, animPosition, channel.scale)) {
                position += animPosition;
            }
            glm::quat animOrientation(0.0f, 0.0f, 0.0f, 1.0f);
            if (animNode.getOrientation(time, animOrientation)) {
                orientation = animOrientation;
            }
        }

        glm::mat4 transform(1.0f);
        transform = glm::translate(transform, position);
        transform *= glm::mat4_cast(orientation);
        channel.localTransforms.insert(make_pair(modelNode->nodeNumber(), transform));
    }

    for (auto &child : animNode.children()) {
        updateLocalTransforms(channel, *child);
    }
}

void SceneNodeAnimator::updateAbsoluteTransforms(ModelNode &modelNode, const glm::mat4 &parentTransform) {
    if (modelNode.skin()) return;

    glm::mat4 localTransform(modelNode.localTransform());

    if (isInTransition()) {
        auto maybeTransform0 = _channels[0].localTransforms.find(modelNode.nodeNumber());
        bool hasTransform0 = maybeTransform0 != _channels[0].localTransforms.end();
        auto maybeTransform1 = _channels[1].localTransforms.find(modelNode.nodeNumber());
        bool hasTransform1 = maybeTransform1 != _channels[1].localTransforms.end();
        if (hasTransform0 && hasTransform1) {
            float delta = 1.0f - (_channels[0].animation->transitionTime() - _channels[0].time) / _channels[0].animation->transitionTime();
            glm::quat orientation0(glm::toQuat(maybeTransform0->second));
            glm::quat orientation1(glm::toQuat(maybeTransform1->second));
            localTransform = glm::translate(glm::mat4(1.0f), glm::vec3(maybeTransform0->second[3]));
            localTransform *= glm::mat4_cast(glm::slerp(orientation1, orientation0, delta));
        } else if (hasTransform0) {
            localTransform = maybeTransform0->second;
        } else if (hasTransform1) {
            localTransform = maybeTransform1->second;
        }
    } else if (_mode == Mode::Overlay) {
        // In the Overlay mode, select first animation channel to have a local transform for the given node
        for (int i = kChannelCount - 1; i >= 0; --i) {
            if (_channels[i].isActive()) {
                auto maybeTransform = _channels[i].localTransforms.find(modelNode.nodeNumber());
                if (maybeTransform != _channels[i].localTransforms.end()) {
                    localTransform = maybeTransform->second;
                    break;
                }
            }
        }
    } else {
        auto maybeTransform = _channels[0].localTransforms.find(modelNode.nodeNumber());
        if (maybeTransform != _channels[0].localTransforms.end()) {
            localTransform = maybeTransform->second;
        }
    }

    glm::mat4 absTransform(parentTransform * localTransform);
    _absTransforms.insert(make_pair(modelNode.nodeNumber(), absTransform));

    for (auto &child : modelNode.children()) {
        updateAbsoluteTransforms(*child, absTransform);
    }
}

void SceneNodeAnimator::applyAnimationTransforms(ModelNode &modelNode) {
    if (modelNode.skin()) return;

    glm::mat4 transform(_absTransforms.find(modelNode.nodeNumber())->second);

    ModelNodeSceneNode *sceneNode = _modelSceneNode->getModelNodeByIndex(modelNode.index());
    sceneNode->setLocalTransform(transform);
    sceneNode->setBoneTransform(transform * modelNode.absoluteTransformInverse());

    for (auto &child : modelNode.children()) {
        applyAnimationTransforms(*child);
    }
}

void SceneNodeAnimator::advanceTime(AnimationChannel &channel, float dt) {
    if (channel.freeze) return;

    float length = channel.animation->length();
    channel.time += channel.speed * dt;

    bool loop = channel.flags & AnimationFlags::loop;
    if (loop) {
        channel.time = glm::mod(channel.time, length);
    } else {
        channel.time = glm::min(channel.time, length);
        if (channel.time == length) {
            channel.finished = true;
        }
    }
}

void SceneNodeAnimator::playDefaultAnimation() {
    if (_defaultAnim) {
        playAnimation(_defaultAnim, _defaultAnimFlags, _defaultAnimSpeed);
    }
}

void SceneNodeAnimator::playAnimation(const shared_ptr<Animation> &anim, int flags, float speed, float scale) {
    Mode mode = getMode(flags);

    // Clear animation flags
    flags &= ~(AnimationFlags::blend | AnimationFlags::overlay);

    _mode = mode;

    switch (mode) {
        case Mode::Mono:
            if (!_channels[0].isSameAnimation(anim, flags, speed, scale)) {
                // Play the specified animation on the first channel - stop animation on other channels
                _channels[0].setAnimation(anim, flags, speed, scale);
                for (int i = 1; i < kChannelCount; ++i) {
                    _channels[i].stopAnimation();
                }
            }
            break;
        case Mode::Blend:
            if (!_channels[0].isSameAnimation(anim, flags, speed, scale)) {
                // Play the specified animation on the first channel - previous animation is moved onto the second channel and is freezed.
                _channels[1] = _channels[0];
                _channels[0].setAnimation(anim, flags, speed, scale);
                _channels[0].time = glm::max(0.0f, anim->transitionTime() - kTransitionDuration);
                _channels[1].freeze = true;
                _transition = true;
            }
            break;
        case Mode::Overlay:
            // Play the specified animation on the first vacant channel, if any
            for (int i = 0; i < kChannelCount; ++i) {
                if (!_channels[i].isActive()) {
                    _channels[i].setAnimation(anim, flags, speed, scale);
                    break;
                }
            }
            break;
        default:
            break;
    }
}

bool SceneNodeAnimator::isAnimationFinished() const {
    return !_channels[0].isActive();
}

void SceneNodeAnimator::setDefaultAnimation(const shared_ptr<Animation> &anim, int flags, float speed) {
    _defaultAnim = anim;
    _defaultAnimFlags = flags;
    _defaultAnimSpeed = speed;
}

bool SceneNodeAnimator::isInTransition() const {
    return _mode == Mode::Blend && _transition;
}

SceneNodeAnimator::Mode SceneNodeAnimator::getMode(int animFlags) const {
    return (animFlags & AnimationFlags::blend) ?
        Mode::Blend :
        ((animFlags & AnimationFlags::overlay) ? Mode::Overlay : Mode::Mono);
}

} // namespace scene

} // namespace reone
