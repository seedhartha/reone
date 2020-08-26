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

#include "spatial.h"

#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/norm.hpp"

using namespace std;

using namespace reone::render;

namespace reone {

namespace game {

SpatialObject::SpatialObject(uint32_t id) : Object(id) {
}

float SpatialObject::distanceTo(const glm::vec3 &point) const {
    return glm::distance2(_position, point);
}

bool SpatialObject::contains(const glm::vec3 &point) const {
    if (!_model) return false;

    const AABB &aabb = _model->model()->aabb();

    return (aabb * _transform).contains(point);
}

void SpatialObject::face(const SpatialObject &other) {
    glm::vec2 dir(glm::normalize(other._position - _position));
    _heading = -glm::atan(dir.x, dir.y);
    updateTransform();
}

void SpatialObject::update(const UpdateContext &ctx) {
    if (!_model) return;

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

void SpatialObject::initGL() {
    if (!_model) return;

    _model->initGL();
}

void SpatialObject::animate(const string &anim, int flags, float speed) {
    if (!_model) return;

    _model->animate(anim, flags, speed);
}

void SpatialObject::animate(const string &parent, const string &anim, int flags, float speed) {
    if (!_model) return;

    _model->animate(parent, anim, flags, speed);
}

const glm::vec3 &SpatialObject::position() const {
    return _position;
}

float SpatialObject::heading() const {
    return _heading;
}

const glm::mat4 &SpatialObject::transform() const {
    return _transform;
}
void SpatialObject::setPosition(const glm::vec3 &position) {
    _position = position;
    updateTransform();
}

void SpatialObject::updateTransform() {
    _transform = glm::translate(glm::mat4(1.0f), _position);
    _transform *= glm::eulerAngleZ(_heading);
}

void SpatialObject::setHeading(float heading) {
    _heading = heading;
    updateTransform();
}

shared_ptr<ModelInstance> SpatialObject::model() const {
    return _model;
}

shared_ptr<Walkmesh> SpatialObject::walkmesh() const {
    return _walkmesh;
}

} // namespace game

} // namespace reone
