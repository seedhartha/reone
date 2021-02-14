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

#include "../render/meshes.h"
#include "../render/shaders.h"

using namespace std;

using namespace reone::audio;
using namespace reone::render;

namespace reone {

namespace video {

void Video::init() {
    if (_inited) return;

    glGenTextures(1, &_textureId);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    _inited = true;
}

void Video::deinit() {
    if (!_inited) return;

    glDeleteTextures(1, &_textureId);

    _inited = false;
}

void Video::update(float dt) {
    if (_finished) return;

    updateFrame(dt);
    updateFrameTexture();
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

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, &_frame->data[0]);
}

void Video::render() {
    if (!_inited) return;

    GlobalUniforms globals;
    globals.projection = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f);
    Shaders::instance().setGlobalUniforms(globals);

    LocalUniforms locals;
    Shaders::instance().activate(ShaderProgram::SimpleGUI, locals);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureId);

    Meshes::instance().getQuad().render();

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Video::finish() {
    _finished = true;
}

void Video::setMediaStream(const shared_ptr<MediaStream<Frame>> &stream) {
    _stream = stream;
}

} // namespace video

} // namespace reone
