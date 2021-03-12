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

#include <algorithm>
#include <stdexcept>
#include <unordered_map>

#include "glm/gtc/constants.hpp"

#include "../../common/random.h"
#include "../../render/meshes.h"
#include "../../render/shaders.h"
#include "../../render/stateutil.h"

#include "../scenegraph.h"

#include "cameranode.h"
#include "modelscenenode.h"

using namespace std;

using namespace reone::render;

namespace reone {

namespace scene {

static constexpr float kMotionBlurStrength = 0.25f;

EmitterSceneNode::EmitterSceneNode(const ModelSceneNode *modelSceneNode, const shared_ptr<Emitter> &emitter, SceneGraph *sceneGraph) :
    SceneNode(sceneGraph),
    _modelSceneNode(modelSceneNode),
    _emitter(emitter) {

    if (!modelSceneNode) {
        throw invalid_argument("modelSceneNode must not be null");
    }
    if (!emitter) {
        throw invalid_argument("emitter must not be null");
    }

    init();
}

void EmitterSceneNode::init() {
    if (_emitter->birthrate() != 0) {
        _birthInterval = 1.0f / static_cast<float>(_emitter->birthrate());
    }
}

void EmitterSceneNode::update(float dt) {
    shared_ptr<CameraSceneNode> camera(_sceneGraph->activeCamera());
    if (!camera) return;

    removeExpiredParticles(dt);
    spawnParticles(dt);

    for (auto &particle : _particles) {
        particle->update(dt);
    }
}

void EmitterSceneNode::removeExpiredParticles(float dt) {
    for (auto it = _particles.begin(); it != _particles.end(); ) {
        auto &particle = (*it);
        if (particle->isExpired()) {
            it = _particles.erase(it);
        } else {
            ++it;
        }
    }
}

void EmitterSceneNode::spawnParticles(float dt) {
    switch (_emitter->updateMode()) {
        case Emitter::UpdateMode::Fountain:
            if (_emitter->birthrate() != 0.0f) {
                if (_birthTimer.advance(dt)) {
                    if (_particles.size() < kMaxParticleCount) {
                        doSpawnParticle();
                    }
                    _birthTimer.reset(_birthInterval);
                }
            }
            break;
        case Emitter::UpdateMode::Single:
            if (!_spawned || (_particles.empty() && _emitter->loop())) {
                doSpawnParticle();
                _spawned = true;
            }
            break;
        default:
            break;
    }
}

void EmitterSceneNode::doSpawnParticle() {
    float halfW = 0.005f * _emitter->size().x;
    float halfH = 0.005f * _emitter->size().y;
    glm::vec3 position(random(-halfW, halfW), random(-halfH, halfH), 0.0f);

    float sign;
    if (_emitter->spread() > glm::pi<float>() && random(0, 1) != 0) {
        sign = -1.0f;
    } else {
        sign = 1.0f;
    }
    float velocity = sign * (_emitter->velocity() + random(0.0f, _emitter->randomVelocity()));

    auto particle = make_shared<Particle>(position, velocity, this);
    _particles.push_back(particle);
}

void EmitterSceneNode::renderParticles(const vector<Particle *> &particles) {
    if (particles.empty()) return;

    shared_ptr<Texture> texture(_emitter->texture());
    if (!texture) return;

    ShaderUniforms uniforms(_sceneGraph->uniformsPrototype());
    uniforms.general.featureMask |= UniformFeatureFlags::billboard;
    uniforms.billboard.gridSize = glm::vec2(_emitter->gridWidth(), _emitter->gridHeight());
    uniforms.billboard.render = static_cast<int>(_emitter->renderMode());

    for (size_t i = 0; i < particles.size(); ++i) {
        const Particle &particle = *particles[i];

        glm::mat4 transform(_absoluteTransform);
        transform = glm::translate(transform, particles[i]->position());
        if (_emitter->renderMode() == Emitter::RenderMode::MotionBlur) {
            transform = glm::scale(transform, glm::vec3((1.0f + kMotionBlurStrength * _modelSceneNode->projectileSpeed()) * particle.size(), particle.size(), particle.size()));
        } else {
            transform = glm::scale(transform, glm::vec3(particle.size()));
        }

        uniforms.billboard.particles[i].transform = move(transform);
        uniforms.billboard.particles[i].position = _absoluteTransform * glm::vec4(particle.position(), 1.0f);
        uniforms.billboard.particles[i].color = glm::vec4(particle.color(), 1.0f);
        uniforms.billboard.particles[i].size = glm::vec2(particle.size());
        uniforms.billboard.particles[i].alpha = particle.alpha();
        uniforms.billboard.particles[i].frame = particle.frame();
    }

    Shaders::instance().activate(ShaderProgram::BillboardBillboard, uniforms);

    setActiveTextureUnit(TextureUnits::diffuse);
    texture->bind();

    bool lighten = _emitter->blendMode() == Emitter::BlendMode::Lighten;
    if (lighten) {
        withAdditiveBlending([&particles]() { Meshes::instance().getBillboard()->renderInstanced(static_cast<int>(particles.size())); });
    } else {
        Meshes::instance().getBillboard()->renderInstanced(static_cast<int>(particles.size()));
    }
}

void EmitterSceneNode::detonate() {
    doSpawnParticle();
}

} // namespace scene

} // namespace reone
