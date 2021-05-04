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

/** @file
 * Wrapper functions to toggle OpenGL state.
 */

#include <functional>

#include "glm/vec4.hpp"

namespace reone {

namespace render {

void withWireframes(const std::function<void()> &block);
void withViewport(const glm::ivec4 &viewport, const std::function<void()> &block);
void withScissorTest(const glm::ivec4 &bounds, const std::function<void()> &block);
void withDepthTest(const std::function<void()> &block);
void withAdditiveBlending(const std::function<void()> &block);
void withBackFaceCulling(const std::function<void()> &block);

void setActiveTextureUnit(int n);

} // namespace render

} // namespace reone
