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

#include "reone/game/neo/game.h"

#include "reone/game/neo/object/area.h"
#include "reone/game/neo/object/camera.h"
#include "reone/game/neo/object/creature.h"
#include "reone/game/neo/object/door.h"
#include "reone/game/neo/object/encounter.h"
#include "reone/game/neo/object/item.h"
#include "reone/game/neo/object/placeable.h"
#include "reone/game/neo/object/sound.h"
#include "reone/game/neo/object/store.h"
#include "reone/game/neo/object/trigger.h"
#include "reone/game/neo/object/waypoint.h"
#include "reone/resource/di/services.h"
#include "reone/resource/exception/notfound.h"
#include "reone/resource/provider/2das.h"
#include "reone/resource/provider/gffs.h"
#include "reone/resource/provider/layouts.h"
#include "reone/resource/provider/paths.h"
#include "reone/resource/provider/visibilities.h"
#include "reone/resource/template/generated/are.h"
#include "reone/resource/template/generated/git.h"
#include "reone/resource/template/generated/ifo.h"
#include "reone/resource/template/generated/utc.h"
#include "reone/resource/template/generated/utd.h"
#include "reone/resource/template/generated/ute.h"
#include "reone/resource/template/generated/utm.h"
#include "reone/resource/template/generated/utp.h"
#include "reone/resource/template/generated/uts.h"
#include "reone/resource/template/generated/utt.h"
#include "reone/resource/template/generated/utw.h"
#include "reone/system/exception/notimplemented.h"
#include "reone/system/exception/validation.h"

using namespace reone::resource;
using namespace reone::resource::generated;

namespace reone {

namespace game {

namespace neo {

void Game::startModule(const std::string &name) {
    _module = loadModule(name);
}

bool Game::handle(const input::Event &event) {
    return false;
}

void Game::update(float dt) {
    if (_module) {
        _module->get().update(dt);
    }
}

Module &Game::loadModule(const std::string &name) {
    auto ifo = _resourceSvc.gffs.get("module", ResType::Ifo);
    if (!ifo) {
        throw ResourceNotFoundException("Module IFO not found: " + name);
    }
    auto parsedIFO = parseIFO(*ifo);
    auto &module = newModule(parsedIFO.Mod_Tag);
    module.load(parsedIFO);
    return module;
}

Area &Game::loadArea(const std::string &name) {
    auto are = _resourceSvc.gffs.get(name, ResType::Are);
    if (!are) {
        throw ResourceNotFoundException("Area ARE not found: " + name);
    }
    auto parsedARE = parseARE(*are);
    auto git = _resourceSvc.gffs.get(name, ResType::Git);
    if (!git) {
        throw ResourceNotFoundException("Area GIT not found: " + name);
    }
    auto parsedGIT = parseGIT(*git);
    auto lyt = _resourceSvc.layouts.get(name);
    if (!lyt) {
        throw ResourceNotFoundException("Area LYT not found: " + name);
    }
    auto vis = _resourceSvc.visibilities.get(name);
    if (!vis) {
    }
    auto pth = _resourceSvc.paths.get(name);
    if (!pth) {
    }
    auto &area = newArea(parsedARE.Tag);
    area.load(parsedARE, parsedGIT, *lyt, *vis, *pth);
    return area;
}

Camera &Game::loadCamera() {
    auto &camera = newCamera("");
    return camera;
}

Creature &Game::loadCreature(const resource::ResRef &tmplt) {
    auto utc = _resourceSvc.gffs.get(tmplt.value(), ResType::Utc);
    if (!utc) {
        throw ResourceNotFoundException("Creature UTC not found: " + tmplt.value());
    }
    auto parsedUTC = parseUTC(*utc);
    auto &creature = newCreature(parsedUTC.Tag);
    auto appearance2da = _resourceSvc.twoDas.get("appearance");
    if (!appearance2da) {
        throw ResourceNotFoundException("Apperance 2DA not found");
    }
    auto heads2da = _resourceSvc.twoDas.get("heads");
    if (!heads2da) {
        throw ResourceNotFoundException("Heads 2DA not found");
    }
    creature.load(parsedUTC, *appearance2da, *heads2da);
    return creature;
}

Door &Game::loadDoor(const resource::ResRef &tmplt) {
    auto utd = _resourceSvc.gffs.get(tmplt.value(), ResType::Utd);
    if (!utd) {
        throw ResourceNotFoundException("Door UTD not found: " + tmplt.value());
    }
    auto parsedUTD = parseUTD(*utd);
    auto &door = newDoor(parsedUTD.Tag);
    return door;
}

Encounter &Game::loadEncounter(const resource::ResRef &tmplt) {
    auto ute = _resourceSvc.gffs.get(tmplt.value(), ResType::Ute);
    if (!ute) {
        throw ResourceNotFoundException("Encounter UTE not found: " + tmplt.value());
    }
    auto parsedUTE = parseUTE(*ute);
    auto &encounter = newEncounter(parsedUTE.Tag);
    return encounter;
}

Placeable &Game::loadPlaceable(const resource::ResRef &tmplt) {
    auto utp = _resourceSvc.gffs.get(tmplt.value(), ResType::Utp);
    if (!utp) {
        throw ResourceNotFoundException("Placeable UTP not found: " + tmplt.value());
    }
    auto parsedUTP = parseUTP(*utp);
    auto &placeable = newPlaceable(parsedUTP.Tag);
    return placeable;
}

Sound &Game::loadSound(const resource::ResRef &tmplt) {
    auto uts = _resourceSvc.gffs.get(tmplt.value(), ResType::Uts);
    if (!uts) {
        throw ResourceNotFoundException("Sound UTS not found: " + tmplt.value());
    }
    auto parsedUTS = parseUTS(*uts);
    auto &sound = newSound(parsedUTS.Tag);
    return sound;
}

Store &Game::loadStore(const resource::ResRef &tmplt) {
    auto utm = _resourceSvc.gffs.get(tmplt.value(), ResType::Utm);
    if (!utm) {
        throw ResourceNotFoundException("Store UTM not found: " + tmplt.value());
    }
    auto parsedUTM = parseUTM(*utm);
    auto &store = newStore(parsedUTM.Tag);
    return store;
}

Trigger &Game::loadTrigger(const resource::ResRef &tmplt) {
    auto utt = _resourceSvc.gffs.get(tmplt.value(), ResType::Utt);
    if (!utt) {
        throw ResourceNotFoundException("Trigger UTT not found: " + tmplt.value());
    }
    auto parsedUTT = parseUTT(*utt);
    auto &trigger = newTrigger(parsedUTT.Tag);
    return trigger;
}

Waypoint &Game::loadWaypoint(const resource::ResRef &tmplt) {
    auto utw = _resourceSvc.gffs.get(tmplt.value(), ResType::Utw);
    if (!utw) {
        throw ResourceNotFoundException("Waypoint UTW not found: " + tmplt.value());
    }
    auto parsedUTW = parseUTW(*utw);
    auto &waypoint = newWaypoint(parsedUTW.Tag);
    return waypoint;
}

Area &Game::newArea(ObjectTag tag) {
    auto object = std::make_unique<Area>(_nextObjectId++, std::move(tag), *this);
    auto &area = *object;
    _objects.push_back(std::move(object));
    return area;
}

Camera &Game::newCamera(ObjectTag tag) {
    auto object = std::make_unique<Camera>(_nextObjectId++, std::move(tag));
    auto &camera = *object;
    _objects.push_back(std::move(object));
    return camera;
}

Creature &Game::newCreature(ObjectTag tag) {
    auto object = std::make_unique<Creature>(_nextObjectId++, std::move(tag));
    auto &creature = *object;
    _objects.push_back(std::move(object));
    return creature;
}

Door &Game::newDoor(ObjectTag tag) {
    auto object = std::make_unique<Door>(_nextObjectId++, std::move(tag));
    auto &door = *object;
    _objects.push_back(std::move(object));
    return door;
}

Encounter &Game::newEncounter(ObjectTag tag) {
    auto object = std::make_unique<Encounter>(_nextObjectId++, std::move(tag));
    auto &encounter = *object;
    _objects.push_back(std::move(object));
    return encounter;
}

Item &Game::newItem(ObjectTag tag) {
    auto object = std::make_unique<Item>(_nextObjectId++, std::move(tag));
    auto &item = *object;
    _objects.push_back(std::move(object));
    return item;
}

Module &Game::newModule(ObjectTag tag) {
    auto object = std::make_unique<Module>(_nextObjectId++, std::move(tag), *this);
    auto &module = *object;
    _objects.push_back(std::move(object));
    return module;
}

Placeable &Game::newPlaceable(ObjectTag tag) {
    auto object = std::make_unique<Placeable>(_nextObjectId++, std::move(tag));
    auto &placeable = *object;
    _objects.push_back(std::move(object));
    return placeable;
}

Sound &Game::newSound(ObjectTag tag) {
    auto object = std::make_unique<Sound>(_nextObjectId++, std::move(tag));
    auto &sound = *object;
    _objects.push_back(std::move(object));
    return sound;
}

Store &Game::newStore(ObjectTag tag) {
    auto object = std::make_unique<Store>(_nextObjectId++, std::move(tag));
    auto &store = *object;
    _objects.push_back(std::move(object));
    return store;
}

Trigger &Game::newTrigger(ObjectTag tag) {
    auto object = std::make_unique<Trigger>(_nextObjectId++, std::move(tag));
    auto &trigger = *object;
    _objects.push_back(std::move(object));
    return trigger;
}

Waypoint &Game::newWaypoint(ObjectTag tag) {
    auto object = std::make_unique<Waypoint>(_nextObjectId++, std::move(tag));
    auto &waypoint = *object;
    _objects.push_back(std::move(object));
    return waypoint;
}

} // namespace neo

} // namespace game

} // namespace reone
