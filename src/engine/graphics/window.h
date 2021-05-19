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

#pragma once

#include <functional>
#include <memory>

#include <boost/noncopyable.hpp>

#include "glm/mat4x4.hpp"

#include "cursor.h"
#include "texture/texture.h"
#include "types.h"

namespace reone {

namespace graphics {

class Window : boost::noncopyable {
public:
    Window(GraphicsOptions options, IEventHandler *eventHandler = nullptr);
    ~Window();

    void init();
    void deinit();

    void processEvents(bool &quit);

    void show();
    void clear() const;
    void drawCursor() const;
    void swapBuffers() const;

    bool isInFocus() const { return _focus; }

    glm::mat4 getOrthoProjection(float near = 0.0f, float far = 100.0f) const;

    void setRelativeMouseMode(bool enabled);
    void setCursor(const std::shared_ptr<Cursor> &cursor);

private:
    bool _inited { false };
    GraphicsOptions _options;
    IEventHandler *_eventHandler { nullptr };
    SDL_Window *_window { nullptr };
    SDL_GLContext _context { nullptr };
    bool _relativeMouseMode { false };
    std::shared_ptr<Cursor> _cursor;
    bool _focus { true };

    void initSDL();
    void initGL();

    bool handleEvent(const SDL_Event &event, bool &quit);
    bool handleKeyDownEvent(const SDL_KeyboardEvent &event, bool &quit);
    bool handleWindowEvent(const SDL_WindowEvent &event);

    inline int getWindowFlags() const;
};

} // namespace graphics

} // namespace reone
