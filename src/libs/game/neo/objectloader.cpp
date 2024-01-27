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

#include "reone/game/neo/objectloader.h"

#include "reone/game/neo/object/area.h"
#include "reone/game/neo/object/camera.h"
#include "reone/game/neo/object/creature.h"
#include "reone/game/neo/object/door.h"
#include "reone/game/neo/object/encounter.h"
#include "reone/game/neo/object/item.h"
#include "reone/game/neo/object/module.h"
#include "reone/game/neo/object/placeable.h"
#include "reone/game/neo/object/sound.h"
#include "reone/game/neo/object/store.h"
#include "reone/game/neo/object/trigger.h"
#include "reone/game/neo/object/waypoint.h"
#include "reone/game/neo/objectfactory.h"
#include "reone/resource/di/services.h"
#include "reone/resource/exception/notfound.h"
#include "reone/resource/parser/2da/appearance.h"
#include "reone/resource/parser/2da/genericdoors.h"
#include "reone/resource/parser/2da/heads.h"
#include "reone/resource/parser/2da/placeables.h"
#include "reone/resource/parser/2da/portraits.h"
#include "reone/resource/parser/gff/are.h"
#include "reone/resource/parser/gff/git.h"
#include "reone/resource/parser/gff/ifo.h"
#include "reone/resource/parser/gff/utc.h"
#include "reone/resource/parser/gff/utd.h"
#include "reone/resource/parser/gff/ute.h"
#include "reone/resource/parser/gff/utm.h"
#include "reone/resource/parser/gff/utp.h"
#include "reone/resource/parser/gff/uts.h"
#include "reone/resource/parser/gff/utt.h"
#include "reone/resource/parser/gff/utw.h"
#include "reone/resource/provider/2das.h"
#include "reone/resource/provider/gffs.h"
#include "reone/resource/provider/layouts.h"
#include "reone/resource/provider/paths.h"
#include "reone/resource/provider/visibilities.h"

using namespace reone::resource;
using namespace reone::resource::generated;

namespace reone {

namespace game {

namespace neo {

Module &ObjectLoader::loadModule(const std::string &name) {
    auto ifo = _resourceSvc.gffs.get("module", ResType::Ifo);
    if (!ifo) {
        throw ResourceNotFoundException("Module IFO not found: " + name);
    }
    auto parsedIFO = parseIFO(*ifo);
    auto &module = _objectFactory.newModule(parsedIFO.Mod_Tag);
    module.load(parsedIFO);
    return module;
}

Area &ObjectLoader::loadArea(const std::string &name) {
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
    auto &area = _objectFactory.newArea(parsedARE.Tag);
    area.load(parsedARE, parsedGIT, *lyt, *vis, *pth);
    return area;
}

Camera &ObjectLoader::loadCamera() {
    auto &camera = _objectFactory.newCamera("");
    return camera;
}

Creature &ObjectLoader::loadCreature(const resource::ResRef &tmplt) {
    auto utc = _resourceSvc.gffs.get(tmplt.value(), ResType::Utc);
    if (!utc) {
        throw ResourceNotFoundException("Creature UTC not found: " + tmplt.value());
    }
    auto parsedUTC = parseUTC(*utc);
    auto &creature = _objectFactory.newCreature(parsedUTC.Tag);
    auto appearanceRaw = _resourceSvc.twoDas.get("appearance");
    if (!appearanceRaw) {
        throw ResourceNotFoundException("appearance 2DA not found");
    }
    auto appearance = parseAppearanceTwoDA(*appearanceRaw);
    auto headsRaw = _resourceSvc.twoDas.get("heads");
    if (!headsRaw) {
        throw ResourceNotFoundException("heads 2DA not found");
    }
    auto heads = parseHeadsTwoDA(*headsRaw);
    creature.load(parsedUTC, appearance, heads);
    return creature;
}

Creature &ObjectLoader::loadCreature(ObjectTag tag, PortraitId portraitId) {
    auto &creature = _objectFactory.newCreature(std::move(tag));
    auto portraits = _resourceSvc.twoDas.get("portraits");
    if (!portraits) {
        throw ResourceNotFoundException("portraits 2DA not found");
    }
    auto appearanceRaw = _resourceSvc.twoDas.get("appearance");
    if (!appearanceRaw) {
        throw ResourceNotFoundException("appearance 2DA not found");
    }
    auto appearance = parseAppearanceTwoDA(*appearanceRaw);
    auto headsRaw = _resourceSvc.twoDas.get("heads");
    if (!headsRaw) {
        throw ResourceNotFoundException("heads 2DA not found");
    }
    auto heads = parseHeadsTwoDA(*headsRaw);
    auto portraitsRow = parsePortraitsTwoDARow(*portraits, portraitId);
    creature.load(*portraitsRow.appearancenumber, appearance, heads);
    return creature;
}

Door &ObjectLoader::loadDoor(const resource::ResRef &tmplt) {
    auto utd = _resourceSvc.gffs.get(tmplt.value(), ResType::Utd);
    if (!utd) {
        throw ResourceNotFoundException("Door UTD not found: " + tmplt.value());
    }
    auto parsedUTD = parseUTD(*utd);
    auto &door = _objectFactory.newDoor(parsedUTD.Tag);
    auto genericDoorsRaw = _resourceSvc.twoDas.get("genericdoors");
    if (!genericDoorsRaw) {
        throw ResourceNotFoundException("genericdoors 2DA not found");
    }
    auto genericDoors = parseGenericdoorsTwoDA(*genericDoorsRaw);
    door.load(parsedUTD, genericDoors);
    return door;
}

Encounter &ObjectLoader::loadEncounter(const resource::ResRef &tmplt) {
    auto ute = _resourceSvc.gffs.get(tmplt.value(), ResType::Ute);
    if (!ute) {
        throw ResourceNotFoundException("Encounter UTE not found: " + tmplt.value());
    }
    auto parsedUTE = parseUTE(*ute);
    auto &encounter = _objectFactory.newEncounter(parsedUTE.Tag);
    return encounter;
}

Placeable &ObjectLoader::loadPlaceable(const resource::ResRef &tmplt) {
    auto utp = _resourceSvc.gffs.get(tmplt.value(), ResType::Utp);
    if (!utp) {
        throw ResourceNotFoundException("Placeable UTP not found: " + tmplt.value());
    }
    auto parsedUTP = parseUTP(*utp);
    auto &placeable = _objectFactory.newPlaceable(parsedUTP.Tag);
    auto placeablesRaw = _resourceSvc.twoDas.get("placeables");
    if (!placeablesRaw) {
        throw ResourceNotFoundException("placeables 2DA not found");
    }
    auto placeables = parsePlaceablesTwoDA(*placeablesRaw);
    placeable.load(parsedUTP, placeables);
    return placeable;
}

Sound &ObjectLoader::loadSound(const resource::ResRef &tmplt) {
    auto uts = _resourceSvc.gffs.get(tmplt.value(), ResType::Uts);
    if (!uts) {
        throw ResourceNotFoundException("Sound UTS not found: " + tmplt.value());
    }
    auto parsedUTS = parseUTS(*uts);
    auto &sound = _objectFactory.newSound(parsedUTS.Tag);
    return sound;
}

Store &ObjectLoader::loadStore(const resource::ResRef &tmplt) {
    auto utm = _resourceSvc.gffs.get(tmplt.value(), ResType::Utm);
    if (!utm) {
        throw ResourceNotFoundException("Store UTM not found: " + tmplt.value());
    }
    auto parsedUTM = parseUTM(*utm);
    auto &store = _objectFactory.newStore(parsedUTM.Tag);
    return store;
}

Trigger &ObjectLoader::loadTrigger(const resource::ResRef &tmplt) {
    auto utt = _resourceSvc.gffs.get(tmplt.value(), ResType::Utt);
    if (!utt) {
        throw ResourceNotFoundException("Trigger UTT not found: " + tmplt.value());
    }
    auto parsedUTT = parseUTT(*utt);
    auto &trigger = _objectFactory.newTrigger(parsedUTT.Tag);
    return trigger;
}

Waypoint &ObjectLoader::loadWaypoint(const resource::ResRef &tmplt) {
    auto utw = _resourceSvc.gffs.get(tmplt.value(), ResType::Utw);
    if (!utw) {
        throw ResourceNotFoundException("Waypoint UTW not found: " + tmplt.value());
    }
    auto parsedUTW = parseUTW(*utw);
    auto &waypoint = _objectFactory.newWaypoint(parsedUTW.Tag);
    return waypoint;
}

} // namespace neo

} // namespace game

} // namespace reone
