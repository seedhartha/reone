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

    static GlobalUniforms defaultGlobals;
    static LocalUniforms defaultLocals;
    static SceneGlobalUniforms defaultSceneGlobals;
    static SceneLocalUniforms defaultSceneLocals;
    static BoneUniforms defaultBones;
    static ParticleUniforms defaultParticles;
    static GrassUniforms defaultGrass;
    static WalkmeshUniforms defaultWalkmesh;
    static TextUniforms defaultText;
    static ScreenEffectUniforms defaultScreenEffect;

    _ubGlobals = initBuffer(&defaultGlobals, sizeof(GlobalUniforms));
    _ubLocals = initBuffer(&defaultLocals, sizeof(LocalUniforms));
    _ubSceneGlobals = initBuffer(&defaultSceneGlobals, sizeof(SceneGlobalUniforms));
    _ubSceneLocals = initBuffer(&defaultSceneLocals, sizeof(SceneLocalUniforms));
    _ubBones = initBuffer(&defaultBones, sizeof(BoneUniforms));
    _ubParticles = initBuffer(&defaultParticles, sizeof(ParticleUniforms));
    _ubGrass = initBuffer(&defaultGrass, sizeof(GrassUniforms));
    _ubWalkmesh = initBuffer(&defaultWalkmesh, sizeof(WalkmeshUniforms));
    _ubText = initBuffer(&defaultText, sizeof(TextUniforms));
    _ubScreenEffect = initBuffer(&defaultScreenEffect, sizeof(ScreenEffectUniforms));

    _context.bind(*_ubGlobals, UniformBlockBindingPoints::globals);
    _context.bind(*_ubLocals, UniformBlockBindingPoints::locals);
    _context.bind(*_ubSceneGlobals, UniformBlockBindingPoints::sceneGlobals);
    _context.bind(*_ubSceneLocals, UniformBlockBindingPoints::sceneLocals);
    _context.bind(*_ubBones, UniformBlockBindingPoints::bones);
    _context.bind(*_ubParticles, UniformBlockBindingPoints::particles);
    _context.bind(*_ubGrass, UniformBlockBindingPoints::grass);
    _context.bind(*_ubWalkmesh, UniformBlockBindingPoints::walkmesh);
    _context.bind(*_ubText, UniformBlockBindingPoints::text);
    _context.bind(*_ubScreenEffect, UniformBlockBindingPoints::screenEffect);

    _inited = true;
}

void Uniforms::deinit() {
    if (!_inited) {
        return;
    }

    _ubGlobals.reset();
    _ubLocals.reset();
    _ubSceneGlobals.reset();
    _ubSceneLocals.reset();
    _ubBones.reset();
    _ubParticles.reset();
    _ubGrass.reset();
    _ubWalkmesh.reset();
    _ubText.reset();
    _ubScreenEffect.reset();

    _inited = false;
}

void Uniforms::setGlobals(const std::function<void(GlobalUniforms &)> &block) {
    block(_globals);
    _context.bind(*_ubGlobals, UniformBlockBindingPoints::globals);
    _ubGlobals->setData(&_globals, sizeof(GlobalUniforms));
}

void Uniforms::setLocals(const std::function<void(LocalUniforms &)> &block) {
    block(_locals);
    _context.bind(*_ubLocals, UniformBlockBindingPoints::locals);
    _ubLocals->setData(&_locals, sizeof(LocalUniforms));
}

void Uniforms::setSceneGlobals(const std::function<void(SceneGlobalUniforms &)> &block) {
    block(_sceneGlobals);
    _context.bind(*_ubSceneGlobals, UniformBlockBindingPoints::sceneGlobals);
    _ubSceneGlobals->setData(&_sceneGlobals, sizeof(SceneGlobalUniforms));
}

void Uniforms::setSceneLocals(const std::function<void(SceneLocalUniforms &)> &block) {
    block(_sceneLocals);
    _context.bind(*_ubSceneLocals, UniformBlockBindingPoints::sceneLocals);
    _ubSceneLocals->setData(&_sceneLocals, sizeof(SceneLocalUniforms));
}

void Uniforms::setBones(const std::function<void(BoneUniforms &)> &block) {
    block(_bones);
    _context.bind(*_ubBones, UniformBlockBindingPoints::bones);
    _ubBones->setData(&_bones, sizeof(BoneUniforms));
}

void Uniforms::setParticles(const std::function<void(ParticleUniforms &)> &block) {
    block(_particles);
    _context.bind(*_ubParticles, UniformBlockBindingPoints::particles);
    _ubParticles->setData(&_particles, sizeof(ParticleUniforms));
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

void Uniforms::setText(const std::function<void(TextUniforms &)> &block) {
    block(_text);
    _context.bind(*_ubText, UniformBlockBindingPoints::text);
    _ubText->setData(&_text, sizeof(TextUniforms));
}

void Uniforms::setScreenEffect(const std::function<void(ScreenEffectUniforms &)> &block) {
    block(_screenEffect);
    _context.bind(*_ubScreenEffect, UniformBlockBindingPoints::screenEffect);
    _ubScreenEffect->setData(&_screenEffect, sizeof(ScreenEffectUniforms));
}

std::unique_ptr<UniformBuffer> Uniforms::initBuffer(const void *data, ptrdiff_t size) {
    auto buf = std::make_unique<UniformBuffer>();
    buf->setData(data, size, false);
    buf->init();
    return buf;
}

} // namespace graphics

} // namespace reone
