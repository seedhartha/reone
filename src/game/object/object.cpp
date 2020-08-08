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

#include "object.h"

#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/norm.hpp"

using namespace std;

using namespace reone::render;
using namespace reone::resources;

namespace reone {

namespace game {

Object::Object(uint32_t id) : _id(id) {
}

void Object::animate(const string &anim, int flags) {
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

void Object::face(const Object &other) {
    glm::vec2 dir(glm::normalize(other._position - _position));
    _heading = -glm::atan(dir.x, dir.y);
    updateTransform();
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

const string &Object::tag() const {
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

shared_ptr<ModelInstance> Object::model() const {
    return _model;
}

shared_ptr<Walkmesh> Object::walkmesh() const {
    return _walkmesh;
}

void Object::updateTransform() {
    _transform = glm::translate(glm::mat4(1.0f), _position);
    _transform *= glm::eulerAngleZ(_heading);
}

} // namespace game

} // namespace reone
