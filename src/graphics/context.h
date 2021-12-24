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

#include "options.h"
#include "types.h"

namespace reone {

namespace graphics {

class GraphicsContext : boost::noncopyable {
public:
    GraphicsContext(GraphicsOptions options) :
        _options(std::move(options)) {
    }

    void init();

    void withFaceCulling(CullFaceMode mode, const std::function<void()> &block);
    void withBlending(BlendMode mode, const std::function<void()> &block);
    void withViewport(glm::ivec4 viewport, const std::function<void()> &block);
    void withScissorTest(const glm::ivec4 &bounds, const std::function<void()> &block);

    void withoutDepthTest(const std::function<void()> &block);

private:
    GraphicsOptions _options;

    bool _inited {false};

    bool _depthTest {true};
    CullFaceMode _cullFaceMode {CullFaceMode::None};
    BlendMode _blendMode {BlendMode::None};

    glm::ivec4 _viewport {0};

    void setCullFaceMode(CullFaceMode mode);
    void setBlendMode(BlendMode mode);
};

} // namespace graphics

} // namespace reone
