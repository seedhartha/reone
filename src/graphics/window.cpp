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

#include "window.h"

#include "SDL2/SDL.h"

#include "cursor.h"
#include "eventhandler.h"

using namespace std;

namespace reone {

namespace graphics {

void Window::init() {
    if (_inited) {
        return;
    }
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    _window = SDL_CreateWindow(
        "reone",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        _options.width, _options.height,
        getWindowFlags());
    if (!_window) {
        throw runtime_error("Failed to create a window: " + string(SDL_GetError()));
    }

    _context = SDL_GL_CreateContext(_window);
    if (!_context) {
        throw runtime_error("Failed to create a GL context: " + string(SDL_GetError()));
    }

    SDL_GL_SetSwapInterval(0);
    _inited = true;
}

int Window::getWindowFlags() const {
    int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
    if (_options.fullscreen) {
        flags |= SDL_WINDOW_FULLSCREEN;
    }
    return flags;
}

void Window::deinit() {
    if (!_inited) {
        return;
    }
    SDL_GL_DeleteContext(_context);
    SDL_DestroyWindow(_window);
    SDL_Quit();
    _inited = false;
}

void Window::processEvents(bool &quit) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (handleEvent(event, quit)) {
            continue;
        }
        _eventHandler->handle(event);
    }
}

bool Window::handleEvent(const SDL_Event &event, bool &quit) {
    switch (event.type) {
    case SDL_QUIT:
        quit = true;
        return true;
    case SDL_KEYDOWN:
        return handleKeyDownEvent(event.key, quit);
    case SDL_WINDOWEVENT:
        return handleWindowEvent(event.window);
    default:
        return false;
    }
}

bool Window::handleKeyDownEvent(const SDL_KeyboardEvent &event, bool &quit) {
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

bool Window::handleWindowEvent(const SDL_WindowEvent &event) {
    switch (event.event) {
    case SDL_WINDOWEVENT_FOCUS_GAINED:
        _focus = true;
        return true;
    case SDL_WINDOWEVENT_FOCUS_LOST:
        _focus = false;
        return true;
    default:
        return false;
    }
}

void Window::drawCursor() const {
    if (_relativeMouseMode || !_cursor) {
        return;
    }
    int x, y;
    uint32_t state = SDL_GetMouseState(&x, &y);
    bool pressed = state & SDL_BUTTON(1);

    _cursor->setPosition({x, y});
    _cursor->setPressed(pressed);
    _cursor->draw();
}

void Window::swapBuffers() const {
    SDL_GL_SwapWindow(_window);
}

glm::mat4 Window::getOrthoProjection(float near, float far) const {
    return glm::ortho(0.0f, static_cast<float>(_options.width), static_cast<float>(_options.height), 0.0f, near, far);
}

void Window::setRelativeMouseMode(bool enabled) {
    SDL_SetRelativeMouseMode(enabled ? SDL_TRUE : SDL_FALSE);
    _relativeMouseMode = enabled;
}

void Window::setCursor(const shared_ptr<Cursor> &cursor) {
    _cursor = cursor;
    SDL_ShowCursor(!static_cast<bool>(cursor));
}

} // namespace graphics

} // namespace reone
