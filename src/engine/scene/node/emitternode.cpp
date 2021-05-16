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
#include "../../graphics/mesh/meshes.h"
#include "../../graphics/shader/shaders.h"
#include "../../graphics/statemanager.h"

#include "../scenegraph.h"

#include "cameranode.h"
#include "modelnode.h"

using namespace std;

using namespace reone::graphics;

namespace reone {

namespace scene {

static constexpr float kMotionBlurStrength = 0.25f;
static constexpr float kProjectileSpeed = 16.0f;

EmitterSceneNode::EmitterSceneNode(const ModelSceneNode *model, shared_ptr<ModelNode> modelNode, SceneGraph *sceneGraph) :
    ModelNodeSceneNode(modelNode, SceneNodeType::Emitter, sceneGraph),
    _model(model) {

    if (!model) {
        throw invalid_argument("model must not be null");
    }
    _particleSize.start = modelNode->sizeStart().getByFrameOrElse(0, 0.0f);
    _particleSize.mid = modelNode->sizeMid().getByFrameOrElse(0, 0.0f);
    _particleSize.end = modelNode->sizeEnd().getByFrameOrElse(0, 0.0f);
    _color.start = modelNode->colorStart().getByFrameOrElse(0, glm::vec3(0.0f));
    _color.mid = modelNode->colorMid().getByFrameOrElse(0, glm::vec3(0.0f));
    _color.end = modelNode->colorEnd().getByFrameOrElse(0, glm::vec3(0.0f));
    _alpha.start = modelNode->alphaStart().getByFrameOrElse(0, 0.0f);
    _alpha.mid = modelNode->alphaMid().getByFrameOrElse(0, 0.0f);
    _alpha.end = modelNode->alphaEnd().getByFrameOrElse(0, 0.0f);
    _frameStart = static_cast<int>(modelNode->frameStart().getByFrameOrElse(0, 0.0f));
    _frameEnd = static_cast<int>(modelNode->frameEnd().getByFrameOrElse(0, 0.0f));
    _size.x = modelNode->xSize().getByFrameOrElse(0, 0.0f);
    _size.y = modelNode->ySize().getByFrameOrElse(0, 0.0f);
    _birthrate = modelNode->birthrate().getByFrameOrElse(0, 0.0f);
    _lifeExpectancy = modelNode->lifeExp().getByFrameOrElse(0, 0.0f);
    _velocity = modelNode->velocity().getByFrameOrElse(0, 0.0f);
    _randomVelocity = modelNode->randVel().getByFrameOrElse(0, 0.0f);
    _spread = modelNode->spread().getByFrameOrElse(0, 0.0f);
    _fps = modelNode->fps().getByFrameOrElse(0, 0.0f);

    if (_birthrate != 0.0f) {
        _birthInterval = 1.0f / static_cast<float>(_birthrate);
    }
}

void EmitterSceneNode::update(float dt) {
    removeExpiredParticles(dt);
    spawnParticles(dt);

    for (auto &particle : _particles) {
        updateParticle(*particle, dt);
    }
}

void EmitterSceneNode::removeExpiredParticles(float dt) {
    auto expiredParticles = find_if(_particles.begin(), _particles.end(), [this](auto &particle) { return isParticleExpired(*particle); });
    _particles.erase(expiredParticles, _particles.end());
}

bool EmitterSceneNode::isParticleExpired(Particle &particle) const {
    return _lifeExpectancy != -1.0f && particle.lifetime >= _lifeExpectancy;
}

void EmitterSceneNode::spawnParticles(float dt) {
    shared_ptr<ModelNode::Emitter> emitter(_modelNode->emitter());
    switch (emitter->updateMode) {
        case ModelNode::Emitter::UpdateMode::Fountain:
            if (_birthrate != 0.0f) {
                if (_birthTimer.advance(dt)) {
                    if (_particles.size() < kMaxParticles) {
                        doSpawnParticle();
                    }
                    _birthTimer.reset(_birthInterval);
                }
            }
            break;
        case ModelNode::Emitter::UpdateMode::Single:
            if (!_spawned || (_particles.empty() && emitter->loop)) {
                doSpawnParticle();
                _spawned = true;
            }
            break;
        default:
            break;
    }
}

void EmitterSceneNode::doSpawnParticle() {
    shared_ptr<ModelNode::Emitter> emitter(_modelNode->emitter());
    float halfW = 0.005f * _size.x;
    float halfH = 0.005f * _size.y;
    glm::vec3 position(random(-halfW, halfW), random(-halfH, halfH), 0.0f);

    float sign;
    if (_spread > glm::pi<float>() && random(0, 1) != 0) {
        sign = -1.0f;
    } else {
        sign = 1.0f;
    }
    float velocity = sign * (_velocity + random(0.0f, _randomVelocity));

    auto particle = make_shared<Particle>();
    particle->emitter = this;
    particle->position = move(position);
    particle->velocity = velocity;
    particle->frame = _frameStart;
    if (_fps > 0.0f) {
        particle->animLength = (_frameEnd - _frameStart + 1) / _fps;
    }
    _particles.push_back(particle);
}

void EmitterSceneNode::updateParticle(Particle &particle, float dt) {
    shared_ptr<ModelNode::Emitter> emitter(_modelNode->emitter());

    if (_lifeExpectancy != -1.0f) {
        particle.lifetime = glm::min(particle.lifetime + dt, _lifeExpectancy);
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
    if (_lifeExpectancy != -1.0f) {
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

void EmitterSceneNode::detonate() {
    doSpawnParticle();
}

void EmitterSceneNode::drawParticles(const vector<Particle *> &particles) {
    if (particles.empty()) return;

    shared_ptr<ModelNode::Emitter> emitter(_modelNode->emitter());
    shared_ptr<Texture> texture(emitter->texture);
    if (!texture) return;

    ShaderUniforms uniforms(_sceneGraph->uniformsPrototype());
    uniforms.combined.featureMask |= UniformFeatureFlags::particles;
    uniforms.particles->gridSize = glm::vec2(emitter->gridWidth, emitter->gridHeight);
    uniforms.particles->render = static_cast<int>(emitter->renderMode);

    for (size_t i = 0; i < particles.size(); ++i) {
        const Particle &particle = *particles[i];

        glm::mat4 transform(_absTransform);
        transform = glm::translate(transform, particles[i]->position);
        if (emitter->renderMode == ModelNode::Emitter::RenderMode::MotionBlur) {
            transform = glm::scale(transform, glm::vec3((1.0f + kMotionBlurStrength * kProjectileSpeed) * particle.size, particle.size, particle.size));
        } else {
            transform = glm::scale(transform, glm::vec3(particle.size));
        }

        uniforms.particles->particles[i].transform = move(transform);
        uniforms.particles->particles[i].color = glm::vec4(particle.color, 1.0f);
        uniforms.particles->particles[i].size = glm::vec2(particle.size);
        uniforms.particles->particles[i].alpha = particle.alpha;
        uniforms.particles->particles[i].frame = particle.frame;
    }

    Shaders::instance().activate(ShaderProgram::ParticleParticle, uniforms);

    StateManager::instance().setActiveTextureUnit(TextureUnits::diffuseMap);
    texture->bind();

    bool lighten = emitter->blendMode == ModelNode::Emitter::BlendMode::Lighten;
    if (lighten) {
        StateManager::instance().withLightenBlending([&particles]() {
            Meshes::instance().getBillboard()->drawInstanced(static_cast<int>(particles.size()));
        });
    } else {
        Meshes::instance().getBillboard()->drawInstanced(static_cast<int>(particles.size()));
    }
}

} // namespace scene

} // namespace reone
