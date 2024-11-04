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

#include "reone/graphics/window.h"

#include "SDL2/SDL.h"

#include "reone/system/checkutil.h"
#include "reone/system/threadutil.h"

namespace reone {

namespace graphics {

void Window::init() {
    checkThat(!_inited, "Must not be initialized");
    checkMainThread();

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI;
    if (_options.fullscreen) {
        flags |= SDL_WINDOW_FULLSCREEN;
    }
    _window = SDL_CreateWindow(
        "reone",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        _options.width * _options.winScale / 100,
        _options.height * _options.winScale / 100,
        flags);
    if (!_window) {
        throw std::runtime_error("SDL_CreateWindow failed: " + std::string(SDL_GetError()));
    }
    _windowID = SDL_GetWindowID(_window);
    _context = SDL_GL_CreateContext(_window);
    if (!_context) {
        throw std::runtime_error("SDL_GL_CreateContext failed: " + std::string(SDL_GetError()));
    }
    SDL_GL_SetSwapInterval(_options.vsync ? 1 : 0);

    _inited = true;
}

void Window::deinit() {
    if (!_inited) {
        return;
    }
    SDL_GL_DeleteContext(_context);
    SDL_DestroyWindow(_window);
    _inited = false;
}

bool Window::isAssociatedWith(const SDL_Event &event) const {
    switch (event.type) {
    case SDL_WINDOWEVENT:
        return event.window.windowID == _windowID;
    case SDL_KEYDOWN:
        return event.key.windowID == _windowID;
    case SDL_KEYUP:
        return event.key.windowID == _windowID;
    case SDL_MOUSEMOTION:
        return event.motion.windowID == _windowID;
    case SDL_MOUSEBUTTONDOWN:
        return event.button.windowID == _windowID;
    case SDL_MOUSEBUTTONUP:
        return event.button.windowID == _windowID;
    case SDL_MOUSEWHEEL:
        return event.wheel.windowID == _windowID;
    default:
        return false;
    }
}

bool Window::handle(const SDL_Event &event) {
    switch (event.type) {
    case SDL_WINDOWEVENT:
        if (handleWindowEvent(event.window)) {
            return true;
        }
        break;
    case SDL_KEYDOWN:
        if (handleKeyDownEvent(event.key)) {
            return true;
        }
        break;
    default:
        break;
    }
    return false;
}

bool Window::handleWindowEvent(const SDL_WindowEvent &event) {
    switch (event.event) {
    case SDL_WINDOWEVENT_FOCUS_GAINED:
        _inFocus = true;
        return true;
    case SDL_WINDOWEVENT_FOCUS_LOST:
        _inFocus = false;
        return true;
    case SDL_WINDOWEVENT_CLOSE:
        _closeRequested = true;
        return true;
    default:
        break;
    }
    return false;
}

bool Window::handleKeyDownEvent(const SDL_KeyboardEvent &event) {
    switch (event.keysym.scancode) {
    case SDL_SCANCODE_C:
        if (event.keysym.mod & KMOD_CTRL) {
            _closeRequested = true;
            return true;
        }
        break;
    default:
        break;
    }
    return false;
}

void Window::swap() {
    SDL_GL_SwapWindow(_window);
}

} // namespace graphics

} // namespace reone
