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
#include <functional>

#include <boost/noncopyable.hpp>

#include "glm/vec4.hpp"

#include "types.h"

namespace reone {

namespace graphics {

class Context : boost::noncopyable {
public:
    void init();

    void unbindFramebuffer();
    void unbindRenderbuffer();

    bool isDepthTestEnabled() const { return _depthTest; }

    const glm::ivec4 &viewport() const { return _viewport; }
    PolygonMode polygonMode() const { return _polygonMode; }
    BlendMode blendMode() const { return _blendMode; }

    void setViewport(glm::ivec4 viewport);
    void setDepthTestEnabled(bool enabled);
    void setBackFaceCullingEnabled(bool enabled);
    void setPolygonMode(PolygonMode mode);
    void setActiveTextureUnit(int n);
    void setBlendMode(BlendMode mode);

    void withScissorTest(const glm::ivec4 &bounds, const std::function<void()> &block);

private:
    glm::ivec4 _viewport { 0 };
    bool _depthTest { false };
    bool _backFaceCulling { false };
    int _textureUnit { 0 };
    PolygonMode _polygonMode { PolygonMode::Fill };
    BlendMode _blendMode { BlendMode::None };
};

} // namespace graphics

} // namespace reone
