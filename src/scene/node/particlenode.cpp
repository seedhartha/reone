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

#include "particlenode.h"

#include <stdexcept>

#include "glm/common.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "GL/glew.h"
#include "SDL2/SDL_opengl.h"

#include "../../render/mesh/billboard.h"
#include "../../render/shaders.h"
#include "../../render/util.h"

using namespace std;

using namespace reone::render;

namespace reone {

namespace scene {

ParticleSceneNode::ParticleSceneNode(glm::vec3 position, float velocity, const shared_ptr<Emitter> &emitter, SceneGraph *sceneGraph) :
    SceneNode(sceneGraph),
    _position(position),
    _velocity(velocity),
    _emitter(emitter) {

    if (!emitter) {
        throw invalid_argument("emitter must not be null");
    }

    init();
}

void ParticleSceneNode::init() {
    _renderOrder = _emitter->renderOrder();
    _frame = _emitter->frameStart();

    updateLocalTransform();
}

void ParticleSceneNode::updateLocalTransform() {
    setLocalTransform(glm::translate(glm::mat4(1.0f), _position));
}

void ParticleSceneNode::update(float dt) {
    _lifetime = glm::min(_lifetime + dt, static_cast<float>(_emitter->lifeExpectancy()));

    if (_lifetime < _emitter->lifeExpectancy()) {
        _position.z += _velocity * dt;
        updateLocalTransform();
    }

    updateAnimation(dt);
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

void ParticleSceneNode::updateAnimation(float dt) {
    float maturity = _lifetime / static_cast<float>(_emitter->lifeExpectancy());
    _frame =  glm::ceil(_emitter->frameStart() + maturity * (_emitter->frameEnd() - _emitter->frameStart()));
    _size = interpolateConstraints(_emitter->particleSize(), maturity);
    _color = interpolateConstraints(_emitter->color(), maturity);
    _alpha = interpolateConstraints(_emitter->alpha(), maturity);
}

void ParticleSceneNode::renderSingle(bool shadowPass) const {
    if (shadowPass) return;

    shared_ptr<Texture> texture(_emitter->texture());
    if (!texture) return;

    LocalUniforms locals;
    locals.general.color = glm::vec4(_color, 1.0f);
    locals.general.alpha = _alpha;
    locals.general.billboardGridSize = glm::vec2(_emitter->gridWidth(), _emitter->gridHeight());
    locals.general.billboardSize = glm::vec2(_size);
    locals.general.particleCenter = _absoluteTransform[3];
    locals.general.billboardFrame = _frame;
    locals.general.billboardToWorldZ = _emitter->renderType() == Emitter::RenderType::BillboardToWorldZ;

    Shaders::instance().activate(ShaderProgram::BillboardBillboard, locals);

    setActiveTextureUnit(0);
    texture->bind();

    bool lighten = _emitter->blendType() == Emitter::BlendType::Lighten;
    if (lighten) {
        withAdditiveBlending([]() { BillboardMesh::instance().renderTriangles(); });
    } else {
        BillboardMesh::instance().renderTriangles();
    }
}

bool ParticleSceneNode::isExpired() const {
    return _lifetime >= _emitter->lifeExpectancy();
}

int ParticleSceneNode::renderOrder() const {
    return _renderOrder;
}

} // namespace scene

} // namespace reone
