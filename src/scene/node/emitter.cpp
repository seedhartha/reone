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

#include "emitter.h"

#include "../../common/randomutil.h"
#include "../../graphics/context.h"
#include "../../graphics/mesh.h"
#include "../../graphics/meshes.h"
#include "../../graphics/shaders.h"
#include "../../graphics/texture.h"

#include "../graph.h"

#include "camera.h"
#include "particle.h"

using namespace std;

using namespace reone::graphics;

namespace reone {

namespace scene {

static constexpr float kMotionBlurStrength = 0.25f;
static constexpr float kProjectileSpeed = 16.0f;

EmitterSceneNode::EmitterSceneNode(
    shared_ptr<ModelNode> modelNode,
    SceneGraph &sceneGraph,
    Context &context,
    Meshes &meshes,
    Shaders &shaders) :
    ModelNodeSceneNode(
        modelNode,
        SceneNodeType::Emitter,
        sceneGraph,
        context,
        meshes,
        shaders) {

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

    // Pre-allocate particles
    int numParticles;
    if (_modelNode->emitter()->updateMode == ModelNode::Emitter::UpdateMode::Single) {
        numParticles = 1;
    } else {
        numParticles = kMaxParticles;
    }
    for (int i = 0; i < numParticles; ++i) {
        _particlePool.push_back(newParticle());
    }
}

void EmitterSceneNode::update(float dt) {
    removeExpiredParticles(dt);
    spawnParticles(dt);

    for (auto &child : _children) {
        auto particle = static_pointer_cast<ParticleSceneNode>(child);
        particle->update(dt);
    }
}

void EmitterSceneNode::removeExpiredParticles(float dt) {
    if (_lifeExpectancy == -1.0f) {
        return;
    }
    vector<shared_ptr<SceneNode>> expiredParticles;
    for (auto &child : _children) {
        if (child->type() != SceneNodeType::Particle) {
            continue;
        }
        auto particle = static_pointer_cast<ParticleSceneNode>(child);
        if (particle->isExpired()) {
            expiredParticles.push_back(child);
        }
    }
    for (auto &particle : expiredParticles) {
        removeChild(particle);
        _particlePool.push_back(particle);
    }
}

void EmitterSceneNode::spawnParticles(float dt) {
    shared_ptr<ModelNode::Emitter> emitter(_modelNode->emitter());
    switch (emitter->updateMode) {
    case ModelNode::Emitter::UpdateMode::Fountain:
        if (_birthrate != 0.0f) {
            if (_birthTimer.advance(dt)) {
                doSpawnParticle();
                _birthTimer.setTimeout(_birthInterval);
            }
        }
        break;
    case ModelNode::Emitter::UpdateMode::Single:
        if (!_spawned || (_children.empty() && emitter->loop)) {
            doSpawnParticle();
            _spawned = true;
        }
        break;
    case ModelNode::Emitter::UpdateMode::Lightning:
        if (_birthTimer.advance(dt)) {
            spawnLightningParticles();
            _birthTimer.setTimeout(_lightningDelay);
        }
        break;
    default:
        break;
    }
}

void EmitterSceneNode::doSpawnParticle() {
    // Take particle from the pool, if available
    if (_particlePool.empty()) {
        return;
    }
    auto particle = static_pointer_cast<ParticleSceneNode>(_particlePool.front());
    particle->setLifetime(0.0f);

    float halfW = 0.005f * _size.x;
    float halfH = 0.005f * _size.y;
    glm::vec3 position(random(-halfW, halfW), random(-halfH, halfH), 0.0f);
    particle->setPosition(move(position));

    float halfSpread = 0.5f * _spread;
    float angle1 = random(-halfSpread, halfSpread);
    float angle2 = random(-halfSpread, halfSpread);
    glm::vec3 dir(glm::sin(angle1), glm::sin(angle2), glm::cos(angle1) * glm::cos(angle2));
    glm::vec3 velocity((_velocity + random(0.0f, _randomVelocity)) * dir);
    particle->setVelocity(move(velocity));

    particle->setFrame(_frameStart);
    if (_fps > 0.0f) {
        particle->setAnimLength((_frameEnd - _frameStart + 1) / _fps);
    }

    // Remove particle from pool and append it to emitter
    _particlePool.pop_front();
    addChild(move(particle));
}

void EmitterSceneNode::spawnLightningParticles() {
    // Ensure there is a reference node directly under this emitter
    auto ref = find_if(_children.begin(), _children.end(), [](auto &child) { return child->type() == SceneNodeType::Dummy; });
    if (ref == _children.end()) {
        return;
    }

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

    // Return all particles to pool
    for (auto &child : _children) {
        _particlePool.push_back(child);
    }
    _children.clear();

    for (auto &segment : segments) {
        // Take particle from the pool, if available
        if (_particlePool.empty()) {
            return;
        }
        auto particle = static_pointer_cast<ParticleSceneNode>(_particlePool.front());
        _particlePool.pop_front();
        particle->setLifetime(0.0f);

        glm::vec3 endToStart(segment.second - segment.first);
        glm::vec3 center(0.5f * (segment.first + segment.second));
        particle->setPosition(move(center));
        particle->setDir(_absTransform * glm::vec4(glm::normalize(endToStart), 0.0f));
        particle->setSize(glm::vec2(_lightningScale, glm::length(endToStart)));

        addChild(move(particle));
    }
}

void EmitterSceneNode::detonate() {
    doSpawnParticle();
}

void EmitterSceneNode::drawElements(const vector<SceneNode *> &elements, int count) {
    if (elements.empty()) {
        return;
    }
    if (count == -1) {
        count = static_cast<int>(elements.size());
    }

    shared_ptr<ModelNode::Emitter> emitter(_modelNode->emitter());
    shared_ptr<Texture> texture(emitter->texture);
    if (!texture) {
        return;
    }

    ShaderUniforms uniforms(_sceneGraph.uniformsPrototype());
    uniforms.combined.featureMask |= UniformFeatureFlags::particles;
    uniforms.particles->gridSize = emitter->gridSize;
    uniforms.particles->render = static_cast<int>(emitter->renderMode);

    for (int i = 0; i < count; ++i) {
        auto particle = static_cast<ParticleSceneNode *>(elements[i]);

        glm::mat4 transform(_absTransform);
        transform = glm::translate(transform, particle->position());
        if (emitter->renderMode == ModelNode::Emitter::RenderMode::MotionBlur) {
            transform = glm::scale(transform, glm::vec3((1.0f + kMotionBlurStrength * kProjectileSpeed) * particle->size().x, particle->size().y, 1.0f));
        } else {
            transform = glm::scale(transform, glm::vec3(particle->size(), 1.0f));
        }

        uniforms.particles->particles[i].transform = move(transform);
        uniforms.particles->particles[i].dir = glm::vec4(particle->dir(), 1.0f);
        uniforms.particles->particles[i].color = glm::vec4(particle->color(), particle->alpha());
        uniforms.particles->particles[i].size = glm::vec2(particle->size());
        uniforms.particles->particles[i].frame = particle->frame();
    }

    _shaders.activate(ShaderProgram::ParticleParticle, uniforms);
    _context.bindTexture(TextureUnits::diffuseMap, texture);

    BlendMode oldBlendMode(_context.blendMode());
    bool lighten = emitter->blendMode == ModelNode::Emitter::BlendMode::Lighten;
    if (lighten) {
        _context.setBlendMode(BlendMode::Lighten);
    }
    _meshes.billboard().drawInstanced(count);
    _context.setBlendMode(oldBlendMode);
}

unique_ptr<ParticleSceneNode> EmitterSceneNode::newParticle() {
    return make_unique<ParticleSceneNode>(*this, _sceneGraph);
}

} // namespace scene

} // namespace reone
