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

#include "node/emitternode.h"

using namespace std;

using namespace reone::graphics;

namespace reone {

namespace scene {

Particle::Particle(glm::vec3 position, float velocity, EmitterSceneNode *emitter) :
    _position(position),
    _velocity(velocity),
    _emitter(emitter) {

    if (!emitter) {
        throw invalid_argument("emitter must not be null");
    }

    init();
}

void Particle::init() {
    shared_ptr<ModelNode::Emitter> emitter(_emitter->emitter());

    if (emitter->fps > 0) {
        _animLength = (emitter->frameEnd - emitter->frameStart + 1) / static_cast<float>(emitter->fps);
    }

    _renderOrder = emitter->renderOrder;
    _frame = emitter->frameStart;
}

void Particle::update(float dt) {
    shared_ptr<ModelNode::Emitter> emitter(_emitter->emitter());

    if (emitter->lifeExpectancy != -1) {
        _lifetime = glm::min(_lifetime + dt, static_cast<float>(emitter->lifeExpectancy));
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
static T interpolateConstraints(const ModelNode::Emitter::Constraints<T> &constraints, float t) {
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
    shared_ptr<ModelNode::Emitter> emitter(_emitter->emitter());

    float maturity;
    if (emitter->lifeExpectancy != -1) {
        maturity = _lifetime / static_cast<float>(emitter->lifeExpectancy);
    } else if (_animLength > 0.0f) {
        maturity = _lifetime / _animLength;
    } else {
        maturity = 0.0f;
    }

    _frame = static_cast<int>(glm::ceil(emitter->frameStart + maturity * (emitter->frameEnd - emitter->frameStart)));
    _size = interpolateConstraints(emitter->particleSize, maturity);
    _color = interpolateConstraints(emitter->color, maturity);
    _alpha = interpolateConstraints(emitter->alpha, maturity);
}

bool Particle::isExpired() const {
    shared_ptr<ModelNode::Emitter> emitter(_emitter->emitter());
    return emitter->lifeExpectancy != -1 && _lifetime >= emitter->lifeExpectancy;
}

} // namespace scene

} // namespace reone
