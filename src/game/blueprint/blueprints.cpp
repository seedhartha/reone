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

#include "blueprints.h"

#include "../../core/log.h"
#include "../../resources/resources.h"

using namespace std;

using namespace reone::resources;

namespace reone {

namespace game {

static map<string, shared_ptr<CreatureBlueprint>> g_creatureCache;
static map<string, shared_ptr<DoorBlueprint>> g_doorCache;
static map<string, shared_ptr<ItemBlueprint>> g_itemCache;
static map<string, shared_ptr<PlaceableBlueprint>> g_placeableCache;
static map<string, shared_ptr<TriggerBlueprint>> g_triggerCache;
static map<string, shared_ptr<WaypointBlueprint>> g_waypointCache;

BlueprintManager &BlueprintManager::instance() {
    static BlueprintManager instance;
    return instance;
}

void BlueprintManager::clearCache() {
    g_creatureCache.clear();
    g_doorCache.clear();
    g_itemCache.clear();
    g_placeableCache.clear();
    g_triggerCache.clear();
    g_waypointCache.clear();
}

shared_ptr<CreatureBlueprint> BlueprintManager::findCreature(const string &resRef) {
    return find<CreatureBlueprint>(resRef, ResourceType::CreatureBlueprint, g_creatureCache);
}

shared_ptr<DoorBlueprint> BlueprintManager::findDoor(const string &resRef) {
    return find<DoorBlueprint>(resRef, ResourceType::DoorBlueprint, g_doorCache);
}

shared_ptr<ItemBlueprint> BlueprintManager::findItem(const string &resRef) {
    return find<ItemBlueprint>(resRef, ResourceType::ItemBlueprint, g_itemCache);
}

shared_ptr<PlaceableBlueprint> BlueprintManager::findPlaceable(const string &resRef) {
    return find<PlaceableBlueprint>(resRef, ResourceType::PlaceableBlueprint, g_placeableCache);
}

shared_ptr<TriggerBlueprint> BlueprintManager::findTrigger(const string &resRef) {
    return find<TriggerBlueprint>(resRef, ResourceType::TriggerBlueprint, g_triggerCache);
}

shared_ptr<WaypointBlueprint> BlueprintManager::findWaypoint(const string &resRef) {
    return find<WaypointBlueprint>(resRef, ResourceType::WaypointBlueprint, g_waypointCache);
}

template <class T>
shared_ptr<T> BlueprintManager::find(const string &resRef, ResourceType type, map<string, shared_ptr<T>> &cache) {
    auto it = cache.find(resRef);
    if (it != cache.end()) {
        return it->second;
    }
    shared_ptr<GffStruct> gff(Resources.findGFF(resRef, type));
    shared_ptr<T> blueprint;

    if (gff) {
        blueprint.reset(new T());
        blueprint->load(resRef, *gff);
    } else {
        warn(boost::format("Blueprints: not found: %s %d") % resRef % static_cast<int>(type));
    }
    auto pair = cache.insert(make_pair(resRef, blueprint));

    return pair.first->second;
}

} // namespace game

} // namespace reone
