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

#include "reone/graphics/context.h"

namespace reone {

namespace graphics {

void Uniforms::init() {
    if (_inited) {
        return;
    }

    static GlobalsUniforms defaultsGlobals;
    static LocalsUniforms defaultsLocals;
    static TextUniforms defaultsText;
    static LightingUniforms defaultsLighting;
    static SkeletalUniforms defaultsSkeletal;
    static ParticlesUniforms defaultsParticles;
    static GrassUniforms defaultsGrass;
    static WalkmeshUniforms defaultsWalkmesh;
    static PointsUniforms defaultsPoints;
    static ScreenSpaceUniforms defaultsScreenSpace;

    _ubGlobals = initBuffer(&defaultsGlobals, sizeof(GlobalsUniforms));
    _ubLocals = initBuffer(&defaultsLocals, sizeof(LocalsUniforms));
    _ubText = initBuffer(&defaultsText, sizeof(TextUniforms));
    _ubLighting = initBuffer(&defaultsLighting, sizeof(LightingUniforms));
    _ubSkeletal = initBuffer(&defaultsSkeletal, sizeof(SkeletalUniforms));
    _ubParticles = initBuffer(&defaultsParticles, sizeof(ParticlesUniforms));
    _ubGrass = initBuffer(&defaultsGrass, sizeof(GrassUniforms));
    _ubWalkmesh = initBuffer(&defaultsWalkmesh, sizeof(WalkmeshUniforms));
    _ubPoints = initBuffer(&defaultsPoints, sizeof(PointsUniforms));
    _ubScreenSpace = initBuffer(&defaultsScreenSpace, sizeof(ScreenSpaceUniforms));

    _context.bind(*_ubGlobals, UniformBlockBindingPoints::globals);
    _context.bind(*_ubLocals, UniformBlockBindingPoints::locals);
    _context.bind(*_ubText, UniformBlockBindingPoints::text);
    _context.bind(*_ubLighting, UniformBlockBindingPoints::lighting);
    _context.bind(*_ubSkeletal, UniformBlockBindingPoints::skeletal);
    _context.bind(*_ubParticles, UniformBlockBindingPoints::particles);
    _context.bind(*_ubGrass, UniformBlockBindingPoints::grass);
    _context.bind(*_ubWalkmesh, UniformBlockBindingPoints::walkmesh);
    _context.bind(*_ubPoints, UniformBlockBindingPoints::points);
    _context.bind(*_ubScreenSpace, UniformBlockBindingPoints::screenSpace);

    _inited = true;
}

void Uniforms::deinit() {
    if (!_inited) {
        return;
    }

    _ubGlobals.reset();
    _ubLocals.reset();
    _ubText.reset();
    _ubLighting.reset();
    _ubSkeletal.reset();
    _ubParticles.reset();
    _ubGrass.reset();
    _ubWalkmesh.reset();
    _ubPoints.reset();
    _ubScreenSpace.reset();

    _inited = false;
}

void Uniforms::setGlobals(const std::function<void(GlobalsUniforms &)> &block) {
    block(_globals);
    _context.bind(*_ubGlobals, UniformBlockBindingPoints::globals);
    _ubGlobals->setData(&_globals, sizeof(GlobalsUniforms));
}

void Uniforms::setLocals(const std::function<void(LocalsUniforms &)> &block) {
    block(_locals);
    _context.bind(*_ubLocals, UniformBlockBindingPoints::locals);
    _ubLocals->setData(&_locals, sizeof(LocalsUniforms));
}

void Uniforms::setText(const std::function<void(TextUniforms &)> &block) {
    block(_text);
    _context.bind(*_ubText, UniformBlockBindingPoints::text);
    _ubText->setData(&_text, sizeof(TextUniforms));
}

void Uniforms::setLighting(const std::function<void(LightingUniforms &)> &block) {
    block(_lighting);
    _context.bind(*_ubLighting, UniformBlockBindingPoints::lighting);
    _ubLighting->setData(&_lighting, sizeof(LightingUniforms));
}

void Uniforms::setSkeletal(const std::function<void(SkeletalUniforms &)> &block) {
    block(_skeletal);
    _context.bind(*_ubSkeletal, UniformBlockBindingPoints::skeletal);
    _ubSkeletal->setData(&_skeletal, sizeof(SkeletalUniforms));
}

void Uniforms::setParticles(const std::function<void(ParticlesUniforms &)> &block) {
    block(_particles);
    _context.bind(*_ubParticles, UniformBlockBindingPoints::particles);
    _ubParticles->setData(&_particles, sizeof(ParticlesUniforms));
}

void Uniforms::setGrass(const std::function<void(GrassUniforms &)> &block) {
    block(_grass);
    _context.bind(*_ubGrass, UniformBlockBindingPoints::grass);
    _ubGrass->setData(&_grass, sizeof(GrassUniforms));
}

void Uniforms::setWalkmesh(const std::function<void(WalkmeshUniforms &)> &block) {
    block(_walkmesh);
    _context.bind(*_ubWalkmesh, UniformBlockBindingPoints::walkmesh);
    _ubWalkmesh->setData(&_walkmesh, sizeof(WalkmeshUniforms));
}

void Uniforms::setPoints(const std::function<void(PointsUniforms &)> &block) {
    block(_points);
    _context.bind(*_ubPoints, UniformBlockBindingPoints::points);
    _ubPoints->setData(&_points, sizeof(PointsUniforms));
}

void Uniforms::setScreenSpace(const std::function<void(ScreenSpaceUniforms &)> &block) {
    block(_screenSpace);
    _context.bind(*_ubScreenSpace, UniformBlockBindingPoints::screenSpace);
    _ubScreenSpace->setData(&_screenSpace, sizeof(ScreenSpaceUniforms));
}

std::unique_ptr<UniformBuffer> Uniforms::initBuffer(const void *data, ptrdiff_t size) {
    auto buf = std::make_unique<UniformBuffer>();
    buf->setData(data, size, false);
    buf->init();
    return buf;
}

} // namespace graphics

} // namespace reone
