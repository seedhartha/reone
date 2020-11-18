/*
 * Copyright (c) 2020 The reone project contributors
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

#include "../../resource/resources.h"
#include "../../common/streamutil.h"

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

shared_ptr<CreatureBlueprint> Blueprints::getCreature(const string &resRef) {
    return get<CreatureBlueprint>(resRef, ResourceType::CreatureBlueprint, _creatureCache);
}

template <class T>
shared_ptr<T> Blueprints::get(const string &resRef, ResourceType type, unordered_map<string, shared_ptr<T>> &cache) {
    auto maybeBlueprint = cache.find(resRef);
    if (maybeBlueprint != cache.end()) {
        return maybeBlueprint->second;
    }
    auto inserted = cache.insert(make_pair(resRef, doGet<T>(resRef, type)));

    return inserted.first->second;
}

template <class T>
shared_ptr<T> Blueprints::doGet(const string &resRef, ResourceType type) {
    shared_ptr<GffStruct> data(Resources::instance().getGFF(resRef, type));
    shared_ptr<T> blueprint;

    if (data) {
        blueprint.reset(new T(resRef));
        blueprint->load(*data);
    }

    return move(blueprint);
}

shared_ptr<DoorBlueprint> Blueprints::getDoor(const string &resRef) {
    return get<DoorBlueprint>(resRef, ResourceType::DoorBlueprint, _doorCache);
}

shared_ptr<ItemBlueprint> Blueprints::getItem(const string &resRef) {
    return get<ItemBlueprint>(resRef, ResourceType::ItemBlueprint, _itemCache);
}

shared_ptr<PlaceableBlueprint> Blueprints::getPlaceable(const string &resRef) {
    return get<PlaceableBlueprint>(resRef, ResourceType::PlaceableBlueprint, _placeableCache);
}

shared_ptr<SoundBlueprint> Blueprints::getSound(const string &resRef) {
    return get<SoundBlueprint>(resRef, ResourceType::SoundBlueprint, _soundCache);
}

shared_ptr<TriggerBlueprint> Blueprints::getTrigger(const string &resRef) {
    return get<TriggerBlueprint>(resRef, ResourceType::TriggerBlueprint, _triggerCache);
}

shared_ptr<WaypointBlueprint> Blueprints::getWaypoint(const string &resRef) {
    return get<WaypointBlueprint>(resRef, ResourceType::WaypointBlueprint, _waypointCache);
}

} // namespace game

} // namespace reone
