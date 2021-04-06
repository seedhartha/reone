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

/** @file
 *  Area functions related to loading GIT files.
 */

#include "area.h"

#include "../../resource/resources.h"

#include "../game.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace game {

void Area::loadGIT(const GffStruct &git) {
    loadProperties(git);
    loadCreatures(git);
    loadDoors(git);
    loadPlaceables(git);
    loadWaypoints(git);
    loadTriggers(git);
    loadSounds(git);
    loadCameras(git);
}

void Area::loadProperties(const GffStruct &git) {
    shared_ptr<GffStruct> props(git.getStruct("AreaProperties"));
    int musicIdx = props->getInt("MusicDay");
    if (musicIdx) {
        shared_ptr<TwoDA> musicTable(Resources::instance().get2DA("ambientmusic"));
        _music = musicTable->getString(musicIdx, "resource");
    }
}

void Area::loadCreatures(const GffStruct &git) {
    for (auto &gffs : git.getList("Creature List")) {
        shared_ptr<Creature> creature(_game->objectFactory().newCreature());
        creature->loadFromGIT(*gffs);
        landObject(*creature);
        add(creature);
    }
}

void Area::loadDoors(const GffStruct &git) {
    for (auto &gffs : git.getList("Door List")) {
        shared_ptr<Door> door(_game->objectFactory().newDoor());
        door->loadFromGIT(*gffs);
        add(door);
    }
}

void Area::loadPlaceables(const GffStruct &git) {
    for (auto &gffs : git.getList("Placeable List")) {
        shared_ptr<Placeable> placeable(_game->objectFactory().newPlaceable());
        placeable->loadFromGIT(*gffs);
        add(placeable);
    }
}

void Area::loadWaypoints(const GffStruct &git) {
    for (auto &gffs : git.getList("WaypointList")) {
        shared_ptr<Waypoint> waypoint(_game->objectFactory().newWaypoint());
        waypoint->loadFromGIT(*gffs);
        add(waypoint);
    }
}

void Area::loadTriggers(const GffStruct &git) {
    for (auto &gffs : git.getList("TriggerList")) {
        shared_ptr<Trigger> trigger(_game->objectFactory().newTrigger());
        trigger->loadFromGIT(*gffs);
        add(trigger);
    }
}

void Area::loadSounds(const GffStruct &git) {
    for (auto &gffs : git.getList("SoundList")) {
        shared_ptr<Sound> sound(_game->objectFactory().newSound());
        sound->loadFromGIT(*gffs);
        add(sound);
    }
}

void Area::loadCameras(const GffStruct &git) {
    for (auto &gffs : git.getList("CameraList")) {
        shared_ptr<PlaceableCamera> camera(_game->objectFactory().newCamera());
        camera->loadFromGIT(*gffs);
        add(camera);
    }
}

} // namespace game

} // namespace reone
