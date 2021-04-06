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
 *  Placeable functions related to blueprint loading.
 */

#include "placeable.h"

#include <boost/algorithm/string.hpp>

#include "../../resource/strings.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace game {

void Placeable::loadUTP(const GffStruct &utp) {
    _appearance = utp.getInt("Appearance");
    _blueprintResRef = boost::to_lower_copy(utp.getString("TemplateResRef"));
    _conversation = utp.getString("Conversation");
    _currentHitPoints = utp.getInt("CurrentHP");
    _hasInventory = utp.getBool("HasInventory");
    _hitPoints = utp.getInt("HP");
    _minOneHP = utp.getBool("Min1HP");
    _tag = boost::to_lower_copy(utp.getString("Tag"));
    _usable = utp.getBool("Useable");

    loadNameFromUTP(utp);
    loadScriptsFromUTP(utp);

    for (auto &itemGffs : utp.getList("ItemList")) {
        string resRef(boost::to_lower_copy(itemGffs->getString("InventoryRes")));
        addItem(resRef, 1, true);
    }
}

void Placeable::loadNameFromUTP(const GffStruct &utp) {
    int locNameStrRef = utp.getInt("LocName", -1);
    if (locNameStrRef != -1) {
        _name = Strings::instance().get(locNameStrRef);
    }
}

void Placeable::loadScriptsFromUTP(const GffStruct &utp) {
    _heartbeat = boost::to_lower_copy(utp.getString("OnHeartbeat"));
    _onInvDisturbed = boost::to_lower_copy(utp.getString("OnInvDisturbed"));
    _onUsed = boost::to_lower_copy(utp.getString("OnUsed"));
    _onUserDefined = boost::to_lower_copy(utp.getString("OnUserDefined"));
}

} // namespace game

} // namespace reone
