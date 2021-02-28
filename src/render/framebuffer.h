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

#include <cstdint>

#include <boost/noncopyable.hpp>

#include "renderbuffer.h"
#include "texture.h"

namespace reone {

namespace render {

/**
 * Abstraction over the OpenGL framebuffer used for off-screen rendering.
 */
class Framebuffer : boost::noncopyable {
public:
    Framebuffer() = default;
    ~Framebuffer();

    void init();
    void deinit();

    void bind() const;
    void unbind() const;

    /**
     * Attaches a texture as a color buffer of this framebuffer. Framebuffer must be bound.
     *
     * @param texture texture to attach
     * @param index index of the color buffer
     * @param mip mip level of the texture
     */
    void attachColor(const Texture &texture, int index = 0, int mip = 0) const;

    void attachCubeMapFaceAsColor(const Texture &texture, CubeMapFace face, int index = 0, int mip = 0) const;

    /**
     * Attaches a texture as a depth buffer of this framebuffer. Framebuffer must be bound.
     */
    void attachDepth(const Texture &texture) const;

    void attachDepth(const Renderbuffer &renderbuffer) const;

    /**
     * Throws logic_error if this framebuffer is not complete. Framebuffer must be bound.
     */
    void checkCompleteness();

private:
    bool _inited { false };
    uint32_t _framebuffer { 0 };
};

} // namespace render

} // namespace reone
