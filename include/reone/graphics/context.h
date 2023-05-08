/*
 * Copyright (c) 2020-2022 The reone project contributors
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
    GraphicsContext(GraphicsOptions &options) :
        _options(options) {
    }

    void init();

    void clearColor(glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
    void clearDepth();
    void clearColorDepth(glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

    void withDepthTest(DepthTestMode mode, const std::function<void()> &block);
    void withFaceCulling(CullFaceMode mode, const std::function<void()> &block);
    void withBlending(BlendMode mode, const std::function<void()> &block);
    void withPolygonMode(PolygonMode mode, const std::function<void()> &block);
    void withViewport(glm::ivec4 viewport, const std::function<void()> &block);
    void withScissorTest(const glm::ivec4 &bounds, const std::function<void()> &block);

private:
    GraphicsOptions &_options;

    bool _inited {false};

    // States

    std::stack<DepthTestMode> _depthTestModes;
    std::stack<CullFaceMode> _cullFaceModes;
    std::stack<BlendMode> _blendModes;
    std::stack<PolygonMode> _polygonModes;
    std::stack<glm::ivec4> _viewports;

    // END States

    void setDepthTestMode(DepthTestMode mode);
    void setCullFaceMode(CullFaceMode mode);
    void setBlendMode(BlendMode mode);
    void setPolygonMode(PolygonMode mode);
    void setViewport(glm::ivec4 viewport);
};

} // namespace graphics

} // namespace reone
