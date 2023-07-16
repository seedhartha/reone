/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/graphics/uniforms.h"

namespace reone {

namespace graphics {

void Uniforms::init() {
    if (_inited) {
        return;
    }

    static GeneralUniforms defaultsGeneral;
    static TextUniforms defaultsText;
    static LightingUniforms defaultsLighting;
    static SkeletalUniforms defaultsSkeletal;
    static ParticlesUniforms defaultsParticles;
    static GrassUniforms defaultsGrass;
    static SSAOUniforms defaultsSSAO;
    static WalkmeshUniforms defaultsWalkmesh;
    static PointsUniforms defaultsPoints;

    _ubGeneral = initBuffer(&defaultsGeneral, sizeof(GeneralUniforms));
    _ubText = initBuffer(&defaultsText, sizeof(TextUniforms));
    _ubLighting = initBuffer(&defaultsLighting, sizeof(LightingUniforms));
    _ubSkeletal = initBuffer(&defaultsSkeletal, sizeof(SkeletalUniforms));
    _ubParticles = initBuffer(&defaultsParticles, sizeof(ParticlesUniforms));
    _ubGrass = initBuffer(&defaultsGrass, sizeof(GrassUniforms));
    _ubSSAO = initBuffer(&defaultsSSAO, sizeof(SSAOUniforms));
    _ubWalkmesh = initBuffer(&defaultsWalkmesh, sizeof(WalkmeshUniforms));
    _ubPoints = initBuffer(&defaultsPoints, sizeof(PointsUniforms));

    _inited = true;
}

void Uniforms::deinit() {
    if (!_inited) {
        return;
    }

    _ubGeneral.reset();
    _ubText.reset();
    _ubLighting.reset();
    _ubSkeletal.reset();
    _ubParticles.reset();
    _ubGrass.reset();
    _ubSSAO.reset();
    _ubWalkmesh.reset();
    _ubPoints.reset();

    _inited = false;
}

void Uniforms::setGeneral(const std::function<void(GeneralUniforms &)> &block) {
    block(_general);
    refreshBuffer(*_ubGeneral, UniformBlockBindingPoints::general, &_general, sizeof(GeneralUniforms));
}

void Uniforms::setText(const std::function<void(TextUniforms &)> &block) {
    block(_text);
    refreshBuffer(*_ubText, UniformBlockBindingPoints::text, &_text, sizeof(TextUniforms));
}

void Uniforms::setLighting(const std::function<void(LightingUniforms &)> &block) {
    block(_lighting);
    refreshBuffer(*_ubLighting, UniformBlockBindingPoints::lighting, &_lighting, sizeof(LightingUniforms));
}

void Uniforms::setSkeletal(const std::function<void(SkeletalUniforms &)> &block) {
    block(_skeletal);
    refreshBuffer(*_ubSkeletal, UniformBlockBindingPoints::skeletal, &_skeletal, sizeof(SkeletalUniforms));
}

void Uniforms::setParticles(const std::function<void(ParticlesUniforms &)> &block) {
    block(_particles);
    refreshBuffer(*_ubParticles, UniformBlockBindingPoints::particles, &_particles, sizeof(ParticlesUniforms));
}

void Uniforms::setGrass(const std::function<void(GrassUniforms &)> &block) {
    block(_grass);
    refreshBuffer(*_ubGrass, UniformBlockBindingPoints::grass, &_grass, sizeof(GrassUniforms));
}

void Uniforms::setSSAO(const std::function<void(SSAOUniforms &)> &block) {
    block(_ssao);
    refreshBuffer(*_ubSSAO, UniformBlockBindingPoints::ssao, &_ssao, sizeof(SSAOUniforms));
}

void Uniforms::setWalkmesh(const std::function<void(WalkmeshUniforms &)> &block) {
    block(_walkmesh);
    refreshBuffer(*_ubWalkmesh, UniformBlockBindingPoints::walkmesh, &_walkmesh, sizeof(WalkmeshUniforms));
}

void Uniforms::setPoints(const std::function<void(PointsUniforms &)> &block) {
    block(_points);
    refreshBuffer(*_ubPoints, UniformBlockBindingPoints::points, &_points, sizeof(PointsUniforms));
}

std::unique_ptr<UniformBuffer> Uniforms::initBuffer(const void *data, ptrdiff_t size) {
    auto buf = std::make_unique<UniformBuffer>();
    buf->setData(data, size);
    buf->init();
    return buf;
}

void Uniforms::refreshBuffer(UniformBuffer &buffer, int bindingPoint, const void *data, ptrdiff_t size) {
    buffer.bind(bindingPoint);
    buffer.setData(data, size, true);
}

} // namespace graphics

} // namespace reone
