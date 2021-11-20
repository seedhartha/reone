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

#include "../../../graphics/font.h"
#include "../../../graphics/texture/texture.h"

#include "../../core/contextaction.h"

namespace reone {

namespace game {

class Game;
class Services;
class SpatialObject;

class SelectionOverlay {
public:
    SelectionOverlay(
        Game &game,
        Services &services);

    void load();

    bool handle(const SDL_Event &event);
    void update();
    void draw();

private:
    struct ActionSlot {
        std::vector<ContextAction> actions;
        uint32_t indexSelected {0};
    };

    Game &_game;
    Services &_services;

    std::shared_ptr<graphics::Font> _font;
    std::shared_ptr<graphics::Texture> _friendlyReticle;
    std::shared_ptr<graphics::Texture> _friendlyReticle2;
    std::shared_ptr<graphics::Texture> _hostileReticle;
    std::shared_ptr<graphics::Texture> _hostileReticle2;
    std::shared_ptr<graphics::Texture> _friendlyScroll;
    std::shared_ptr<graphics::Texture> _hostileScroll;
    std::shared_ptr<graphics::Texture> _hilightedScroll;
    std::shared_ptr<SpatialObject> _hilightedObject;
    std::shared_ptr<SpatialObject> _selectedObject;
    std::vector<ActionSlot> _actionSlots;
    glm::vec3 _hilightedScreenCoords {0.0f};
    glm::vec3 _selectedScreenCoords {0.0f};
    int _reticleHeight {0};
    int _selectedActionSlot {-1};
    bool _hilightedHostile {false};
    bool _selectedHostile {false};
    bool _hasActions {false};

    bool handleMouseMotion(const SDL_MouseMotionEvent &event);
    bool handleMouseButtonDown(const SDL_MouseButtonEvent &event);
    bool handleMouseWheel(const SDL_MouseWheelEvent &event);

    void drawReticle(graphics::Texture &texture, const glm::vec3 &screenCoords);
    void drawTitleBar();
    void drawHealthBar();
    void drawActionBar();

    void drawActionFrame(int index);
    void drawActionIcon(int index);

    bool getActionScreenCoords(int index, float &x, float &y) const;
    glm::vec3 getColorFromSelectedObject() const;
};

} // namespace game

} // namespace reone
