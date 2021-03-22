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

#include "../../common/log.h"

using namespace std;

namespace reone {

namespace render {

ModelNode::ModelNode(int index, const ModelNode *parent) : _index(index), _parent(parent) {
}

void ModelNode::initGL() {
    if (_mesh) {
        _mesh->initGL();
    }
    for (auto &child : _children) {
        child->initGL();
    }
}

void ModelNode::addChild(shared_ptr<ModelNode> child) {
    child->_parent = this;
    _children.push_back(move(child));
}

void ModelNode::computeLocalTransforms() {
    if (_parent) {
        _localTransform = glm::inverse(_parent->_absTransform) * _absTransform;
        _absTransform = _parent->_absTransform * _localTransform;
        _absTransformInv = glm::inverse(_absTransform);

        // Extract position and orientation for use in animations.
        glm::vec3 scale, skew;
        glm::vec4 perspective;
        glm::decompose(_localTransform, scale, _orientation, _position, skew, perspective);

    } else {
        _localTransform = _absTransform;
    }

    _absTransform = _parent ? _parent->_absTransform * _localTransform : _localTransform;
    _absTransformInv = glm::inverse(_absTransform);

    for (auto &child : _children) {
        child->computeLocalTransforms();
    }
}

void ModelNode::computeAbsoluteTransforms() {
    _absTransform = _parent ? _parent->_absTransform * _localTransform : _localTransform;
    _absTransformInv = glm::inverse(_absTransform);

    for (auto &child : _children) {
        child->computeAbsoluteTransforms();
    }
}

void ModelNode::addTranslationKeyframe(Keyframe keyframe) {
    _translationFrames.push_back(move(keyframe));
}

void ModelNode::addOrientationKeyframe(Keyframe keyframe) {
    _orientationFrames.push_back(move(keyframe));
}

void ModelNode::addScaleKeyframe(Keyframe keyframe) {
    _scaleFrames.push_back(move(keyframe));
}

void ModelNode::addAlphaKeyframe(Keyframe keyframe) {
    _alphaFrames.push_back(move(keyframe));
}

void ModelNode::addSelfIllumColorKeyframe(Keyframe keyframe) {
    _selfIllumFrames.push_back(move(keyframe));
}

/**
 * @return false if keyframes vector is empty, true otherwise
 */
static bool getKeyframes(
    const vector<ModelNode::Keyframe> &frames, float time,
    const ModelNode::Keyframe *&frame1, const ModelNode::Keyframe *&frame2, float &factor) {

    if (frames.empty()) return false;

    frame1 = &frames[0];
    frame2 = &frames[0];

    for (auto it = frames.begin(); it != frames.end(); ++it) {
        if (it->time >= time) {
            frame2 = &*it;
            if (it != frames.begin()) {
                frame1 = &*(it - 1);
            }
            break;
        }
    }

    if (frame1 == frame2) {
        factor = 0.0f;
    } else {
        factor = (time - frame1->time) / (frame2->time - frame1->time);
    }

    return true;
}

bool ModelNode::getTranslation(float time, glm::vec3 &translation, float scale) const {
    const Keyframe *frame1, *frame2;
    float factor;

    if (getKeyframes(_translationFrames, time, frame1, frame2, factor)) {
        translation = glm::mix(frame1->translation, frame2->translation, factor) * scale;
        return true;
    }

    return false;
}

bool ModelNode::getOrientation(float time, glm::quat &orientation) const {
    const Keyframe *frame1, *frame2;
    float factor;

    if (getKeyframes(_orientationFrames, time, frame1, frame2, factor)) {
        orientation = glm::slerp(frame1->orientation, frame2->orientation, factor);
        return true;
    }

    return false;
}

bool ModelNode::getScale(float time, float &scale) const {
    const Keyframe *frame1, *frame2;
    float factor;

    if (getKeyframes(_scaleFrames, time, frame1, frame2, factor)) {
        scale = glm::mix(frame1->scale, frame2->scale, factor);
        return true;
    }

    return false;
}

bool ModelNode::getAlpha(float time, float &alpha) const {
    const Keyframe *frame1, *frame2;
    float factor;

    if (getKeyframes(_alphaFrames, time, frame1, frame2, factor)) {
        alpha = glm::mix(frame1->alpha, frame2->alpha, factor);
        return true;
    }

    return false;
}

bool ModelNode::getSelfIllumColor(float time, glm::vec3 &color) const {
    const Keyframe *frame1, *frame2;
    float factor;

    if (getKeyframes(_selfIllumFrames, time, frame1, frame2, factor)) {
        color = glm::mix(frame1->selfIllumColor, frame2->selfIllumColor, factor);
        return true;
    }

    return false;
}

bool ModelNode::getTranslation(int leftFrameIdx, int rightFrameIdx, float interpolant, glm::vec3 &translation, float scale) const {
    if (leftFrameIdx < 0 || leftFrameIdx >= static_cast<int>(_translationFrames.size()) ||
        rightFrameIdx < 0 || rightFrameIdx >= static_cast<int>(_translationFrames.size())) return false;

    if (leftFrameIdx == rightFrameIdx) {
        translation = scale * _translationFrames[leftFrameIdx].translation;
    } else {
        translation = scale * glm::mix(
            _translationFrames[leftFrameIdx].translation,
            _translationFrames[rightFrameIdx].translation,
            interpolant);
    }

    return true;
}

bool ModelNode::getOrientation(int leftFrameIdx, int rightFrameIdx, float interpolant, glm::quat &orientation) const {
    if (leftFrameIdx < 0 || leftFrameIdx >= static_cast<int>(_orientationFrames.size()) ||
        rightFrameIdx < 0 || rightFrameIdx >= static_cast<int>(_orientationFrames.size())) return false;

    if (leftFrameIdx == rightFrameIdx) {
        orientation = _orientationFrames[leftFrameIdx].orientation;
    } else {
        orientation = glm::mix(
            _orientationFrames[leftFrameIdx].orientation,
            _orientationFrames[rightFrameIdx].orientation,
            interpolant);
    }

    return true;
}

bool ModelNode::getScale(int leftFrameIdx, int rightFrameIdx, float interpolant, float &scale) const {
    if (leftFrameIdx < 0 || leftFrameIdx >= static_cast<int>(_scaleFrames.size()) ||
        rightFrameIdx < 0 || rightFrameIdx >= static_cast<int>(_scaleFrames.size())) return false;

    if (leftFrameIdx == rightFrameIdx) {
        scale = _scaleFrames[leftFrameIdx].scale;
    } else {
        scale = glm::mix(
            _scaleFrames[leftFrameIdx].scale,
            _scaleFrames[rightFrameIdx].scale,
            interpolant);
    }

    return true;
}

const glm::vec3 &ModelNode::getCenterOfAABB() const {
    return _mesh->mesh()->aabb().center();
}

void ModelNode::setName(string name) {
    _name = move(name);
}

void ModelNode::setNodeNumber(uint16_t nodeNumber) {
    _nodeNumber = nodeNumber;
}

void ModelNode::setAbsoluteTransform(glm::mat4 transform) {
    _absTransform = move(transform);
    _absTransformInv = glm::inverse(_absTransform);
}

void ModelNode::setMesh(shared_ptr<ModelMesh> mesh) {
    _mesh = move(mesh);
}

void ModelNode::setSkin(shared_ptr<Skin> skin) {
    _skin = move(skin);
}

void ModelNode::setColor(glm::vec3 color) {
    _color = move(color);
}

void ModelNode::setSelfIllumColor(glm::vec3 color) {
    _selfIllumColor = move(color);
}

void ModelNode::setAlpha(float alpha) {
    _alpha = alpha;
}

} // namespace render

} // namespace reone
