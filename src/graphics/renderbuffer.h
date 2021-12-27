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

#include "attachment.h"
#include "types.h"

namespace reone {

namespace graphics {

class Renderbuffer : public IAttachment, boost::noncopyable {
public:
    ~Renderbuffer() { deinit(); }

    void init();
    void deinit();

    void bind();
    void unbind();

    void configure(int width, int height, PixelFormat format);
    void refresh();

    bool isTexture() const override { return false; }
    bool isRenderbuffer() const override { return true; }

    // OpenGL

    uint32_t nameGL() const { return _nameGL; }

    // END OpenGL

private:
    bool _inited {false};

    int _width {0};
    int _height {0};
    PixelFormat _pixelFormat {PixelFormat::RGB};

    // OpenGL

    uint32_t _nameGL {0};

    // END OpenGL
};

} // namespace graphics

} // namespace reone
