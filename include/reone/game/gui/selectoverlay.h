/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/graphics/font.h"
#include "reone/graphics/texture.h"
#include "reone/input/event.h"

#include "../contextaction.h"

namespace reone {

namespace game {

struct ServicesView;

class Game;
class Object;

class SelectionOverlay {
public:
    SelectionOverlay(
        Game &game,
        ServicesView &services);

    void init();

    bool handle(const input::Event &event);
    void update();
    void render();

private:
    struct ActionSlot {
        std::vector<ContextAction> actions;
        uint32_t indexSelected {0};
    };

    Game &_game;
    ServicesView &_services;

    std::shared_ptr<graphics::Font> _font;
    std::shared_ptr<graphics::Texture> _friendlyReticle;
    std::shared_ptr<graphics::Texture> _friendlyReticle2;
    std::shared_ptr<graphics::Texture> _hostileReticle;
    std::shared_ptr<graphics::Texture> _hostileReticle2;
    std::shared_ptr<graphics::Texture> _friendlyScroll;
    std::shared_ptr<graphics::Texture> _hostileScroll;
    std::shared_ptr<graphics::Texture> _hilightedScroll;
    std::shared_ptr<Object> _hilightedObject;
    std::shared_ptr<Object> _selectedObject;
    std::vector<ActionSlot> _actionSlots;
    glm::vec3 _hilightedScreenCoords {0.0f};
    glm::vec3 _selectedScreenCoords {0.0f};
    int _reticleHeight {0};
    int _selectedActionSlot {-1};
    bool _hilightedHostile {false};
    bool _selectedHostile {false};
    bool _hasActions {false};

    bool handleMouseMotion(const input::MouseMotionEvent &event);
    bool handleMouseButtonDown(const input::MouseButtonEvent &event);
    bool handleMouseWheel(const input::MouseWheelEvent &event);

    void renderReticle(std::shared_ptr<graphics::Texture> texture, const glm::vec3 &screenCoords);
    void renderTitleBar();
    void renderHealthBar();
    void renderActionBar();

    void renderActionFrame(int index);
    void renderActionIcon(int index);

    bool getActionScreenCoords(int index, float &x, float &y) const;
    glm::vec3 getColorFromSelectedObject() const;
};

} // namespace game

} // namespace reone
