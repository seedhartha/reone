#include "modelnode.h"

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

int ModelNode::index() const {
    return _index;
}

const ModelNode *ModelNode::parent() const {
    return _parent;
}

uint16_t ModelNode::nodeNumber() const {
    return _nodeNumber;
}

const std::string &ModelNode::name() const {
    return _name;
}

const glm::vec3 &ModelNode::position() const {
    return _position;
}

const glm::quat &ModelNode::orientation() const {
    return _orientation;
}

const glm::mat4 &ModelNode::absoluteTransform() const {
    return _absTransform;
}

const glm::mat4 &ModelNode::absoluteTransformInverse() const {
    return _absTransformInv;
}

bool ModelNode::getPosition(float time, glm::vec3 &position) const {
    if (_positionFrames.empty()) return false;

    const PositionKeyframe *left = &_positionFrames.front();
    const PositionKeyframe *right = left;

    for (auto it = _positionFrames.begin(); it != _positionFrames.end(); ++it) {
        if (it->time >= time) {
            right = &*it;
            if (it != _positionFrames.begin()) left = &*(it - 1);
            break;
        }
    }

    if (left == right) {
        position = left->position;
        return true;
    }

    float factor = (time - left->time) / (right->time - left->time);

    position = glm::mix(left->position, right->position, factor);

    return true;
}

bool ModelNode::getOrientation(float time, glm::quat &orientation) const {
    if (_orientationFrames.empty()) return false;

    const OrientationKeyframe *left = &_orientationFrames.front();
    const OrientationKeyframe *right = left;

    for (auto it = _orientationFrames.begin(); it != _orientationFrames.end(); ++it) {
        if (it->time >= time) {
            right = &*it;
            if (it != _orientationFrames.begin()) left = &*(it - 1);
            break;
        }
    }

    if (left == right) {
        orientation = left->orientation;
        return true;
    }

    float factor = (time - left->time) / (right->time - left->time);

    orientation = glm::slerp(left->orientation, right->orientation, factor);

    return true;
}

float ModelNode::alpha() const {
    return _alpha;
}

std::shared_ptr<ModelMesh> ModelNode::mesh() const {
    return _mesh;
}

std::shared_ptr<ModelNode::Skin> ModelNode::skin() const {
    return _skin;
}

const std::vector<std::shared_ptr<ModelNode>> &ModelNode::children() const {
    return _children;
}

} // namespace render

} // namespace reone
