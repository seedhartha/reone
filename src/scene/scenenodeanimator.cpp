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

#include "node/modelnodescenenode.h"
#include "node/modelscenenode.h"
#include "types.h"

using namespace std;

using namespace reone::render;

namespace reone {

namespace scene {

static constexpr float kTransitionDuration = 0.25f;

SceneNodeAnimator::SceneNodeAnimator(ModelSceneNode *modelSceneNode, set<string> ignoreNodes) :
    _modelSceneNode(modelSceneNode),
    _ignoreNodes(ignoreNodes) {

    if (!modelSceneNode) {
        throw invalid_argument("modelSceneNode must not be null");
    }
    for (int i = 0; i < kChannelCount; ++i) {
        _channels.push_back(AnimationChannel(modelSceneNode, ignoreNodes));
    }
}

void SceneNodeAnimator::update(float dt) {
    // Regardless of the composition mode, when there is not active animation on
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
        channel.update(dt);
    }

    // Compute and apply absolute transforms to the managed model scene node
    _transformByNodeNumber.clear();
    computeAbsoluteTransforms(*_modelSceneNode->model()->rootNode());
    applyAbsoluteTransforms(*_modelSceneNode->model()->rootNode());
}

void SceneNodeAnimator::playDefaultAnimation() {
    if (!_defaultAnimName.empty()) {
        playAnimation(_defaultAnimName, _defaultAnimProperties);
    }
}

bool SceneNodeAnimator::isInTransition() const {
    return _compositionMode == CompositionMode::Blend && _transition;
}

void SceneNodeAnimator::computeAbsoluteTransforms(ModelNode &modelNode, glm::mat4 parentTransform) {
    if (modelNode.skin()) return;

    glm::mat4 localTransform(modelNode.localTransform());

    if (isInTransition()) {
        // In the Blend mode, blend animations on the first two channels
        glm::mat4 transform1, transform2;
        bool hasTransform1 = _channels[0].getTransformByNodeNumber(modelNode.nodeNumber(), transform1);
        bool hasTransform2 = _channels[1].getTransformByNodeNumber(modelNode.nodeNumber(), transform2);
        if (hasTransform1 && hasTransform2) {
            float delta = 1.0f - (_channels[0].getTransitionTime() - _channels[0].time()) / _channels[0].getTransitionTime();
            glm::quat orientation0(glm::toQuat(transform1));
            glm::quat orientation1(glm::toQuat(transform2));
            localTransform = glm::translate(glm::mat4(1.0f), glm::vec3(transform1[3]));
            localTransform *= glm::mat4_cast(glm::slerp(orientation1, orientation0, delta));
        } else if (hasTransform1) {
            localTransform = move(transform1);
        } else if (hasTransform2) {
            localTransform = move(transform2);
        }
    } else if (_compositionMode == CompositionMode::Overlay) {
        // In the Overlay mode, select the first animation channel to have a local transform for the given node
        for (int i = kChannelCount - 1; i >= 0; --i) {
            glm::mat4 transform;
            if (_channels[i].isActive() && _channels[i].getTransformByNodeNumber(modelNode.nodeNumber(), transform)) {
                localTransform = move(transform);
                break;
            }
        }
    } else {
        // Otherwise, select animation on the first channel
        glm::mat4 transform;
        if (_channels[0].getTransformByNodeNumber(modelNode.nodeNumber(), transform)) {
            localTransform = move(transform);
        }
    }

    glm::mat4 absTransform(parentTransform * localTransform);
    _transformByNodeNumber.insert(make_pair(modelNode.nodeNumber(), absTransform));

    for (auto &child : modelNode.children()) {
        computeAbsoluteTransforms(*child, absTransform);
    }
}

void SceneNodeAnimator::applyAbsoluteTransforms(ModelNode &modelNode) {
    // Do not apply transforms to skinned model nodes
    if (modelNode.skin()) return;

    auto maybeTransform = _transformByNodeNumber.find(modelNode.nodeNumber());
    if (maybeTransform != _transformByNodeNumber.end()) {
        ModelNodeSceneNode *sceneNode = _modelSceneNode->getModelNodeByIndex(modelNode.index());
        sceneNode->setLocalTransform(maybeTransform->second);
        sceneNode->setBoneTransform(maybeTransform->second * modelNode.absoluteTransformInverse());
    }

    for (auto &child : modelNode.children()) {
        applyAbsoluteTransforms(*child);
    }
}

void SceneNodeAnimator::playAnimation(const string &name, AnimationProperties properties) {
    shared_ptr<Model> model(_modelSceneNode->model());
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
        properties.scale = _modelSceneNode->model()->animationScale();
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
                // Play the specified animation on the first channel - previous animation is moved onto the second channel and is freezed
                _channels[1] = _channels[0];
                _channels[0].reset(anim, properties, lipAnim);
                _channels[0].setTime(glm::max(0.0f, anim->transitionTime() - kTransitionDuration));
                _channels[1].freeze();
                _transition = true;
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
