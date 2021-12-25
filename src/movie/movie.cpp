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

#include "movie.h"

#include "../audio/player.h"
#include "../graphics/context.h"
#include "../graphics/mesh.h"
#include "../graphics/meshes.h"
#include "../graphics/shaders.h"
#include "../graphics/textures.h"
#include "../graphics/textureutil.h"

using namespace std;

using namespace reone::audio;
using namespace reone::graphics;

namespace reone {

namespace movie {

void Movie::init() {
    if (!_texture && _videoStream) {
        _width = _videoStream->width();
        _height = _videoStream->height();
        _texture = make_shared<Texture>("video", getTextureProperties(TextureUsage::Video));
        _texture->clear(1, 1, PixelFormat::RGB, 1);
        _texture->init();
    }
    if (!_audioSource && _audioStream) {
        _audioSource = _audioPlayer.play(_audioStream, AudioType::Movie);
    }
}

void Movie::deinit() {
    if (_audioSource) {
        _audioSource.reset();
    }
    if (_audioStream) {
        _audioStream.reset();
    }
    if (_texture) {
        _texture.reset();
    }
    if (_videoStream) {
        _videoStream.reset();
    }
}

void Movie::update(float dt) {
    if (_finished) {
        return;
    }
    _time += dt;
    _videoStream->seek(_time);
    if (_videoStream->hasEnded()) {
        _finished = true;
        return;
    }
    if (_audioSource) {
        _audioSource->update();
    }
}

void Movie::draw() {
    auto &frame = _videoStream->frame();
    if (frame.pixels) {
        _textures.bind(*_texture);
        _texture->setPixels(_width, _height, PixelFormat::RGB, frame.pixels, true);
    }
    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetGlobals();
    uniforms.general.resetLocals();
    uniforms.general.uv = glm::mat3x4(
        glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
        glm::vec4(0.0f, -1.0f, 0.0f, 0.0f),
        glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));

    _graphicsContext.clearColorDepth();
    _shaders.use(_shaders.gui(), true);
    _meshes.quadNDC().draw();
}

} // namespace movie

} // namespace reone
