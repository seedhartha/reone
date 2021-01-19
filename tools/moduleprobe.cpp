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

#include "moduleprobe.h"

#include <stdexcept>

#include <boost/property_tree/json_parser.hpp>

#include "../src/common/pathutil.h"
#include "../src/common/streamutil.h"
#include "../src/resource/lytfile.h"

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

using namespace std;

using namespace reone::resource;

namespace reone {

namespace tools {

void ModuleProbe::probe(const string &name, const fs::path &gamePath, const fs::path &destPath) {
    _gamePath = gamePath;
    _keyBifProvider.init(_gamePath);
    loadResources(name);
    pt::ptree description(describeModule());
    writeDescription(name, description, destPath);
}

void ModuleProbe::loadResources(const string &moduleName) {
    fs::path tlkPath(getPathIgnoreCase(_gamePath, "dialog.tlk", false));
    if (!fs::exists(tlkPath)) {
        throw runtime_error("TLK file not found: " + tlkPath.string());
    }
    TlkFile tlk;
    tlk.load(tlkPath);
    _talkTable = tlk.table();

    fs::path modulesPath(getPathIgnoreCase(_gamePath, "modules", false));
    if (!fs::exists(modulesPath)) {
        throw runtime_error("Modules directory not found: " + modulesPath.string());
    }

    fs::path mainPath(getPathIgnoreCase(modulesPath, moduleName + ".rim", false));
    if (!fs::exists(mainPath)) {
        throw runtime_error("Main module RIM not found: " + mainPath.string());
    }
    _rimMain.load(mainPath);

    fs::path blueprintsPath(getPathIgnoreCase(modulesPath, moduleName + "_s.rim", false));
    if (!fs::exists(blueprintsPath)) {
        throw runtime_error("Module blueprints RIM not found: " + blueprintsPath.string());
    }
    _rimBlueprints.load(blueprintsPath);
}

static void putIfNotEmpty(const std::string &name, std::string value, pt::ptree &tree) {
    if (!value.empty()) {
        tree.put(name, move(value));
    }
}

pt::ptree ModuleProbe::describeModule() {
    pt::ptree description;

    GffFile ifo;
    ifo.load(wrap(_rimMain.find("module", ResourceType::Ifo)));
    auto ifoGffs = ifo.top();

    // Entry

    pt::ptree entry;
    entry.put("x", ifoGffs->getFloat("Mod_Entry_X"));
    entry.put("y", ifoGffs->getFloat("Mod_Entry_Y"));
    entry.put("z", ifoGffs->getFloat("Mod_Entry_Z"));
    entry.put("dir_x", ifoGffs->getFloat("Mod_Entry_Dir_X"));
    entry.put("dir_y", ifoGffs->getFloat("Mod_Entry_Dir_Y"));
    description.add_child("entry", entry);

    // END Entry

    // Scripts

    pt::ptree scripts;
    putIfNotEmpty("on_heartbeat", ifoGffs->getString("Mod_OnHeartbeat"), scripts);
    putIfNotEmpty("on_mod_load", ifoGffs->getString("Mod_OnModLoad"), scripts);
    putIfNotEmpty("on_client_entr", ifoGffs->getString("Mod_OnClientEntr"), scripts);
    putIfNotEmpty("on_client_leav", ifoGffs->getString("Mod_OnClientLeav"), scripts);
    putIfNotEmpty("on_actvt_item", ifoGffs->getString("Mod_OnActvtItem"), scripts);
    putIfNotEmpty("on_acquir_item", ifoGffs->getString("Mod_OnAcquirItem"), scripts);
    putIfNotEmpty("on_usr_defined", ifoGffs->getString("Mod_OnUsrDefined"), scripts);
    putIfNotEmpty("on_un_aqre_item", ifoGffs->getString("Mod_OnUnAqreItem"), scripts);
    putIfNotEmpty("on_plr_death", ifoGffs->getString("Mod_OnPlrDeath"), scripts);
    putIfNotEmpty("on_plr_dying", ifoGffs->getString("Mod_OnPlrDying"), scripts);
    putIfNotEmpty("on_plr_lvl_up", ifoGffs->getString("Mod_OnPlrLvlUp"), scripts);
    putIfNotEmpty("on_spawn_btn_dn", ifoGffs->getString("Mod_OnSpawnBtnDn"), scripts);
    putIfNotEmpty("on_plr_death", ifoGffs->getString("Mod_OnPlrDeath"), scripts);
    putIfNotEmpty("on_plr_rest", ifoGffs->getString("Mod_OnPlrRest"), scripts);
    if (!scripts.empty()) {
        description.add_child("scripts", scripts);
    }

    // END Scripts

    TwoDaFile appearance;
    appearance.load(wrap(getResource("appearance", ResourceType::TwoDa)));
    auto appearanceTable = appearance.table();

    TwoDaFile placeables;
    placeables.load(wrap(getResource("placeables", ResourceType::TwoDa)));
    auto placeablesTable = placeables.table();

    // Areas

    pt::ptree areas;
    for (auto &areaGffs : ifoGffs->getList("Mod_Area_list")) {
        string name(areaGffs->getString("Area_Name"));
        areas.push_back(make_pair("", describeArea(name, *appearanceTable, *placeablesTable)));
    }
    description.add_child("areas", areas);

    // END Areas

    return move(description);
}

pt::ptree ModuleProbe::describeArea(const string &name, const TwoDaTable &appearance, const resource::TwoDaTable &placeables) {
    GffFile are;
    are.load(wrap(_rimMain.find(name, ResourceType::Are)));
    auto areGffs = are.top();

    GffFile git;
    git.load(wrap(_rimMain.find(name, ResourceType::Git)));
    auto gitGffs = git.top();

    LytFile lyt;
    lyt.load(wrap(getResource(name, ResourceType::Lyt)));

    // Rooms

    pt::ptree rooms;
    for (auto &lytRoom : lyt.rooms()) {
        pt::ptree position;
        position.put("x", lytRoom.position.x);
        position.put("y", lytRoom.position.y);
        position.put("z", lytRoom.position.z);

        pt::ptree room;
        room.put("name", lytRoom.name);
        room.add_child("position", position);

        rooms.push_back(make_pair("", room));
    }

    // END Rooms

    // Creatures

    pt::ptree creatures;
    for (auto &gitCreature : gitGffs->getList("Creature List")) {
        creatures.push_back(make_pair("", describeCreature(*gitCreature, appearance)));
    }

    // END Creatures

    // Doors

    pt::ptree doors;
    for (auto &gitDoor : gitGffs->getList("Door List")) {
        doors.push_back(make_pair("", describeDoor(*gitDoor)));
    }

    // END Doors

    // Placeables

    pt::ptree placeablesTree;
    for (auto &gitPlaceable : gitGffs->getList("Placeable List")) {
        placeablesTree.push_back(make_pair("", describePlaceable(*gitPlaceable, placeables)));
    }

    // END Placeables

    // Triggers

    pt::ptree triggers;
    for (auto &gitTrigger : gitGffs->getList("TriggerList")) {
        triggers.push_back(make_pair("", describeTrigger(*gitTrigger)));
    }

    // END Triggers

    // Waypoints

    pt::ptree waypoints;
    for (auto &gitWaypoint : gitGffs->getList("WaypointList")) {
        waypoints.push_back(make_pair("", describeWaypoint(*gitWaypoint)));
    }

    // END Waypoints

    // Sounds

    pt::ptree sounds;
    for (auto &gitSound : gitGffs->getList("SoundList")) {
        sounds.push_back(make_pair("", describeSound(*gitSound)));
    }

    // END Sounds

    // Scripts

    pt::ptree scripts;
    putIfNotEmpty("on_enter", areGffs->getString("OnEnter"), scripts);
    putIfNotEmpty("on_exit", areGffs->getString("OnExit"), scripts);
    putIfNotEmpty("on_heartbeat", areGffs->getString("OnHeartbeat"), scripts);
    putIfNotEmpty("on_user_defined", areGffs->getString("OnUserDefined"), scripts);

    // END Scripts

    pt::ptree area;
    area.put("name", name);
    area.put("title", getString(areGffs->getInt("Name")));
    if (!scripts.empty()) {
        area.add_child("scripts", scripts);
    }
    if (!rooms.empty()) {
        area.add_child("rooms", rooms);
    }
    if (!creatures.empty()) {
        area.add_child("creatures", creatures);
    }
    if (!doors.empty()) {
        area.add_child("doors", doors);
    }
    if (!placeablesTree.empty()) {
        area.add_child("placeables", placeablesTree);
    }
    if (!triggers.empty()) {
        area.add_child("triggers", triggers);
    }
    if (!waypoints.empty()) {
        area.add_child("waypoints", waypoints);
    }
    if (!sounds.empty()) {
        area.add_child("sounds", sounds);
    }

    return move(area);
}

static string describeFaction(int id) {
    static unordered_map<int, string> descriptions {
        { 1, "Hostile1" },
        { 2, "Friendly1" },
        { 3, "Hostile2" },
        { 4, "Friendly2" },
        { 5, "Neutral" },
        { 6, "Insane" },
        { 7, "Tuskan" },
        { 8, "GlobalXor" },
        { 9, "Surrender1" },
        { 10, "Surrender2" },
        { 11, "Predator" },
        { 12, "Prey" },
        { 13, "Trap" },
        { 14, "EndarSpire" },
        { 15, "Rancor" },
        { 16, "Gizka1" },
        { 17, "Gizka2" },
        { 21, "SelfLoathing" },
        { 22, "OneOnOne" },
        { 23, "PartyPuppet" }
    };

    auto maybeDescription = descriptions.find(id);

    return maybeDescription != descriptions.end() ? maybeDescription->second : to_string(id);
}

pt::ptree ModuleProbe::describeCreature(const GffStruct &gitCreature, const TwoDaTable &appearance) {
    pt::ptree position;
    position.put("x", gitCreature.getFloat("XPosition"));
    position.put("y", gitCreature.getFloat("YPosition"));
    position.put("z", gitCreature.getFloat("ZPosition"));

    pt::ptree orientation;
    orientation.put("x", gitCreature.getFloat("XOrientation"));
    orientation.put("y", gitCreature.getFloat("YOrientation"));

    GffFile utc;
    utc.load(wrap(getResource(gitCreature.getString("TemplateResRef"), ResourceType::Utc)));
    auto utcGffs = utc.top();

    pt::ptree equipment;
    for (auto &utcItem : utcGffs->getList("Equip_ItemList")) {
        pt::ptree item;
        item.put("resref", utcItem->getString("EquippedRes"));
        equipment.push_back(make_pair("", item));
    }

    pt::ptree items;
    for (auto &utcItem : utcGffs->getList("ItemList")) {
        pt::ptree item;
        item.put("resref", utcItem->getString("InventoryRes"));
        item.put("dropable", utcItem->getBool("Dropable"));
        items.push_back(make_pair("", item));
    }

    pt::ptree scripts;
    putIfNotEmpty("heartbeat", utcGffs->getString("ScriptHeartbeat"), scripts);
    putIfNotEmpty("on_notice", utcGffs->getString("ScriptOnNotice"), scripts);
    putIfNotEmpty("spell_at", utcGffs->getString("ScriptSpellAt"), scripts);
    putIfNotEmpty("attacked", utcGffs->getString("ScriptAttacked"), scripts);
    putIfNotEmpty("damaged", utcGffs->getString("ScriptDamaged"), scripts);
    putIfNotEmpty("end_round", utcGffs->getString("ScriptEndRound"), scripts);
    putIfNotEmpty("end_dialogu", utcGffs->getString("ScriptEndDialogu"), scripts);
    putIfNotEmpty("dialogue", utcGffs->getString("ScriptDialogue"), scripts);
    putIfNotEmpty("spawn", utcGffs->getString("ScriptSpawn"), scripts);
    putIfNotEmpty("rested", utcGffs->getString("ScriptRested"), scripts);
    putIfNotEmpty("death", utcGffs->getString("ScriptDeath"), scripts);
    putIfNotEmpty("user_define", utcGffs->getString("ScriptUserDefine"), scripts);
    putIfNotEmpty("on_blocked", utcGffs->getString("ScriptOnBlocked"), scripts);

    pt::ptree blueprint;
    blueprint.put("resref", utcGffs->getString("TemplateResRef"));
    blueprint.put("tag", utcGffs->getString("Tag"));
    putIfNotEmpty("first_name", getString(utcGffs->getInt("FirstName")), blueprint);
    putIfNotEmpty("last_name", getString(utcGffs->getInt("LastName")), blueprint);
    blueprint.put("appearance", appearance.getString(utcGffs->getInt("Appearance_Type"), "label"));
    blueprint.put("faction", describeFaction(utcGffs->getInt("FactionID")));
    putIfNotEmpty("conversation", utcGffs->getString("Conversation"), blueprint);
    if (!equipment.empty()) {
        blueprint.add_child("equipment", equipment);
    }
    if (!items.empty()) {
        blueprint.add_child("items", items);
    }
    if (!scripts.empty()) {
        blueprint.add_child("scripts", scripts);
    }

    pt::ptree creature;
    creature.add_child("position", position);
    creature.add_child("orientation", orientation);
    creature.add_child("blueprint", blueprint);

    return move(creature);
}

pt::ptree ModuleProbe::describeDoor(const GffStruct &gitDoor) {
    GffFile utd;
    utd.load(wrap(getResource(gitDoor.getString("TemplateResRef"), ResourceType::Utd)));
    auto utdGffs = utd.top();

    pt::ptree scripts;
    putIfNotEmpty("on_closed", utdGffs->getString("OnClosed"), scripts);
    putIfNotEmpty("on_damaged", utdGffs->getString("OnDamaged"), scripts);
    putIfNotEmpty("on_death", utdGffs->getString("OnDeath"), scripts);
    putIfNotEmpty("on_disarm", utdGffs->getString("OnDisarm"), scripts);
    putIfNotEmpty("on_hearbeat", utdGffs->getString("OnHeartbeat"), scripts);
    putIfNotEmpty("on_lock", utdGffs->getString("OnLock"), scripts);
    putIfNotEmpty("on_melee_attacked", utdGffs->getString("OnMeleeAttacked"), scripts);
    putIfNotEmpty("on_open", utdGffs->getString("OnOpen"), scripts);
    putIfNotEmpty("on_spell_cast_at", utdGffs->getString("OnSpellCastAt"), scripts);
    putIfNotEmpty("on_trap_triggered", utdGffs->getString("OnTrapTriggered"), scripts);
    putIfNotEmpty("on_unlock", utdGffs->getString("OnUnlock"), scripts);
    putIfNotEmpty("on_user_defined", utdGffs->getString("OnUserDefined"), scripts);
    putIfNotEmpty("on_click", utdGffs->getString("OnClick"), scripts);
    putIfNotEmpty("on_fail_to_open", utdGffs->getString("OnFailToOpen"), scripts);

    pt::ptree blueprint;
    blueprint.put("resref", utdGffs->getString("TemplateResRef"));
    blueprint.put("loc_name", getString(utdGffs->getInt("LocName")));
    blueprint.put("static", utdGffs->getBool("Static"));
    putIfNotEmpty("conversation", utdGffs->getString("Conversation"), blueprint);
    blueprint.put("locked", utdGffs->getBool("Locked"));
    blueprint.put("key_required", utdGffs->getBool("KeyRequired"));
    if (!scripts.empty()) {
        blueprint.add_child("scripts", scripts);
    }

    pt::ptree door;
    door.put("x", gitDoor.getFloat("X"));
    door.put("y", gitDoor.getFloat("Y"));
    door.put("z", gitDoor.getFloat("Z"));
    door.put("bearing", gitDoor.getFloat("Bearing"));
    door.put("tag", gitDoor.getString("Tag"));
    putIfNotEmpty("linked_to_module", gitDoor.getString("LinkedToModule"), door);
    putIfNotEmpty("linked_to", gitDoor.getString("LinkedTo"), door);
    door.add_child("blueprint", blueprint);

    return move(door);
}

pt::ptree ModuleProbe::describePlaceable(const GffStruct &gitPlaceable, const TwoDaTable &placeables) {
    GffFile utp;
    utp.load(wrap(getResource(gitPlaceable.getString("TemplateResRef"), ResourceType::Utp)));
    auto utpGffs = utp.top();

    pt::ptree items;
    for (auto &utcItem : utpGffs->getList("ItemList")) {
        pt::ptree item;
        item.put("resref", utcItem->getString("InventoryRes"));
        items.push_back(make_pair("", item));
    }

    pt::ptree scripts;
    putIfNotEmpty("on_closed", utpGffs->getString("OnClosed"), scripts);
    putIfNotEmpty("on_damaged", utpGffs->getString("OnDamaged"), scripts);
    putIfNotEmpty("on_death", utpGffs->getString("OnDeath"), scripts);
    putIfNotEmpty("on_disarm", utpGffs->getString("OnDisarm"), scripts);
    putIfNotEmpty("on_heartbeat", utpGffs->getString("OnHeartbeat"), scripts);
    putIfNotEmpty("on_lock", utpGffs->getString("OnLock"), scripts);
    putIfNotEmpty("on_melee_attacked", utpGffs->getString("OnMeleeAttacked"), scripts);
    putIfNotEmpty("on_open", utpGffs->getString("OnOpen"), scripts);
    putIfNotEmpty("on_spell_cast_at", utpGffs->getString("OnSpellCastAt"), scripts);
    putIfNotEmpty("on_trap_triggered", utpGffs->getString("OnTrapTriggered"), scripts);
    putIfNotEmpty("on_unlock", utpGffs->getString("OnUnlock"), scripts);
    putIfNotEmpty("on_user_defined", utpGffs->getString("OnUserDefined"), scripts);
    putIfNotEmpty("on_end_dialogue", utpGffs->getString("OnEndDialogue"), scripts);
    putIfNotEmpty("on_inv_disturbed", utpGffs->getString("OnInvDisturbed"), scripts);
    putIfNotEmpty("on_used", utpGffs->getString("OnUsed"), scripts);

    pt::ptree blueprint;
    blueprint.put("resref", utpGffs->getString("TemplateResRef"));
    blueprint.put("tag", utpGffs->getString("Tag"));
    blueprint.put("loc_name", getString(utpGffs->getInt("LocName")));
    blueprint.put("appearance", placeables.getString(utpGffs->getInt("Appearance"), "label"));
    blueprint.put("static", utpGffs->getBool("Static"));
    putIfNotEmpty("conversation", utpGffs->getString("Conversation"), blueprint);
    blueprint.put("locked", utpGffs->getBool("Locked"));
    blueprint.put("key_required", utpGffs->getBool("KeyRequired"));
    if (!items.empty()) {
        blueprint.add_child("items", items);
    }
    if (!scripts.empty()) {
        blueprint.add_child("scripts", scripts);
    }

    pt::ptree placeable;
    placeable.put("x", gitPlaceable.getFloat("X"));
    placeable.put("y", gitPlaceable.getFloat("Y"));
    placeable.put("z", gitPlaceable.getFloat("Z"));
    placeable.put("bearing", gitPlaceable.getFloat("Bearing"));
    placeable.add_child("blueprint", blueprint);

    return move(placeable);
}

pt::ptree ModuleProbe::describeTrigger(const GffStruct &gitTrigger) {
    GffFile utt;
    utt.load(wrap(getResource(gitTrigger.getString("TemplateResRef"), ResourceType::Utt)));
    auto uttGffs = utt.top();

    pt::ptree position;
    position.put("x", gitTrigger.getFloat("XPosition"));
    position.put("y", gitTrigger.getFloat("YPosition"));
    position.put("z", gitTrigger.getFloat("ZPosition"));

    pt::ptree orientation;
    orientation.put("x", gitTrigger.getFloat("XOrientation"));
    orientation.put("y", gitTrigger.getFloat("YOrientation"));
    orientation.put("z", gitTrigger.getFloat("ZOrientation"));

    pt::ptree geometry;
    for (auto &gitPoint : gitTrigger.getList("Geometry")) {
        pt::ptree point;
        point.put("point_x", gitPoint->getFloat("PointX"));
        point.put("point_y", gitPoint->getFloat("PointY"));
        point.put("point_z", gitPoint->getFloat("PointZ"));
        geometry.push_back(make_pair("", point));
    }

    pt::ptree scripts;
    putIfNotEmpty("on_disarm", uttGffs->getString("OnDisarm"), scripts);
    putIfNotEmpty("on_trap_triggered", uttGffs->getString("OnTrapTriggered"), scripts);
    putIfNotEmpty("on_click", uttGffs->getString("OnClick"), scripts);
    putIfNotEmpty("heartbeat", uttGffs->getString("ScriptHeartbeat"), scripts);
    putIfNotEmpty("on_enter", uttGffs->getString("ScriptOnEnter"), scripts);
    putIfNotEmpty("on_exit", uttGffs->getString("ScriptOnExit"), scripts);
    putIfNotEmpty("user_define", uttGffs->getString("ScriptUserDefine"), scripts);

    pt::ptree blueprint;
    blueprint.put("resref", uttGffs->getString("TemplateResRef"));
    blueprint.put("tag", uttGffs->getString("Tag"));
    blueprint.put("localized_name", getString(uttGffs->getInt("LocalizedName")));
    if (!scripts.empty()) {
        blueprint.add_child("scripts", scripts);
    }

    pt::ptree trigger;
    trigger.add_child("position", position);
    trigger.add_child("orientation", orientation);
    trigger.add_child("geometry", geometry);
    trigger.add_child("blueprint", blueprint);

    return move(trigger);
}

pt::ptree ModuleProbe::describeWaypoint(const GffStruct &gitWaypoint) {
    pt::ptree position;
    position.put("x", gitWaypoint.getFloat("XPosition"));
    position.put("y", gitWaypoint.getFloat("YPosition"));
    position.put("z", gitWaypoint.getFloat("ZPosition"));

    pt::ptree orientation;
    orientation.put("x", gitWaypoint.getFloat("XOrientation"));
    orientation.put("y", gitWaypoint.getFloat("YOrientation"));

    pt::ptree waypoint;
    waypoint.put("blueprint_resref", gitWaypoint.getString("TemplateResRef"));
    waypoint.put("tag", gitWaypoint.getString("Tag"));
    waypoint.put("localized_name", getString(gitWaypoint.getInt("LocalizedName")));
    waypoint.add_child("position", position);
    waypoint.add_child("orientation", orientation);

    return move(waypoint);
}

pt::ptree ModuleProbe::describeSound(const GffStruct &gitSound) {
    GffFile uts;
    uts.load(wrap(getResource(gitSound.getString("TemplateResRef"), ResourceType::Uts)));
    auto utsGffs = uts.top();

    pt::ptree position;
    position.put("x", gitSound.getFloat("XPosition"));
    position.put("y", gitSound.getFloat("YPosition"));
    position.put("z", gitSound.getFloat("ZPosition"));

    TwoDaFile priorityGroups;
    priorityGroups.load(wrap(getResource("prioritygroups", ResourceType::TwoDa)));
    auto priorityGroupsTable = priorityGroups.table();

    pt::ptree sounds;
    for (auto &utsSound : utsGffs->getList("Sounds")) {
        pt::ptree sound;
        sound.put("resref", utsSound->getString("Sound"));
        sounds.push_back(make_pair("", sound));
    }

    pt::ptree blueprint;
    blueprint.put("resref", utsGffs->getString("TemplateResRef"));
    blueprint.put("tag", utsGffs->getString("Tag"));
    blueprint.put("loc_name", getString(utsGffs->getInt("LocName")));
    blueprint.put("active", utsGffs->getBool("Active"));
    blueprint.put("continous", utsGffs->getBool("Continous"));
    blueprint.put("looping", utsGffs->getBool("Looping"));
    blueprint.put("positional", utsGffs->getBool("Positional"));
    blueprint.put("priority", priorityGroupsTable->getString(utsGffs->getInt("Priority"), "label"));
    blueprint.put("interval", utsGffs->getInt("Interval"));
    if (!sounds.empty()) {
        blueprint.add_child("sounds", sounds);
    }

    pt::ptree sound;
    sound.add_child("position", position);
    sound.add_child("blueprint", blueprint);

    return move(sound);
}

void ModuleProbe::writeDescription(const string &moduleName, const pt::ptree &tree, const fs::path &destPath) {
    fs::path jsonPath(destPath);
    jsonPath.append(moduleName + ".json");

    fs::ofstream json(jsonPath);
    pt::write_json(json, tree);
}

shared_ptr<ByteArray> ModuleProbe::getResource(const string &resRef, ResourceType type) {
    shared_ptr<ByteArray> result;
    string cacheKey(resRef + "." + to_string(static_cast<int>(type)));

    auto maybeResource = _resourceCache.find(cacheKey);
    if (maybeResource != _resourceCache.end()) return maybeResource->second;

    result = _rimMain.find(resRef, type);

    if (!result) {
        result = _rimBlueprints.find(resRef, type);
    }
    if (!result) {
        result = _keyBifProvider.find(resRef, type);
    }
    if (!result) {
        throw runtime_error("Resource not found: " + resRef + " " + to_string(static_cast<int>(type)));
    }

    _resourceCache.insert(make_pair(cacheKey, result));

    return move(result);
}

string ModuleProbe::getString(int strRef) const {
    if (strRef == -1) return "";

    return _talkTable->getString(strRef).text;
}

} // namespace tools

} // namespace reone
