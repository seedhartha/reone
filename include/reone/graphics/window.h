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
class EventHandler;

class IWindow {
public:
    virtual ~IWindow() = default;

    virtual bool isAssociatedWith(const SDL_Event &event) const = 0;
    virtual bool handle(const SDL_Event &event) = 0;

    virtual void swap() = 0;

    virtual bool isInFocus() const = 0;
    virtual bool isCloseRequested() const = 0;
};

class Window : public IWindow, boost::noncopyable {
public:
    Window(GraphicsOptions &options) :
        _options(options) {
    }

    ~Window() {
        deinit();
    }

    void init();
    void deinit();

    bool isAssociatedWith(const SDL_Event &event) const override;
    bool handle(const SDL_Event &event) override;

    void swap() override;

    bool isInFocus() const override {
        return _inFocus;
    }

    bool isCloseRequested() const override {
        return _closeRequested;
    }

private:
    GraphicsOptions &_options;

    bool _inited {false};

    SDL_Window *_window {nullptr};
    SDL_GLContext _context {nullptr};

    uint32_t _windowID {0};

    bool _inFocus {true};
    bool _closeRequested {false};

    bool handleWindowEvent(const SDL_WindowEvent &event);
    bool handleKeyDownEvent(const SDL_KeyboardEvent &event);
};

} // namespace graphics

} // namespace reone
