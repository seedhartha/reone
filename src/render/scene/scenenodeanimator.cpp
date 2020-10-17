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

SceneNodeAnimator::SceneNodeAnimator(ModelSceneNode *modelSceneNode, const set<string> &skipNodes) :
    _modelSceneNode(modelSceneNode),
    _skipNodes(skipNodes) {

    if (!_modelSceneNode) {
        throw invalid_argument("modelSceneNode must not be null");
    }
}

void SceneNodeAnimator::update(float dt) {
    if (!_animation) return;

    updateModelNodes();
    advanceTime(dt);

    if (_animFinished) {
        playDefaultAnimation();
    }
}

void SceneNodeAnimator::updateModelNodes() {
    applyAnimationTransforms(*_animation->rootNode());
    updateBoneTransforms(_modelSceneNode->model()->rootNode());
}

void SceneNodeAnimator::applyAnimationTransforms(ModelNode &animNode) {
    ModelNodeSceneNode *sceneNode = _modelSceneNode->getModelNode(animNode.name());
    if (sceneNode) {
        ModelNode *modelNode = sceneNode->modelNode();
        glm::vec3 position(modelNode->position());
        glm::quat orientation(modelNode->orientation());

        bool skip = _skipNodes.count(modelNode->name()) > 0;
        if (!skip) {
            glm::vec3 animPosition(0.0f);
            if (animNode.getPosition(_animTime, animPosition, _modelSceneNode->model()->animationScale())) {
                position += animPosition;
            }
            glm::quat animOrientation(0.0f, 0.0f, 0.0f, 1.0f);
            if (animNode.getOrientation(_animTime, animOrientation)) {
                orientation = animOrientation;
            }
        }

        glm::mat4 transform(1.0f);
        transform = glm::translate(transform, position);
        transform *= glm::mat4_cast(orientation);
        sceneNode->setLocalTransform(transform);
    }

    for (auto &child : animNode.children()) {
        applyAnimationTransforms(*child);
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

void SceneNodeAnimator::advanceTime(float dt) {
    float length = _animation->length();
    _animTime += _animSpeed * dt, length;

    bool loop = _animFlags & kAnimationLoop;
    if (loop) {
        _animTime = glm::mod(_animTime, length);
    } else {
        _animTime = glm::min(_animTime, length);
        if (_animTime == length) {
            _animFinished = true;
        }
    }
}

void SceneNodeAnimator::playDefaultAnimation() {
    if (_defaultAnim.empty()) return;

    playAnimation(_defaultAnim, kAnimationLoop);
}

void SceneNodeAnimator::playAnimation(const string &name, int flags, float speed) {
    if (_animName == name && _animFlags == flags && _animSpeed == speed) return;

    _animName = name;
    _animFlags = flags;
    _animSpeed = speed;
    _animTime = 0.0f;
    _animation = _modelSceneNode->model()->getAnimation(name);
    _animFinished = false;
}

bool SceneNodeAnimator::isAnimationFinished() const {
    return _animFinished;
}

void SceneNodeAnimator::setDefaultAnimation(const string &name) {
    _defaultAnim = name;
}

} // namespace render

} // namespace reone
