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

#include "glm/vec3.hpp"

#include "SDL2/SDL_events.h"

#include "../../render/types.h"
#include "../../resource/gfffile.h"
#include "../../resource/types.h"

#include "../player.h"

#include "area.h"
#include "object.h"

namespace reone {

namespace scene {

class SceneGraph;

}

namespace game {

struct ModuleInfo {
    std::string entryArea;
    glm::vec3 entryPosition { 0.0f };
    float entryHeading { 0.0f };
};

class Door;
class Game;
class ObjectFactory;
class Placeable;

class Module : public Object {
public:
    Module(uint32_t id, Game *game);

    void load(const std::string &name, const resource::GffStruct &ifo);
    void loadParty(const std::string &entry = "");

    bool handle(const SDL_Event &event);
    void update(float dt);

    const std::string &name() const;
    const ModuleInfo &info() const;
    std::shared_ptr<Area> area() const;
    Player &player();

private:
    Game *_game { nullptr };
    std::string _name;
    ModuleInfo _info;
    std::shared_ptr<Area> _area;
    std::unique_ptr<Player> _player;

    void onCreatureClick(Creature &creature);
    void onDoorClick(Door &door);
    void onObjectClick(SpatialObject &object);
    void onPlaceableClick(Placeable &placeable);

    void getEntryPoint(const std::string &waypoint, glm::vec3 &position, float &heading) const;

    // Loading

    void loadInfo(const resource::GffStruct &ifo);
    void loadArea(const resource::GffStruct &ifo);
    void loadPlayer();

    // END Loading

    // User input

    bool handleMouseMotion(const SDL_MouseMotionEvent &event);
    bool handleMouseButtonUp(const SDL_MouseButtonEvent &event);
    bool handleKeyUp(const SDL_KeyboardEvent &event);

    // END User input
};

} // namespace game

} // namespace reone
