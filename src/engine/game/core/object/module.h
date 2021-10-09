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

#include "../../../graphics/types.h"
#include "../../../resource/format/gffreader.h"
#include "../../../resource/types.h"

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
    glm::vec3 entryPosition {0.0f};
    float entryFacing {0.0f};
    int dawnHour {0};
    int duskHour {0};
    int minPerHour {0};
};

class Door;
class Game;
class ObjectFactory;
class Placeable;

class Module : public Object {
public:
    struct Time {
        int day {0};
        int hour {0};
        int minute {0};
        int second {0};
        int millisecond {0};
    };

    Module(
        uint32_t id,
        Game *game,
        ActionFactory &actionFactory,
        Classes &classes,
        Combat &combat,
        FootstepSounds &footstepSounds,
        ObjectFactory &objectFactory,
        Party &party,
        Portraits &portraits,
        Reputes &reputes,
        ScriptRunner &scriptRunner,
        SoundSets &soundSets,
        Surfaces &surfaces,
        audio::AudioFiles &audioFiles,
        audio::AudioPlayer &audioPlayer,
        graphics::Context &context,
        graphics::Meshes &meshes,
        graphics::Models &models,
        graphics::Shaders &shaders,
        graphics::Textures &textures,
        graphics::Walkmeshes &walkmeshes,
        resource::Resources &resources,
        resource::Strings &strings,
        scene::SceneGraph &sceneGraph) :
        Object(
            id,
            ObjectType::Module,
            game,
            actionFactory,
            classes,
            combat,
            footstepSounds,
            objectFactory,
            party,
            portraits,
            reputes,
            scriptRunner,
            soundSets,
            surfaces,
            audioFiles,
            audioPlayer,
            context,
            meshes,
            models,
            shaders,
            textures,
            walkmeshes,
            resources,
            strings,
            sceneGraph) {
    }

    void load(std::string name, const resource::GffStruct &ifo, bool fromSave = false);
    void loadParty(const std::string &entry = "", bool fromSave = false);

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
    void loadArea(const resource::GffStruct &ifo, bool fromSave = false);
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
