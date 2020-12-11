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

#include "placeable.h"

#include <boost/algorithm/string.hpp>

#include "../../resource/resources.h"

#include "../object/objectfactory.h"
#include "../object/placeable.h"

#include "blueprints.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace game {

PlaceableBlueprint::PlaceableBlueprint(const string &resRef, const shared_ptr<GffStruct> &utp) :
    _resRef(resRef),
    _utp(utp) {

    if (!utp) {
        throw invalid_argument("utp must not be null");
    }
}

void PlaceableBlueprint::load(Placeable &placeable) {
    placeable._tag = boost::to_lower_copy(_utp->getString("Tag"));

    int locNameStrRef = _utp->getInt("LocName", -1);
    if (locNameStrRef != -1) {
        placeable._title = Resources::instance().getString(locNameStrRef);
    }

    placeable._appearance = _utp->getInt("Appearance");
    placeable._conversation = _utp->getString("Conversation");
    placeable._hasInventory = _utp->getBool("HasInventory");
    placeable._usable = _utp->getBool("Useable");
    placeable._minOneHP = _utp->getBool("Min1HP");
    placeable._hitPoints = _utp->getInt("HP");
    placeable._currentHitPoints = _utp->getInt("CurrentHP");

    loadItems(placeable);
    loadScripts(placeable);
}

void PlaceableBlueprint::loadItems(Placeable &placeable) {
    for (auto &itemGffs : _utp->getList("ItemList")) {
        string resRef(boost::to_lower_copy(itemGffs->getString("InventoryRes")));
        shared_ptr<ItemBlueprint> itemBlueprint(Blueprints::instance().getItem(resRef));

        shared_ptr<Item> item(placeable.objectFactory().newItem());
        item->load(itemBlueprint);

        placeable._items.push_back(move(item));
    }
}

void PlaceableBlueprint::loadScripts(Placeable &placeable) {
    placeable._onInvDisturbed = _utp->getString("OnInvDisturbed");
}

const string &PlaceableBlueprint::resRef() const {
    return _resRef;
}

} // namespace game

} // namespace reone
