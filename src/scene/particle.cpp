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

#include "particle.h"

#include <stdexcept>

#include "glm/common.hpp"

using namespace std;

using namespace reone::render;

namespace reone {

namespace scene {

Particle::Particle(glm::vec3 position, float velocity, const Emitter *emitter) :
    _position(position),
    _velocity(velocity),
    _emitter(emitter) {

    if (!emitter) {
        throw invalid_argument("emitter must not be null");
    }

    init();
}

void Particle::init() {
    if (_emitter->fps() > 0) {
        _animLength = (_emitter->frameEnd() - _emitter->frameStart() + 1) / static_cast<float>(_emitter->fps());
    }
    _renderOrder = _emitter->renderOrder();
    _frame = _emitter->frameStart();
}

void Particle::update(float dt) {
    if (_emitter->lifeExpectancy() != -1) {
        _lifetime = glm::min(_lifetime + dt, static_cast<float>(_emitter->lifeExpectancy()));
    } else if (_lifetime == _animLength) {
        _lifetime = 0.0f;
    } else {
        _lifetime = glm::min(_lifetime + dt, _animLength);
    }
    if (!isExpired()) {
        _position.z += _velocity * dt;
        updateAnimation(dt);
    }
}

template <class T>
static T interpolateConstraints(const Emitter::Constraints<T> &constraints, float t) {
    T result;
    if (t < 0.5f) {
        float tt = 2.0f * t;
        result = (1.0f - tt) * constraints.start + tt * constraints.mid;
    } else {
        float tt = 2.0f * (t - 0.5f);
        result = (1.0f - tt) * constraints.mid + tt * constraints.end;
    }
    return result;
}

void Particle::updateAnimation(float dt) {
    float maturity;
    if (_emitter->lifeExpectancy() != -1) {
        maturity = _lifetime / static_cast<float>(_emitter->lifeExpectancy());
    } else if (_animLength > 0.0f) {
        maturity = _lifetime / _animLength;
    } else {
        maturity = 0.0f;
    }
    _frame = static_cast<int>(glm::ceil(_emitter->frameStart() + maturity * (_emitter->frameEnd() - _emitter->frameStart())));
    _size = interpolateConstraints(_emitter->particleSize(), maturity);
    _color = interpolateConstraints(_emitter->color(), maturity);
    _alpha = interpolateConstraints(_emitter->alpha(), maturity);
}

bool Particle::isExpired() const {
    return _emitter->lifeExpectancy() != -1 && _lifetime >= _emitter->lifeExpectancy();
}

} // namespace scene

} // namespace reone
