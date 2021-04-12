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
 *  Door functions related to blueprint loading.
 */

#include "door.h"

#include <boost/algorithm/string.hpp>

#include "../../resource/strings.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace game {

void Door::loadUTD(const GffStruct &utd) {
    _blueprintResRef = boost::to_lower_copy(utd.getString("TemplateResRef"));
    _conversation = boost::to_lower_copy(utd.getString("Conversation"));
    _currentHitPoints = utd.getInt("CurrentHP");
    _genericType = utd.getInt("GenericType");
    _hitPoints = utd.getInt("HP");
    _keyRequired = utd.getBool("KeyRequired");
    _lockable = utd.getBool("Lockable");
    _locked = utd.getBool("Locked");
    _minOneHP = utd.getBool("Min1HP");
    _static = utd.getBool("Static");
    _tag = boost::to_lower_copy(utd.getString("Tag"));

    loadNameFromUTD(utd);
    loadScriptsFromUTD(utd);
}

void Door::loadNameFromUTD(const GffStruct &utd) {
    int locNameStrRef = utd.getInt("LocName", -1);
    if (locNameStrRef != -1) {
        _name = Strings::instance().get(locNameStrRef);
    }
}

void Door::loadScriptsFromUTD(const GffStruct &utd) {
    _onHeartbeat = utd.getString("OnHeartbeat");
    _onFailToOpen = utd.getString("OnFailToOpen");
    _onOpen = utd.getString("OnOpen");
}

} // namespace game

} // namespace reone
