/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "glm/vec2.hpp"
#include "glm/vec4.hpp"

#include "../framebuffer.h"
#include "../types.h"

namespace reone {

namespace render {

class ControlRenderPipeline {
public:
    ControlRenderPipeline(IRenderable *scene, const glm::ivec4 &extent);

    void init();
    void render(const glm::ivec2 &offset) const;

private:
    IRenderable *_scene { nullptr };
    glm::vec4 _extent { 0.0f };
    Framebuffer _geometry;

    ControlRenderPipeline(const ControlRenderPipeline &) = delete;
    ControlRenderPipeline &operator=(const ControlRenderPipeline &) = delete;
};

} // namespace render

} // namespace reone
