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

#include "blueprints.h"

#include "../../common/streamutil.h"
#include "../../resource/resources.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace game {

Blueprints &Blueprints::instance() {
    static Blueprints instance;
    return instance;
}

void Blueprints::invalidateCache() {
    _creatureCache.clear();
    _doorCache.clear();
    _itemCache.clear();
    _placeableCache.clear();
    _soundCache.clear();
    _triggerCache.clear();
    _waypointCache.clear();
}

template <class T>
static shared_ptr<T> newBlueprint(const string &resRef, ResourceType type) {
    shared_ptr<GffStruct> gffs(Resources::instance().getGFF(resRef, type));
    if (!gffs) return nullptr;

    return make_shared<T>(resRef, gffs);
}

template <class T>
static shared_ptr<T> getBlueprint(const string &resRef, ResourceType type, unordered_map<string, shared_ptr<T>> &cache) {
    auto maybeBlueprint = cache.find(resRef);
    if (maybeBlueprint != cache.end()) {
        return maybeBlueprint->second;
    }
    auto inserted = cache.insert(make_pair(resRef, newBlueprint<T>(resRef, type)));

    return inserted.first->second;
}

shared_ptr<CreatureBlueprint> Blueprints::getCreature(const string &resRef) {
    return getBlueprint<CreatureBlueprint>(resRef, ResourceType::Utc, _creatureCache);
}

shared_ptr<DoorBlueprint> Blueprints::getDoor(const string &resRef) {
    return getBlueprint<DoorBlueprint>(resRef, ResourceType::Utd, _doorCache);
}

shared_ptr<ItemBlueprint> Blueprints::getItem(const string &resRef) {
    return getBlueprint<ItemBlueprint>(resRef, ResourceType::Uti, _itemCache);
}

shared_ptr<PlaceableBlueprint> Blueprints::getPlaceable(const string &resRef) {
    return getBlueprint<PlaceableBlueprint>(resRef, ResourceType::Utp, _placeableCache);
}

shared_ptr<SoundBlueprint> Blueprints::getSound(const string &resRef) {
    return getBlueprint<SoundBlueprint>(resRef, ResourceType::Uts, _soundCache);
}

shared_ptr<TriggerBlueprint> Blueprints::getTrigger(const string &resRef) {
    return getBlueprint<TriggerBlueprint>(resRef, ResourceType::Utt, _triggerCache);
}

shared_ptr<WaypointBlueprint> Blueprints::getWaypoint(const string &resRef) {
    return getBlueprint<WaypointBlueprint>(resRef, ResourceType::Utw, _waypointCache);
}

} // namespace game

} // namespace reone
