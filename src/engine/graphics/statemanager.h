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

#include <boost/noncopyable.hpp>

#include "glm/vec4.hpp"

namespace reone {

namespace graphics {

class StateManager : boost::noncopyable {
public:
    static StateManager &instance();

    void withWireframes(const std::function<void()> &block);
    void withViewport(const glm::ivec4 &viewport, const std::function<void()> &block);
    void withScissorTest(const glm::ivec4 &bounds, const std::function<void()> &block);
    void withDepthTest(const std::function<void()> &block);
    void withAdditiveBlending(const std::function<void()> &block);
    void withBackFaceCulling(const std::function<void()> &block);

    void setActiveTextureUnit(int n);

private:
    int _textureUnit { 0 };
    bool _depthTestEnabled { false };
    uint32_t _polygonMode { 0 };

    void setDepthTestEnabled(bool enabled);
};


} // namespace graphics

} // namespace reone
