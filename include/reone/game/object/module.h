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

#include "reone/graphics/types.h"
#include "reone/resource/format/gffreader.h"
#include "reone/resource/types.h"

#include "../contextaction.h"
#include "../object.h"
#include "../player.h"

#include "area.h"

namespace reone {

namespace scene {

class SceneGraph;

}

namespace game {

struct ModuleInfo {
    std::string entryArea;
    glm::vec3 entryPosition {0.0f};
    float entryFacing {0.0f};
};

class Door;
class Placeable;

class Module : public Object {
public:
    Module(
        uint32_t id,
        Game &game,
        ServicesView &services) :
        Object(
            id,
            ObjectType::Module,
            "",
            game,
            services) {
    }

    void load(std::string name, const resource::Gff &ifo, bool fromSave = false);
    void loadParty(const std::string &entry = "", bool fromSave = false);

    bool handle(const SDL_Event &event);
    void update(float dt);

    std::vector<ContextAction> getContextActions(const std::shared_ptr<Object> &object) const;

    const std::string &name() const { return _name; }
    const ModuleInfo &info() const { return _info; }
    std::shared_ptr<Area> area() const { return _area; }
    Player &player() { return *_player; }

private:
    std::string _name;
    ModuleInfo _info;
    std::shared_ptr<Area> _area;
    std::unique_ptr<Player> _player;

    void onCreatureClick(const std::shared_ptr<Creature> &creature);
    void onDoorClick(const std::shared_ptr<Door> &door);
    void onObjectClick(const std::shared_ptr<Object> &object);
    void onPlaceableClick(const std::shared_ptr<Placeable> &placeable);

    void getEntryPoint(const std::string &waypoint, glm::vec3 &position, float &facing) const;

    // Loading

    void loadInfo(const resource::Gff &ifo);
    void loadArea(const resource::Gff &ifo, bool fromSave = false);
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
