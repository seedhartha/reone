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

#include "window.h"

#include <stdexcept>

#include "GL/glew.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"

#include "glm/ext.hpp"

#include "mesh/aabb.h"
#include "mesh/guiquad.h"

#include "shadermanager.h"

namespace reone {

namespace render {

RenderWindow::RenderWindow(const GraphicsOptions &opts, IEventHandler *eventHandler) : _eventHandler(eventHandler), _opts(opts) {
}

void RenderWindow::init() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN;
    if (_opts.fullscreen) flags |= SDL_WINDOW_FULLSCREEN;

    _window = SDL_CreateWindow(
        "reone",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        _opts.width,
        _opts.height,
        flags);

    if (!_window) {
        throw std::runtime_error("Failed to create a window: " + std::string(SDL_GetError()));
    }

    _context = SDL_GL_CreateContext(_window);

    if (!_context) {
        throw std::runtime_error("Failed to create a GL context: " + std::string(SDL_GetError()));
    }

    SDL_GL_SetSwapInterval(0);
    glewInit();

    ShaderManager::instance().initGL();
    AABBMesh::instance().initGL();
    GUIQuad::instance().initGL();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void RenderWindow::deinit() {
    GUIQuad::instance().deinitGL();
    AABBMesh::instance().deinitGL();
    ShaderManager::instance().deinitGL();
    SDL_GL_DeleteContext(_context);
    SDL_DestroyWindow(_window);
    SDL_Quit();
}

void RenderWindow::show() {
    SDL_ShowWindow(_window);
}

void RenderWindow::processEvents(bool &quit) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (handleEvent(event, quit)) continue;
        if (!_eventHandler) continue;

        _eventHandler->handle(event);
    }
}

bool RenderWindow::handleEvent(const SDL_Event &event, bool &quit) {
    switch (event.type) {
        case SDL_QUIT:
            quit = true;
            return true;

        case SDL_KEYDOWN:
            return handleKeyDownEvent(event.key, quit);

        default:
            return false;
    }
}

bool RenderWindow::handleKeyDownEvent(const SDL_KeyboardEvent &event, bool &quit) {
    switch (event.keysym.scancode) {
        case SDL_SCANCODE_C:
            if (event.keysym.mod & KMOD_CTRL) {
                quit = true;
                return true;
            }
            return false;

        default:
            return false;
    }
}

void RenderWindow::render(const std::shared_ptr<Camera> &camera) const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderWorld(camera);
    renderGUI();

    SDL_GL_SwapWindow(_window);
}

void RenderWindow::renderWorld(const std::shared_ptr<Camera> &camera) const {
    if (!camera) return;

    glEnable(GL_DEPTH_TEST);

    ShaderUniforms uniforms;
    uniforms.projection = camera->projection();
    uniforms.view = camera->view();
    uniforms.cameraPosition = camera->position();

    ShaderManager::instance().setGlobalUniforms(uniforms);

    if (_onRenderWorld) _onRenderWorld();
}

void RenderWindow::renderGUI() const {
    glDisable(GL_DEPTH_TEST);

    ShaderUniforms uniforms;
    uniforms.projection = glm::ortho(0.0f, static_cast<float>(_opts.width), static_cast<float>(_opts.height), 0.0f);
    uniforms.view = glm::mat4(1.0f);

    ShaderManager::instance().setGlobalUniforms(uniforms);

    if (_onRenderGUI) _onRenderGUI();
}

void RenderWindow::setRelativeMouseMode(bool enabled) {
    SDL_SetRelativeMouseMode(enabled ? SDL_TRUE : SDL_FALSE);
}

void RenderWindow::setRenderWorldFunc(const std::function<void()> &fn) {
    _onRenderWorld = fn;
}

void RenderWindow::setRenderGUIFunc(const std::function<void()> &fn) {
    _onRenderGUI = fn;
}

} // namespace render

} // namespace reone
