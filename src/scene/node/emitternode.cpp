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

#include <stdexcept>

#include "../../common/random.h"

#include "particlenode.h"

using namespace std;

using namespace reone::render;

namespace reone {

namespace scene {

constexpr int kMaxParticleCount = 16;

EmitterSceneNode::EmitterSceneNode(const shared_ptr<Emitter> &emitter, SceneGraph *sceneGraph) :
    SceneNode(sceneGraph),
    _emitter(emitter) {

    if (!emitter) {
        throw invalid_argument("emitter must not be null");
    }

    init();
}

void EmitterSceneNode::init() {
    _birthInterval = 1.0f / _emitter->birthrate();
}

void EmitterSceneNode::update(float dt) {
    spawnParticles(dt);
    updateParticles(dt);
}

void EmitterSceneNode::spawnParticles(float dt) {
    _birthTimer.update(dt);

    if (_birthTimer.hasTimedOut()) {
        if (_particles.size() < kMaxParticleCount) {
            float halfW = 0.005f * _emitter->size().x;
            float halfH = 0.005f * _emitter->size().y;
            glm::vec3 position(random(-halfW, halfW), random(-halfH, halfH), 0.0f);

            float velocity = (_emitter->velocity() + random(0.0f, _emitter->randomVelocity()));

            auto particle = make_shared<ParticleSceneNode>(position, velocity, _emitter, _sceneGraph);
            _particles.push_back(particle);
            addChild(particle);
        }
        _birthTimer.reset(_birthInterval);
    }
}

void EmitterSceneNode::updateParticles(float dt) {
    for (auto it = _particles.begin(); it != _particles.end(); ) {
        auto &particle = (*it);
        particle->update(dt);

        if (particle->isExpired()) {
            removeChild(*particle);
            it = _particles.erase(it);
        } else {
            ++it;
        }
    }
}

} // namespace scene

} // namespace reone
