/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "modelnodescenenode.h"
#include "modelscenenode.h"

using namespace std;

using namespace reone::render;

namespace reone {

namespace scene {

static const float kTransitionDuration = 0.25f;

bool SceneNodeAnimator::AnimationChannel::isActive() const {
    return animation && !finished;
}

bool SceneNodeAnimator::AnimationChannel::isSameAnimation(const string &name, int flags, float speed) const {
    return
        this->animation &&
        this->name == name &&
        this->flags == flags &&
        this->speed == speed;
}

void SceneNodeAnimator::AnimationChannel::setAnimation(Animation *animation, int flags, float speed) {
    this->name = animation->name();
    this->flags = flags;
    this->speed = speed;
    this->time = 0.0f;
    this->animation = animation;
    this->finished = false;
    this->transition = false;
    this->freeze = false;
}

void SceneNodeAnimator::AnimationChannel::stopAnimation() {
    animation = nullptr;
}

SceneNodeAnimator::SceneNodeAnimator(ModelSceneNode *modelSceneNode, const set<string> &skipNodes) :
    _modelSceneNode(modelSceneNode),
    _skipNodes(skipNodes) {

    if (!_modelSceneNode) {
        throw invalid_argument("modelSceneNode must not be null");
    }
}

void SceneNodeAnimator::update(float dt) {
    if (!_channels[0].isActive()) {
        playDefaultAnimation();
        return;
    }
    if (_channels[0].transition && _channels[0].time >= _channels[0].animation->transitionTime()) {
        _channels[0].transition = false;
        _channels[1].stopAnimation();
    }
    for (int i = 0; i < kChannelCount; ++i) {
        updateChannel(i, dt);
    }
    _absTransforms.clear();
    updateAbsoluteTransforms(_modelSceneNode->model()->rootNode());
    updateNodeTransforms(_modelSceneNode->model()->rootNode());
}

void SceneNodeAnimator::updateChannel(int channel, float dt) {
    AnimationChannel &animChannel = _channels[channel];
    if (!animChannel.isActive()) return;

    animChannel.localTransforms.clear();
    updateLocalTransforms(animChannel, *animChannel.animation->rootNode());
    advanceTime(animChannel, dt);
}

void SceneNodeAnimator::updateLocalTransforms(AnimationChannel &channel, ModelNode &animNode) {
    ModelNodeSceneNode *sceneNode = _modelSceneNode->getModelNode(animNode.name());
    if (sceneNode) {
        ModelNode *modelNode = sceneNode->modelNode();
        glm::vec3 position(modelNode->position());
        glm::quat orientation(modelNode->orientation());

        bool skip = _skipNodes.count(modelNode->name()) > 0;
        if (!skip) {
            float time = channel.transition ? channel.animation->transitionTime() : channel.time;
            glm::vec3 animPosition(0.0f);
            if (animNode.getPosition(time, animPosition, _modelSceneNode->model()->animationScale())) {
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

    glm::mat4 transform(parentTransform);
    glm::mat4 localTransform;

    if (_channels[0].transition) {
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
        } else {
            localTransform = modelNode.localTransform();
        }
    } else if (_channels[0].flags & kAnimationOverlay) {
        auto maybeTransform0 = _channels[0].localTransforms.find(modelNode.nodeNumber());
        bool hasTransform0 = maybeTransform0 != _channels[0].localTransforms.end();
        auto maybeTransform1 = _channels[1].localTransforms.find(modelNode.nodeNumber());
        bool hasTransform1 = maybeTransform1 != _channels[1].localTransforms.end();
        if (hasTransform0) {
            localTransform = maybeTransform0->second;
        } else if (hasTransform1) {
            localTransform = maybeTransform1->second;
        } else {
            localTransform = modelNode.localTransform();
        }
    } else {
        auto maybeTransform = _channels[0].localTransforms.find(modelNode.nodeNumber());
        if (maybeTransform != _channels[0].localTransforms.end()) {
            localTransform = maybeTransform->second;
        } else {
            localTransform = modelNode.localTransform();
        }
    }

    transform *= localTransform;
    _absTransforms.insert(make_pair(modelNode.nodeNumber(), transform));

    for (auto &child : modelNode.children()) {
        updateAbsoluteTransforms(*child, transform);
    }
}

void SceneNodeAnimator::updateNodeTransforms(ModelNode &modelNode) {
    if (modelNode.skin()) return;

    glm::mat4 transform(_absTransforms.find(modelNode.nodeNumber())->second);

    ModelNodeSceneNode *sceneNode = _modelSceneNode->getModelNodeByIndex(modelNode.index());
    sceneNode->setLocalTransform(transform);
    sceneNode->setBoneTransform(transform * modelNode.absoluteTransformInverse());

    for (auto &child : modelNode.children()) {
        updateNodeTransforms(*child);
    }
}

void SceneNodeAnimator::advanceTime(AnimationChannel &channel, float dt) {
    if (channel.freeze) return;

    float length = channel.animation->length();
    channel.time += channel.speed * dt;

    bool loop = channel.flags & kAnimationLoop;
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
    if (_defaultAnim.empty()) return;

    playAnimation(_defaultAnim, kAnimationLoop | kAnimationBlend);
}

void SceneNodeAnimator::playAnimation(const string &name, int flags, float speed) {
    if (_channels[0].isSameAnimation(name, flags, speed)) return;

    Animation *animation = _modelSceneNode->model()->getAnimation(name);
    if (!animation) return;

    for (int i = 1; i < kChannelCount; ++i) {
        _channels[i].stopAnimation();
    }

    bool set = false;
    if (_channels[0].isActive()) {
        if (flags & kAnimationOverlay) {
            _channels[1] = _channels[0];
        } else if (flags & kAnimationBlend) {
            _channels[1] = _channels[0];
            _channels[0].setAnimation(animation, flags, speed);
            _channels[0].time = glm::max(0.0f, animation->transitionTime() - kTransitionDuration);
            _channels[0].transition = true;
            _channels[1].transition = false;
            _channels[1].freeze = true;

            set = true;
        }
    }
    if (!set) {
        _channels[0].setAnimation(animation, flags, speed);
    }
}

bool SceneNodeAnimator::isAnimationFinished() const {
    return !_channels[0].isActive();
}

void SceneNodeAnimator::setDefaultAnimation(const string &name) {
    _defaultAnim = name;
}

} // namespace scene

} // namespace reone
