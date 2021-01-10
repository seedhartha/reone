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

#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/matrix_decompose.hpp"

#include "../../common/random.h"
#include "../../render/mesh/billboard.h"
#include "../../render/shaders.h"
#include "../../render/util.h"

#include "../scenegraph.h"

#include "cameranode.h"

using namespace std;

using namespace reone::render;

namespace reone {

namespace scene {

constexpr int kMaxParticleCount = 16;
constexpr float kParticleFrameInterval = 0.2f;

EmitterSceneNode::EmitterSceneNode(const shared_ptr<ModelNode> &modelNode, const shared_ptr<Emitter> &emitter, SceneGraph *sceneGraph) :
    SceneNode(sceneGraph),
    _modelNode(modelNode),
    _emitter(emitter) {

    if (!modelNode) {
        throw invalid_argument("modelNode must not be null");
    }
    if (!emitter) {
        throw invalid_argument("emitter must not be null");
    }

    init();
}

void EmitterSceneNode::init() {
    _birthInterval = 1.0f / _emitter->birthrate();
}

void EmitterSceneNode::update(float dt) {
    if (_emitter->updateType() != Emitter::UpdateType::Fountain) return;

    spawnParticles(dt);
    updateParticles(dt);
}

void EmitterSceneNode::spawnParticles(float dt) {
    _birthTimer.update(dt);

    if (_birthTimer.hasTimedOut()) {
        if (_particles.size() < kMaxParticleCount) {
            float halfW = 0.005f * _emitter->size().x;
            float halfH = 0.005f * _emitter->size().y;

            auto particle = make_shared<Particle>();
            particle->position = _absoluteTransform * glm::vec4(random(-halfW, halfW), random(-halfH, halfH), 0.0f, 1.0f);
            particle->frame = _emitter->frameStart();
            particle->frameTimer.reset(kParticleFrameInterval);
            particle->normal = getPlaneNormal();
            particle->velocity = (_emitter->velocity() + random(0.0f, _emitter->randomVelocity()));
            particle->lifetime = _emitter->lifeExpectancy();

            _particles.push_back(move(particle));
        }
        _birthTimer.reset(_birthInterval);
    }
}

glm::vec3 EmitterSceneNode::getPlaneNormal() const {
    static glm::vec3 up(0.0f, 0.0f, 1.0f);

    glm::vec3 scale, translation, skew;
    glm::quat orientation;
    glm::vec4 perspective;

    glm::decompose(_modelNode->absoluteTransform(), scale, orientation, translation, skew, perspective);

    return glm::normalize(orientation * up);
}

void EmitterSceneNode::updateParticles(float dt) {
    for (auto it = _particles.begin(); it != _particles.end(); ) {
        auto &particle = (*it);

        // Expire the particle
        particle->lifetime = glm::max(0.0f, particle->lifetime - dt);
        if (particle->lifetime == 0.0f) {
            it = _particles.erase(it);
        } else {
            // Update the particle position
            particle->position += particle->velocity * particle->normal * dt; // TODO: use velocity?

            // Animate the particle
            particle->frameTimer.update(dt);
            if (particle->frameTimer.hasTimedOut()) {
                ++particle->frame;
                if (particle->frame > _emitter->frameEnd()) {
                    particle->frame = _emitter->frameStart();
                }
                particle->frameTimer.reset(kParticleFrameInterval);
            }

            ++it;
        }
    }
}

void EmitterSceneNode::renderSingle(bool shadowPass) const {
    if (shadowPass) return;

    for (auto &particle : _particles) {
        glm::mat4 transform(1.0f);
        transform = glm::translate(transform, particle->position);
        transform = glm::scale(transform, glm::vec3(_emitter->sizeStart()));

        LocalUniforms locals;
        locals.general.model = move(transform);
        locals.general.billboardGridSize = glm::vec2(_emitter->gridWidth(), _emitter->gridHeight());
        locals.general.billboardFrame = particle->frame;

        Shaders::instance().activate(ShaderProgram::ModelBillboard, locals);

        setActiveTextureUnit(0);
        _emitter->texture()->bind();

        BillboardMesh::instance().renderTriangles();
    }
}

} // namespace scene

} // namespace reone
