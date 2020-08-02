#include "object.h"

#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/norm.hpp"

using namespace reone::render;
using namespace reone::resources;

namespace reone {

namespace game {

Object::Object(uint32_t id) : _id(id) {
}

void Object::animate(const std::string &anim, int flags) {
    if (_model) {
        _model->animate(anim, flags);
    }
}

void Object::update(const UpdateContext &ctx) {
    if (_model) {
        glm::vec4 viewport(-1.0f, -1.0f, 1.0f, 1.0f);
        glm::vec3 screenCoords = glm::project(_position, ctx.view, ctx.projection, viewport);
        float distanceToCamera = glm::distance2(_position, ctx.cameraPosition);
        bool visible = distanceToCamera < _drawDistance && screenCoords.z < 1.0f;
        float alpha = 1.0f;

        if (_drawDistance != _fadeDistance && distanceToCamera > _fadeDistance) {
            alpha = 1.0f - (distanceToCamera - _fadeDistance) / (_drawDistance - _fadeDistance);
        }
        if (visible) {
            _model->show();
        } else {
            _model->hide();
        }
        _model->setAlpha(alpha);
        _model->update(ctx.deltaTime);
    }
}

void Object::initGL() {
    if (_model) {
        _model->initGL();
    }
}

void Object::saveTo(AreaState &state) const {
}

void Object::loadState(const AreaState &state) {
}

float Object::distanceTo(const glm::vec3 &point) const {
    return glm::distance2(_position, point);
}

bool Object::contains(const glm::vec3 &point) const {
    if (!_model) return false;

    const AABB &aabb = _model->model()->aabb();

    return (aabb * _transform).contains(point);
}

void Object::setPosition(const glm::vec3 &position) {
    _position = position;
    updateTransform();
}

void Object::setSynchronize(bool synchronize) {
    _synchronize = synchronize;
}

void Object::setHeading(float heading) {
    _heading = heading;
    updateTransform();
}

uint32_t Object::id() const {
    return _id;
}

ObjectType Object::type() const {
    return _type;
}

const std::string &Object::tag() const {
    return _tag;
}

const glm::vec3 &Object::position() const {
    return _position;
}

float Object::heading() const {
    return _heading;
}

const glm::mat4 &Object::transform() const {
    return _transform;
}

std::shared_ptr<ModelInstance> Object::model() const {
    return _model;
}

std::shared_ptr<Walkmesh> Object::walkmesh() const {
    return _walkmesh;
}

void Object::updateTransform() {
    _transform = glm::translate(glm::mat4(1.0f), _position);
    _transform *= glm::eulerAngleZ(_heading);
}

} // namespace game

} // namespace reone
