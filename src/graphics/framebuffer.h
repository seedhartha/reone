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

#include "types.h"

namespace reone {

namespace graphics {

class Renderbuffer;
class Texture;

class Framebuffer : boost::noncopyable {
public:
    ~Framebuffer() { deinit(); }

    void init();
    void deinit();

    // Attachments

    void attachColor(const Texture &texture, int index = 0, int mip = 0) const;
    void attachColor(const Renderbuffer &renderbuffer, int index = 0) const;
    void attachDepth(const Texture &texture) const;
    void attachDepth(const Renderbuffer &renderbuffer) const;

    // END Attachments

    // OpenGL

    uint32_t nameGL() const { return _nameGL; }

    // END OpenGL

private:
    bool _inited {false};

    // OpenGL

    uint32_t _nameGL {0};

    // END OpenGL
};

} // namespace graphics

} // namespace reone
