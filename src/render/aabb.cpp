#include "aabb.h"

#include <utility>

#include "glm/ext.hpp"
#include "glm/gtx/intersect.hpp"
#include "glm/gtx/norm.hpp"

namespace reone {

namespace render {

AABB::AABB(const glm::vec3 &min, const glm::vec3 &max) : _empty(false), _min(min), _max(max) {
    updateTransform();
}

AABB AABB::operator*(const glm::mat4 &m) const {
    AABB aabb;
    if (!_empty) {
        glm::vec3 a(m * glm::vec4(_min, 1.0f));
        glm::vec3 b(m * glm::vec4(_max, 1.0f));

        glm::vec3 min(
            glm::min(a.x, b.x),
            glm::min(a.y, b.y),
            glm::min(a.z, b.z));

        glm::vec3 max(
            glm::max(a.x, b.x),
            glm::max(a.y, b.y),
            glm::max(a.z, b.z));

        aabb = AABB(min, max);
    }

    return std::move(aabb);
}

void AABB::reset() {
    _min = glm::vec3(0.0f, 0.0f, 0.0f);
    _max = glm::vec3(0.0f, 0.0f, 0.0f);
    _empty = true;

    updateTransform();
}

void AABB::expand(const glm::vec3 &p) {
    if (_empty) {
        _min.x = p.x;
        _min.y = p.y;
        _min.z = p.z;
        _max = _min;
        _empty = false;
    } else {
        if (p.x < _min.x) _min.x = p.x;
        if (p.y < _min.y) _min.y = p.y;
        if (p.z < _min.z) _min.z = p.z;
        if (p.x > _max.x) _max.x = p.x;
        if (p.y > _max.y) _max.y = p.y;
        if (p.z > _max.z) _max.z = p.z;
    }

    updateTransform();
}

void AABB::expand(const AABB &aabb) {
    if (_empty) {
        _min = aabb._min;
        _max = aabb._max;
        _empty = false;
    } else {
        if (aabb._min.x < _min.x) _min.x = aabb._min.x;
        if (aabb._min.y < _min.y) _min.y = aabb._min.y;
        if (aabb._min.z < _min.z) _min.z = aabb._min.z;
        if (aabb._max.x > _max.x) _max.x = aabb._max.x;
        if (aabb._max.y > _max.y) _max.y = aabb._max.y;
        if (aabb._max.z > _max.z) _max.z = aabb._max.z;
    }

    updateTransform();
}

void AABB::updateTransform() {
    glm::vec3 size(glm::abs(_max - _min));
    _center = 0.5f * (_min + _max);

    _transform = glm::translate(glm::mat4(1.0f), _center);
    _transform = glm::scale(_transform, size);
}

bool AABB::contains(const glm::vec2 &p) const {
    return
        p.x >= _min.x && p.x <= _max.x &&
        p.y >= _min.y && p.y <= _max.y;
}

bool AABB::contains(const glm::vec3 &p) const {
    return
        p.x >= _min.x && p.x <= _max.x &&
        p.y >= _min.y && p.y <= _max.y &&
        p.z >= _min.z && p.z <= _max.z;
}

bool AABB::intersectLine(const glm::vec3 &from, const glm::vec3 &to, float &distance) const {
    if (contains(from) || contains(to)) return true;

    glm::vec3 delta(to - from);
    glm::vec3 dir(glm::normalize(delta));
    glm::vec3 dirfrac(1.0f / dir);

    float t1 = (_min.x - from.x) * dirfrac.x;
    float t2 = (_max.x - from.x) * dirfrac.x;
    float t3 = (_min.y - from.y) * dirfrac.y;
    float t4 = (_max.y - from.y) * dirfrac.y;
    float t5 = (_min.z - from.z) * dirfrac.z;
    float t6 = (_max.z - from.z) * dirfrac.z;

    float tmin = glm::max(glm::max(glm::min(t1, t2), glm::min(t3, t4)), glm::min(t5, t6));
    float tmax = glm::min(glm::min(glm::max(t1, t2), glm::max(t3, t4)), glm::max(t5, t6));

    if (tmax < 0.0f) return false;
    if (tmin > tmax) return false;

    float tmin2 = tmin * tmin;
    if (tmin2 > glm::length2(delta)) return false;

    distance = tmin2;

    return true;
}

glm::vec3 AABB::size() const {
    return _max - _min;
}

const glm::vec3 &AABB::min() const {
    return _min;
}

const glm::vec3 &AABB::max() const {
    return _max;
}

const glm::vec3 &AABB::center() const {
    return _center;
}

const glm::mat4 &AABB::transform() const {
    return _transform;
}

} // namespace render

} // namespace reone
