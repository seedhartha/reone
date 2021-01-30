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

#include <string>
#include <memory>
#include <unordered_map>

#include "../../resource/types.h"

#include "creature.h"
#include "door.h"
#include "item.h"
#include "placeable.h"
#include "sound.h"
#include "trigger.h"
#include "waypoint.h"

namespace reone {

namespace game {

class Blueprints {
public:
    static Blueprints &instance();

    void invalidateCache();

    std::shared_ptr<CreatureBlueprint> getCreature(const std::string &resRef);
    std::shared_ptr<DoorBlueprint> getDoor(const std::string &resRef);
    std::shared_ptr<ItemBlueprint> getItem(const std::string &resRef);
    std::shared_ptr<PlaceableBlueprint> getPlaceable(const std::string &resRef);
    std::shared_ptr<SoundBlueprint> getSound(const std::string &resRef);
    std::shared_ptr<TriggerBlueprint> getTrigger(const std::string &resRef);
    std::shared_ptr<WaypointBlueprint> getWaypoint(const std::string &resRef);

private:
    std::unordered_map<std::string, std::shared_ptr<CreatureBlueprint>> _creatureCache;
    std::unordered_map<std::string, std::shared_ptr<DoorBlueprint>> _doorCache;
    std::unordered_map<std::string, std::shared_ptr<ItemBlueprint>> _itemCache;
    std::unordered_map<std::string, std::shared_ptr<PlaceableBlueprint>> _placeableCache;
    std::unordered_map<std::string, std::shared_ptr<SoundBlueprint>> _soundCache;
    std::unordered_map<std::string, std::shared_ptr<TriggerBlueprint>> _triggerCache;
    std::unordered_map<std::string, std::shared_ptr<WaypointBlueprint>> _waypointCache;

    Blueprints() = default;
    Blueprints(const Blueprints &) = delete;
    Blueprints &operator=(const Blueprints &) = delete;
};

} // namespace game

} // namespace reone
