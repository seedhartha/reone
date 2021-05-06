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

#include "statemanager.h"

#include "GL/glew.h"
#include "SDL2/SDL_opengl.h"

using namespace std;

namespace reone {

namespace graphics {

StateManager &StateManager::instance() {
    static StateManager instance;
    return instance;
}

void StateManager::withWireframes(const function<void()> &block) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    block();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void StateManager::withViewport(const glm::ivec4 &viewport, const function<void()> &block) {
    int oldViewport[4];
    glGetIntegerv(GL_VIEWPORT, &oldViewport[0]);
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

    block();

    glViewport(oldViewport[0], oldViewport[1], oldViewport[2], oldViewport[3]);
}

void StateManager::withScissorTest(const glm::ivec4 &bounds, const function<void()> &block) {
    glEnable(GL_SCISSOR_TEST);
    glScissor(bounds[0], bounds[1], bounds[2], bounds[3]);
    glClear(GL_COLOR_BUFFER_BIT);

    block();

    glDisable(GL_SCISSOR_TEST);
}

void StateManager::withDepthTest(const function<void()> &block) {
    setDepthTestEnabled(true);
    block();
    setDepthTestEnabled(false);
}

void StateManager::setDepthTestEnabled(bool enabled) {
    if (_depthTestEnabled != enabled) {
        if (enabled) {
            glEnable(GL_DEPTH_TEST);
        } else {
            glDisable(GL_DEPTH_TEST);
        }
        _depthTestEnabled = enabled;
    }
}

static void withBlending(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha, const function<void()> &block) {
    GLint blendSrcRgb, blendSrcAlpha, blendDstRgb, blendDstAlpha;
    glGetIntegerv(GL_BLEND_SRC_RGB, &blendSrcRgb);
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrcAlpha);
    glGetIntegerv(GL_BLEND_DST_RGB, &blendDstRgb);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDstAlpha);
    glBlendFuncSeparate(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);

    block();

    glBlendFuncSeparate(blendSrcRgb, blendDstRgb, blendSrcAlpha, blendDstAlpha);
}

void StateManager::withAdditiveBlending(const function<void()> &block) {
    withBlending(GL_SRC_ALPHA, GL_ONE, GL_SRC_ALPHA, GL_ONE, block);
}

void StateManager::withBackFaceCulling(const function<void()> &block) {
    glEnable(GL_CULL_FACE);
    block();
    glDisable(GL_CULL_FACE);
}

void StateManager::setActiveTextureUnit(int n) {
    if (_textureUnit != n) {
        glActiveTexture(GL_TEXTURE0 + n);
        _textureUnit = n;
    }
}

} // namespace graphics

} // namespace reone
