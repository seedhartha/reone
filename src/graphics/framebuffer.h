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

class Renderbuffer;
class Texture;

class Framebuffer : boost::noncopyable {
public:
    Framebuffer(std::shared_ptr<IAttachment> depth) :
        _depth(std::move(depth)) {
    }

    Framebuffer(std::shared_ptr<IAttachment> color, std::shared_ptr<IAttachment> depth) :
        _color1(std::move(color)),
        _depth(std::move(depth)) {
    }

    Framebuffer(std::shared_ptr<IAttachment> color1, std::shared_ptr<IAttachment> color2, std::shared_ptr<IAttachment> depth) :
        _color1(std::move(color1)),
        _color2(std::move(color2)),
        _depth(std::move(depth)) {
    }

    ~Framebuffer() {
        deinit();
    }

    void init();
    void deinit();

    // OpenGL

    uint32_t nameGL() const {
        return _nameGL;
    }

    // END OpenGL

private:
    bool _inited {false};

    std::shared_ptr<IAttachment> _color1;
    std::shared_ptr<IAttachment> _color2;
    std::shared_ptr<IAttachment> _depth;

    void configure();

    void attachColor(const Texture &texture, int index = 0) const;
    void attachColor(const Renderbuffer &renderbuffer, int index = 0) const;
    void attachDepth(const Texture &texture) const;
    void attachDepth(const Renderbuffer &renderbuffer) const;

    // OpenGL

    uint32_t _nameGL {0};

    // END OpenGL
};

} // namespace graphics

} // namespace reone
