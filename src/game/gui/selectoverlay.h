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

class Game;

class SelectionOverlay {
public:
    SelectionOverlay(Game *game);

    void load();

    void update();
    void render() const;

private:
    Game *_game { nullptr };
    std::shared_ptr<render::Texture> _friendlyReticle;
    std::shared_ptr<render::Texture> _friendlyReticle2;
    bool _hasHilighted { false };
    bool _hasSelected { false };
    glm::vec3 _hilightedScreenCoords { 0.0f };
    glm::vec3 _selectedScreenCoords { 0.0f };

    void drawReticle(render::Texture &texture, const glm::vec3 &screenCoords) const;
};

} // namespace game

} // namespace reone
