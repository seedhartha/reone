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

#include <memory>
#include <unordered_map>
#include <vector>

#include "SDL2/SDL_events.h"

#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

#include "../types.h"

namespace reone {

namespace render {

class Font;
class Texture;

}

namespace game {

class Game;
class SpatialObject;

class SelectionOverlay {
public:
    SelectionOverlay(Game *game);

    void load();

    bool handle(const SDL_Event &event);
    void update();
    void render();

private:
    Game *_game { nullptr };
    std::shared_ptr<render::Font> _font;
    std::shared_ptr<render::Texture> _friendlyReticle;
    std::shared_ptr<render::Texture> _friendlyReticle2;
    std::shared_ptr<render::Texture> _hostileReticle;
    std::shared_ptr<render::Texture> _hostileReticle2;
    std::shared_ptr<render::Texture> _friendlyScroll;
    std::shared_ptr<render::Texture> _hostileScroll;
    std::shared_ptr<render::Texture> _hilightedScroll;
    std::unordered_map<ContextualAction, std::shared_ptr<render::Texture>> _textureByAction;
    std::shared_ptr<SpatialObject> _hilightedObject;
    std::shared_ptr<SpatialObject> _selectedObject;
    std::vector<ContextualAction> _actions;
    glm::vec3 _hilightedScreenCoords { 0.0f };
    glm::vec3 _selectedScreenCoords { 0.0f };
    int _reticleHeight { 0 };
    int _selectedActionIdx { -1 };
    bool _hilightedHostile { false };
    bool _selectedHostile { false };

    void addTextureByAction(ContextualAction action, const std::string &resRef);

    bool handleMouseMotion(const SDL_MouseMotionEvent &event);
    bool handleMouseButtonDown(const SDL_MouseButtonEvent &event);

    void drawReticle(render::Texture &texture, const glm::vec3 &screenCoords);
    void drawTitleBar();
    void drawHealthBar();
    void drawActionBar();

    bool getActionScreenCoords(int index, float &x, float &y) const;
    glm::vec3 getColorFromSelectedObject() const;
};

} // namespace game

} // namespace reone
