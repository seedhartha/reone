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

#include "glm/vec3.hpp"

#include "SDL2/SDL_events.h"

#include "../../graphics/types.h"
#include "../../resource/format/gffreader.h"
#include "../../resource/types.h"

#include "../contextaction.h"
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
    float entryFacing { 0.0f };
    int dawnHour { 0 };
    int duskHour { 0 };
    int minPerHour { 0 };
};

class Door;
class Game;
class ObjectFactory;
class Placeable;

class Module : public Object {
public:
    struct Time {
        int day { 0 };
        int hour { 0 };
        int minute { 0 };
        int second { 0 };
        int millisecond { 0 };
    };

    Module(uint32_t id, Game *game);

    void load(const std::string &name, const resource::GffStruct &ifo);
    void loadParty(const std::string &entry = "");

    bool handle(const SDL_Event &event);
    void update(float dt);

    std::vector<ContextAction> getContextActions(const std::shared_ptr<Object> &object) const;

    const std::string &name() const { return _name; }
    const ModuleInfo &info() const { return _info; }
    std::shared_ptr<Area> area() const { return _area; }
    Player &player() { return *_player; }
    const Time &time() const { return _time; }

    void setTime(int hour, int minute, int second, int millisecond);

private:
    std::string _name;
    ModuleInfo _info;
    std::shared_ptr<Area> _area;
    std::unique_ptr<Player> _player;
    Time _time;

    void onCreatureClick(const std::shared_ptr<Creature> &creature);
    void onDoorClick(const std::shared_ptr<Door> &door);
    void onObjectClick(const std::shared_ptr<SpatialObject> &object);
    void onPlaceableClick(const std::shared_ptr<Placeable> &placeable);

    void getEntryPoint(const std::string &waypoint, glm::vec3 &position, float &facing) const;

    // Loading

    void loadInfo(const resource::GffStruct &ifo);
    void loadArea(const resource::GffStruct &ifo);
    void loadPlayer();

    // END Loading

    // User input

    bool handleMouseMotion(const SDL_MouseMotionEvent &event);
    bool handleMouseButtonDown(const SDL_MouseButtonEvent &event);
    bool handleKeyDown(const SDL_KeyboardEvent &event);

    // END User input
};

} // namespace game

} // namespace reone
