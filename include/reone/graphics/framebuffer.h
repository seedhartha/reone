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

#include "attachment.h"
#include "types.h"

namespace reone {

namespace graphics {

class Renderbuffer;
class Texture;

class Framebuffer : boost::noncopyable {
public:
    enum class Attachment {
        Color,
        Depth,
        DepthStencil
    };

    ~Framebuffer() {
        deinit();
    }

    void init();
    void deinit();

    void attachDepth(std::shared_ptr<IAttachment> depth) {
        _depth = std::move(depth);
    }

    void attachColorDepth(std::shared_ptr<IAttachment> color, std::shared_ptr<IAttachment> depth) {
        _colors.clear();
        _colors.push_back(std::move(color));
        _depth = std::move(depth);
    }

    void attachColorsDepth(std::vector<std::shared_ptr<IAttachment>> colors, std::shared_ptr<IAttachment> depth) {
        _colors = std::move(colors);
        _depth = std::move(depth);
    }

    void attachTexture(const Texture &texture, Attachment attachment, int index = 0) const;
    void attachTextureLayer(const Texture &texture, int layer, int mip, Attachment attachment, int index = 0) const;
    void attachRenderbuffer(const Renderbuffer &renderbuffer, Attachment attachment, int index = 0) const;

    void checkCompleteness();

    // OpenGL

    uint32_t nameGL() const {
        return _nameGL;
    }

    // END OpenGL

private:
    bool _inited {false};

    std::vector<std::shared_ptr<IAttachment>> _colors;
    std::shared_ptr<IAttachment> _depth;
    std::shared_ptr<IAttachment> _depthStencil;

    void configure();

    // OpenGL

    uint32_t _nameGL {0};

    // END OpenGL
};

} // namespace graphics

} // namespace reone
