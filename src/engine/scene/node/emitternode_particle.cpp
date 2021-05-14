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

#include "emitternode.h"

using namespace std;

using namespace reone::graphics;

namespace reone {

namespace scene {

void EmitterSceneNode::initParticle(Particle &particle) {
    shared_ptr<ModelNode::Emitter> emitter(_modelNode->emitter());

    if (_fps > 0) {
        particle.animLength = (_frameEnd - _frameStart + 1) / static_cast<float>(_fps);
    }

    particle.frame = _frameStart;
}

void EmitterSceneNode::updateParticle(Particle &particle, float dt) {
    shared_ptr<ModelNode::Emitter> emitter(_modelNode->emitter());

    if (_lifeExpectancy != -1) {
        particle.lifetime = glm::min(particle.lifetime + dt, static_cast<float>(_lifeExpectancy));
    } else if (particle.lifetime == particle.animLength) {
        particle.lifetime = 0.0f;
    } else {
        particle.lifetime = glm::min(particle.lifetime + dt, particle.animLength);
    }

    if (!isParticleExpired(particle)) {
        particle.position.z += particle.velocity * dt;
        updateParticleAnimation(particle, dt);
    }
}

template <class T>
static T interpolateConstraints(const EmitterSceneNode::Constraints<T> &constraints, float t) {
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

void EmitterSceneNode::updateParticleAnimation(Particle &particle, float dt) {
    shared_ptr<ModelNode::Emitter> emitter(_modelNode->emitter());

    float maturity;
    if (_lifeExpectancy != -1) {
        maturity = particle.lifetime / static_cast<float>(_lifeExpectancy);
    } else if (particle.animLength > 0.0f) {
        maturity = particle.lifetime / particle.animLength;
    } else {
        maturity = 0.0f;
    }

    particle.frame = static_cast<int>(glm::ceil(_frameStart + maturity * (_frameEnd - _frameStart)));
    particle.size = interpolateConstraints(_particleSize, maturity);
    particle.color = interpolateConstraints(_color, maturity);
    particle.alpha = interpolateConstraints(_alpha, maturity);
}

bool EmitterSceneNode::isParticleExpired(Particle &particle) const {
    shared_ptr<ModelNode::Emitter> emitter(_modelNode->emitter());
    return _lifeExpectancy != -1 && particle.lifetime >= _lifeExpectancy;
}

} // namespace scene

} // namespace reone
