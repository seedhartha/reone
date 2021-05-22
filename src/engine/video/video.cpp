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

#include "video.h"

#include <utility>

#include "GL/glew.h"
#include "SDL2/SDL_opengl.h"

#include "glm/ext.hpp"

#include "../common/guardutil.h"
#include "../graphics/mesh/meshes.h"
#include "../graphics/shader/shaders.h"
#include "../graphics/stateutil.h"
#include "../graphics/texture/textureutil.h"

using namespace std;

using namespace reone::audio;
using namespace reone::graphics;

namespace reone {

namespace video {

Video::Video(Shaders *shaders, Meshes *meshes) : _shaders(shaders), _meshes(meshes) {
    ensureNotNull(shaders, "shaders");
    ensureNotNull(meshes, "meshes");
}

void Video::init() {
    if (!_inited) {
        _texture = make_shared<Texture>("video", getTextureProperties(TextureUsage::Video));
        _texture->init();
        _inited = true;
    }
}

void Video::deinit() {
    if (_inited) {
        _texture.reset();
        _inited = false;
    }
}

void Video::update(float dt) {
    if (!_finished) {
        updateFrame(dt);
        updateFrameTexture();
    }
}

void Video::updateFrame(float dt) {
    _time += dt;

    int frame = static_cast<int>(_fps * _time);
    _frame = _stream->get(frame);

    if (!_frame) {
        _finished = true;
    }
}

void Video::updateFrameTexture() {
    if (!_frame) return;

    setActiveTextureUnit(TextureUnits::diffuseMap);
    _texture->bind();
    _texture->setPixels(_width, _height, PixelFormat::RGB, _frame->pixels);
}

void Video::draw() {
    if (!_inited) return;

    setActiveTextureUnit(TextureUnits::diffuseMap);
    _texture->bind();

    ShaderUniforms uniforms;
    _shaders->activate(ShaderProgram::SimpleGUI, uniforms);
    _meshes->quadNDCFlipY().draw();
}

void Video::finish() {
    _finished = true;
}

void Video::setMediaStream(const shared_ptr<MediaStream<Frame>> &stream) {
    _stream = stream;
}

} // namespace video

} // namespace reone
