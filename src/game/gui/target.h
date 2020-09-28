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

#include <memory>

#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

#include "../../render/texture.h"
#include "../../render/types.h"

#include "../types.h"

namespace reone {

namespace game {

class TargetOverlay {
public:
    TargetOverlay(const render::GraphicsOptions &opts);

    void load();
    void render() const;

    void setContext(const TargetContext &ctx);

private:
    render::GraphicsOptions _opts;
    std::shared_ptr<render::Texture> _friendlyReticle;
    std::shared_ptr<render::Texture> _friendlyReticle2;
    TargetContext _context;

    void drawReticle(render::Texture &texture, const glm::vec3 &screenCoords) const;
};

} // namespace game

} // namespace reone
