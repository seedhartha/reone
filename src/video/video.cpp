/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "GL/glew.h"

#include "SDL2/SDL_opengl.h"

#include "../render/mesh/quad.h"
#include "../render/shaders.h"

using namespace reone::render;

namespace reone {

namespace video {

void Video::init() {
    if (_inited) return;

    glGenTextures(1, &_textureId);
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

    _finished = true;
}

void Video::render() {
    if (!_inited) return;

    GlobalUniforms globals;
    Shaders::instance().setGlobalUniforms(globals);

    LocalUniforms locals;
    Shaders::instance().activate(ShaderProgram::GUIGUI, locals);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureId);

    Quad::getDefault().renderTriangles();

    glBindTexture(GL_TEXTURE_2D, 0);
}

bool Video::isFinished() const {
    return _finished;
}

} // namespace video

} // namespace reone
