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
    enum class Attachment {
        Color1,
        Color2,
        Depth,
        DepthStencil
    };

    ~Framebuffer() {
        deinit();
    }

    void attachDepth(std::shared_ptr<IAttachment> depth) {
        _depth = std::move(depth);
    }

    void attachDepthStencil(std::shared_ptr<IAttachment> depthStencil) {
        _depthStencil = std::move(depthStencil);
    }

    void attachColorDepth(std::shared_ptr<IAttachment> color, std::shared_ptr<IAttachment> depth) {
        _color1 = std::move(color);
        _depth = std::move(depth);
    }

    void attachColorDepthStencil(std::shared_ptr<IAttachment> color, std::shared_ptr<IAttachment> depthStencil) {
        _color1 = std::move(color);
        _depthStencil = std::move(depthStencil);
    }

    void attachColorsDepth(std::shared_ptr<IAttachment> color1, std::shared_ptr<IAttachment> color2, std::shared_ptr<IAttachment> depth) {
        _color1 = std::move(color1);
        _color2 = std::move(color2);
        _depth = std::move(depth);
    }

    void attachColorsDepthStencil(std::shared_ptr<IAttachment> color1, std::shared_ptr<IAttachment> color2, std::shared_ptr<IAttachment> depthStencil) {
        _color1 = std::move(color1);
        _color2 = std::move(color2);
        _depthStencil = std::move(depthStencil);
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
    std::shared_ptr<IAttachment> _depthStencil;

    void configure();

    void attachTexture(const Texture &texture, Attachment attachment) const;
    void attachRenderbuffer(const Renderbuffer &renderbuffer, Attachment attachment) const;

    // OpenGL

    uint32_t _nameGL {0};

    // END OpenGL
};

} // namespace graphics

} // namespace reone
