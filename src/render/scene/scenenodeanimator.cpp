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

namespace reone {

namespace render {

bool SceneNodeAnimator::AnimationChannel::isSameAnimation(const string &name, int flags, float speed) const {
    return animation && animName == name && animFlags == flags && animSpeed == speed;
}

SceneNodeAnimator::SceneNodeAnimator(ModelSceneNode *modelSceneNode, const set<string> &skipNodes) :
    _modelSceneNode(modelSceneNode),
    _skipNodes(skipNodes) {

    if (!_modelSceneNode) {
        throw invalid_argument("modelSceneNode must not be null");
    }
}

void SceneNodeAnimator::update(float dt) {
    if (isAnimationFinished()) {
        playDefaultAnimation();
        return;
    }
    for (int i = 0; i < kChannelCount; ++i) {
        updateChannel(i, dt);
    }
    updateBoneTransforms(_modelSceneNode->model()->rootNode());
}

void SceneNodeAnimator::updateChannel(int channel, float dt) {
    AnimationChannel &animChannel = _channels[channel];
    if (!animChannel.animation) return;

    applyAnimationTransforms(animChannel, *animChannel.animation->rootNode());
    advanceTime(animChannel, dt);
}

void SceneNodeAnimator::applyAnimationTransforms(const AnimationChannel &channel, ModelNode &animNode) {
    ModelNodeSceneNode *sceneNode = _modelSceneNode->getModelNode(animNode.name());
    if (sceneNode) {
        ModelNode *modelNode = sceneNode->modelNode();
        glm::vec3 position(modelNode->position());
        glm::quat orientation(modelNode->orientation());

        bool skip = _skipNodes.count(modelNode->name()) > 0;
        if (!skip) {
            glm::vec3 animPosition(0.0f);
            if (animNode.getPosition(channel.animTime, animPosition, _modelSceneNode->model()->animationScale())) {
                position += animPosition;
            }
            glm::quat animOrientation(0.0f, 0.0f, 0.0f, 1.0f);
            if (animNode.getOrientation(channel.animTime, animOrientation)) {
                orientation = animOrientation;
            }
        }

        glm::mat4 transform(1.0f);
        transform = glm::translate(transform, position);
        transform *= glm::mat4_cast(orientation);
        sceneNode->setLocalTransform(transform);
    }

    for (auto &child : animNode.children()) {
        applyAnimationTransforms(channel, *child);
    }
}

void SceneNodeAnimator::updateBoneTransforms(ModelNode &modelNode) {
    ModelNodeSceneNode *sceneNode = _modelSceneNode->getModelNodeByIndex(modelNode.index());

    glm::mat4 boneTransform(_modelSceneNode->absoluteTransformInverse());
    boneTransform *= sceneNode->absoluteTransform();
    boneTransform *= modelNode.absoluteTransformInverse();
    sceneNode->setBoneTransform(boneTransform);

    for (auto &child : modelNode.children()) {
        updateBoneTransforms(*child);
    }
}

void SceneNodeAnimator::advanceTime(AnimationChannel &channel, float dt) {
    float length = channel.animation->length();
    channel.animTime += channel.animSpeed * dt;

    bool loop = channel.animFlags & kAnimationLoop;
    if (loop) {
        channel.animTime = glm::mod(channel.animTime, length);
    } else {
        channel.animTime = glm::min(channel.animTime, length);
        if (channel.animTime == length) {
            channel.animFinished = true;
        }
    }
}

void SceneNodeAnimator::playDefaultAnimation() {
    if (_defaultAnim.empty()) return;

    playAnimation(_defaultAnim, kAnimationLoop);
}

void SceneNodeAnimator::playAnimation(const string &name, int flags, float speed) {
    for (int i = 1; i < kChannelCount; ++i) {
        stopAnimation(i);
    }
    playAnimation(0, name, flags, speed);
}

void SceneNodeAnimator::stopAnimation(int channel) {
    _channels[channel].animation = nullptr;
}

void SceneNodeAnimator::playAnimation(int channel, const string &name, int flags, float speed) {
    AnimationChannel &animChannel = _channels[channel];
    if (animChannel.isSameAnimation(name, flags, speed)) return;

    animChannel.animName = name;
    animChannel.animFlags = flags;
    animChannel.animSpeed = speed;
    animChannel.animTime = 0.0f;
    animChannel.animation = _modelSceneNode->model()->getAnimation(name);
    animChannel.animFinished = false;
}

bool SceneNodeAnimator::isAnimationFinished() const {
    for (int i = 0; i < kChannelCount; ++i) {
        const AnimationChannel &channel = _channels[i];
        if (channel.animation && !channel.animFinished) return false;
    }
    return true;
}

void SceneNodeAnimator::setDefaultAnimation(const string &name) {
    _defaultAnim = name;
}

} // namespace render

} // namespace reone
