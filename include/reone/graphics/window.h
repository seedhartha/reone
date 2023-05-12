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

#pragma once

#include "options.h"
#include "types.h"

namespace reone {

namespace graphics {

class Cursor;
class IEventHandler;

class IWindow {
public:
    virtual ~IWindow() = default;
};

class Window : public IWindow, boost::noncopyable {
public:
    Window(GraphicsOptions &options) :
        _options(options) {
    }

    ~Window() { deinit(); }

    void init();
    void deinit();

    void processEvents(bool &quit);

    void swapBuffers() const;

    bool isInFocus() const { return _focus; }

    glm::mat4 getOrthoProjection(float near = 0.0f, float far = 100.0f) const;

    void setEventHandler(IEventHandler *eventHandler) { _eventHandler = eventHandler; }
    void setRelativeMouseMode(bool enabled);

    uint32_t mouseState(int *x, int *y);
    void showCursor(bool show);

private:
    GraphicsOptions &_options;

    IEventHandler *_eventHandler {nullptr};
    bool _inited {false};
    SDL_Window *_window {nullptr};
    SDL_GLContext _context {nullptr};
    bool _relativeMouseMode {false};
    bool _focus {true};

    bool handleEvent(const SDL_Event &event, bool &quit);
    bool handleKeyDownEvent(const SDL_KeyboardEvent &event, bool &quit);
    bool handleWindowEvent(const SDL_WindowEvent &event);

    inline int getWindowFlags() const;
};

} // namespace graphics

} // namespace reone
