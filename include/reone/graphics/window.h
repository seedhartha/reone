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

    virtual void processEvents(bool &quit) = 0;

    virtual void swapBuffers() const = 0;

    virtual bool isInFocus() const = 0;

    virtual glm::mat4 getOrthoProjection(float near = 0.0f, float far = 100.0f) const = 0;

    virtual void setEventHandler(IEventHandler *eventHandler) = 0;
    virtual void setRelativeMouseMode(bool enabled) = 0;

    virtual uint32_t mouseState(int *x, int *y) = 0;
    virtual void showCursor(bool show) = 0;
};

class Window : public IWindow, boost::noncopyable {
public:
    Window(GraphicsOptions &options) :
        _options(options) {
    }

    ~Window() { deinit(); }

    void init();
    void deinit();

    void processEvents(bool &quit) override;

    void swapBuffers() const override;

    bool isInFocus() const override { return _focus; }

    glm::mat4 getOrthoProjection(float near = 0.0f, float far = 100.0f) const override;

    void setEventHandler(IEventHandler *eventHandler) override { _eventHandler = eventHandler; }
    void setRelativeMouseMode(bool enabled) override;

    uint32_t mouseState(int *x, int *y) override;
    void showCursor(bool show) override;

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
