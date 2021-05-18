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
#include "../../graphics/stateutil.h"

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

    _birthrate = modelNode->birthrate().getByFrameOrElse(0, 0.0f);
    _lifeExpectancy = modelNode->lifeExp().getByFrameOrElse(0, 0.0f);
    _size.x = modelNode->xSize().getByFrameOrElse(0, 0.0f);
    _size.y = modelNode->ySize().getByFrameOrElse(0, 0.0f);
    _frameStart = static_cast<int>(modelNode->frameStart().getByFrameOrElse(0, 0.0f));
    _frameEnd = static_cast<int>(modelNode->frameEnd().getByFrameOrElse(0, 0.0f));
    _fps = modelNode->fps().getByFrameOrElse(0, 0.0f);
    _spread = modelNode->spread().getByFrameOrElse(0, 0.0f);
    _velocity = modelNode->velocity().getByFrameOrElse(0, 0.0f);
    _randomVelocity = modelNode->randVel().getByFrameOrElse(0, 0.0f);
    _mass = modelNode->mass().getByFrameOrElse(0, 0.0f);
    _grav = modelNode->grav().getByFrameOrElse(0, 0.0f);
    _lightningDelay = modelNode->lightingDelay().getByFrameOrElse(0, 0.0f);
    _lightningRadius = modelNode->lightingRadius().getByFrameOrElse(0, 0.0f);
    _lightningScale = modelNode->lightingScale().getByFrameOrElse(0, 0.0f);
    _lightningSubDiv = static_cast<int>(modelNode->lightingSubDiv().getByFrameOrElse(0, 0.0f));

    _particleSize.setStart(modelNode->sizeStart().getByFrameOrElse(0, 0.0f));
    _particleSize.setMid(modelNode->sizeMid().getByFrameOrElse(0, 0.0f));
    _particleSize.setEnd(modelNode->sizeEnd().getByFrameOrElse(0, 0.0f));
    _color.setStart(modelNode->colorStart().getByFrameOrElse(0, glm::vec3(0.0f)));
    _color.setMid(modelNode->colorMid().getByFrameOrElse(0, glm::vec3(0.0f)));
    _color.setEnd(modelNode->colorEnd().getByFrameOrElse(0, glm::vec3(0.0f)));
    _alpha.setStart(modelNode->alphaStart().getByFrameOrElse(0, 0.0f));
    _alpha.setMid(modelNode->alphaMid().getByFrameOrElse(0, 0.0f));
    _alpha.setEnd(modelNode->alphaEnd().getByFrameOrElse(0, 0.0f));

    if (_birthrate != 0.0f) {
        _birthInterval = 1.0f / _birthrate;
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
    while (!_particles.empty() && isParticleExpired(*_particles[0])) {
        _particles.pop_front();
    }
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
                    doSpawnParticle();
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
        case ModelNode::Emitter::UpdateMode::Lightning:
            if (_birthTimer.advance(dt)) {
                spawnLightningParticles();
                _birthTimer.reset(_lightningDelay);
            }
            break;
        default:
            break;
    }
}

void EmitterSceneNode::doSpawnParticle() {
    float halfW = 0.005f * _size.x;
    float halfH = 0.005f * _size.y;
    glm::vec3 position(random(-halfW, halfW), random(-halfH, halfH), 0.0f);

    float halfSpread = 0.5f * _spread;
    float angle1 = random(-halfSpread, halfSpread);
    float angle2 = random(-halfSpread, halfSpread);
    glm::vec3 dir(glm::sin(angle1), glm::sin(angle2), glm::cos(angle1) * glm::cos(angle2));

    glm::vec3 velocity((_velocity + random(0.0f, _randomVelocity)) * dir);

    auto particle = make_shared<Particle>();
    particle->emitter = this;
    particle->position = move(position);
    particle->velocity = move(velocity);
    particle->frame = _frameStart;
    if (_fps > 0.0f) {
        particle->animLength = (_frameEnd - _frameStart + 1) / _fps;
    }

    while (_particles.size() >= kMaxParticles) {
        _particles.pop_front();
    }
    _particles.push_back(particle);
}

void EmitterSceneNode::spawnLightningParticles() {
    // Ensure there is a reference node directly under this emitter
    auto ref = find_if(_children.begin(), _children.end(), [](auto &child) { return child->type() == SceneNodeType::Dummy; });
    if (ref == _children.end()) return;

    float halfW = 0.005f * _size.x;
    float halfH = 0.005f * _size.y;
    glm::vec3 origin(random(-halfW, halfW), random(-halfH, halfH), 0.0f);
    glm::vec3 emitterSpaceRefPos(_absTransformInv * (*ref)->absoluteTransform()[3]);
    glm::vec3 refToOrigin(emitterSpaceRefPos - origin);
    float distance = glm::abs(refToOrigin.z);
    float segmentLength = distance / static_cast<float>(_lightningSubDiv + 1);
    float halfRadius = 0.5f * _lightningRadius;

    vector<pair<glm::vec3, glm::vec3>> segments;
    segments.resize(_lightningSubDiv + 1);
    segments[0].first = origin;
    for (int i = 1; i < _lightningSubDiv + 1; ++i) {
        glm::vec3 dir(glm::normalize(emitterSpaceRefPos - segments[i - 1].first));
        glm::vec3 offset(
            random(-halfRadius, halfRadius),
            random(-halfRadius, halfRadius),
            0.0f);
        segments[i - 1].second = segments[i - 1].first + segmentLength * dir + offset;
        segments[i].first = segments[i - 1].second;
    }
    segments[_lightningSubDiv].second = emitterSpaceRefPos;

    _particles.clear();
    for (auto &segment : segments) {
        glm::vec3 endToStart(segment.second - segment.first);
        glm::vec3 center(0.5f * (segment.first + segment.second));
        auto particle = make_shared<Particle>();
        particle->emitter = this;
        particle->position = move(center);
        particle->size = glm::vec2(_lightningScale, glm::abs(endToStart.z));
        _particles.push_back(move(particle));
    }
}

void EmitterSceneNode::updateParticle(Particle &particle, float dt) {
    // Lightning emitters are updated elsewhere
    if (_modelNode->emitter()->updateMode == ModelNode::Emitter::UpdateMode::Lightning) return;

    if (_lifeExpectancy != -1.0f) {
        particle.lifetime = glm::min(particle.lifetime + dt, _lifeExpectancy);
    } else if (particle.lifetime == particle.animLength) {
        particle.lifetime = 0.0f;
    } else {
        particle.lifetime = glm::min(particle.lifetime + dt, particle.animLength);
    }

    if (!isParticleExpired(particle)) {
        particle.position += particle.velocity * dt;

        // Gravity-type P2P emitter
        if (_modelNode->emitter()->p2p && !_modelNode->emitter()->p2pBezier) {
            auto ref = find_if(_children.begin(), _children.end(), [](auto &child) { return child->type() == SceneNodeType::Dummy; });
            if (ref != _children.end()) {
                glm::vec3 emitterSpaceRefPos(_absTransformInv * (*ref)->absoluteTransform()[3]);
                glm::vec3 pullDir(glm::normalize(emitterSpaceRefPos - particle.position));
                particle.velocity += _grav * pullDir * dt;
            }
        }

        updateParticleAnimation(particle, dt);
    }
}

void EmitterSceneNode::updateParticleAnimation(Particle &particle, float dt) {
    float factor;
    if (_lifeExpectancy != -1.0f) {
        factor = particle.lifetime / static_cast<float>(_lifeExpectancy);
    } else if (particle.animLength > 0.0f) {
        factor = particle.lifetime / particle.animLength;
    } else {
        factor = 0.0f;
    }

    particle.frame = static_cast<int>(glm::ceil(_frameStart + factor * (_frameEnd - _frameStart)));
    particle.size = glm::vec2(_particleSize.get(factor));
    particle.color = _color.get(factor);
    particle.alpha = _alpha.get(factor);
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
    uniforms.particles->gridSize = emitter->gridSize;
    uniforms.particles->render = static_cast<int>(emitter->renderMode);

    for (size_t i = 0; i < particles.size(); ++i) {
        const Particle &particle = *particles[i];

        glm::mat4 transform(_absTransform);
        transform = glm::translate(transform, particles[i]->position);
        if (emitter->renderMode == ModelNode::Emitter::RenderMode::MotionBlur) {
            transform = glm::scale(transform, glm::vec3((1.0f + kMotionBlurStrength * kProjectileSpeed) * particle.size.x, particle.size.y, 1.0f));
        } else {
            transform = glm::scale(transform, glm::vec3(particle.size, 1.0f));
        }

        uniforms.particles->particles[i].transform = move(transform);
        uniforms.particles->particles[i].color = glm::vec4(particle.color, 1.0f);
        uniforms.particles->particles[i].size = glm::vec2(particle.size);
        uniforms.particles->particles[i].alpha = particle.alpha;
        uniforms.particles->particles[i].frame = particle.frame;
    }

    Shaders::instance().activate(ShaderProgram::ParticleParticle, uniforms);

    setActiveTextureUnit(TextureUnits::diffuseMap);
    texture->bind();

    bool lighten = emitter->blendMode == ModelNode::Emitter::BlendMode::Lighten;
    if (lighten) {
        withLightenBlending([&particles]() {
            Meshes::instance().getBillboard()->drawInstanced(static_cast<int>(particles.size()));
        });
    } else {
        Meshes::instance().getBillboard()->drawInstanced(static_cast<int>(particles.size()));
    }
}

} // namespace scene

} // namespace reone
