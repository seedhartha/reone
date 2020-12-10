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

#include "door.h"

#include <stdexcept>

#include <boost/algorithm/string.hpp>

#include "../../resource/resources.h"

#include "../object/door.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace game {

DoorBlueprint::DoorBlueprint(const string &resRef, const shared_ptr<GffStruct> &utd) :
    _resRef(resRef),
    _utd(utd) {

    if (!utd) {
        throw invalid_argument("utd must not be null");
    }
}

void DoorBlueprint::load(Door &door) {
    door._tag = boost::to_lower_copy(_utd->getString("Tag"));

    int locNameStrRef = _utd->getInt("LocName", -1);
    if (locNameStrRef != -1) {
        door._title = Resources::instance().getString(locNameStrRef);
    }

    door._conversation = boost::to_lower_copy(_utd->getString("Conversation"));
    door._lockable = _utd->getInt("Lockable", 0) != 0;
    door._locked = _utd->getInt("Locked", 0) != 0;
    door._genericType = _utd->getInt("GenericType");
    door._static = _utd->getInt("Static", 0) != 0;
    door._selectable = !door._static;

    door._onOpen = _utd->getString("OnOpen");
    door._onFailToOpen = _utd->getString("OnFailToOpen");
}

const string &DoorBlueprint::resRef() const {
    return _resRef;
}

} // namespace game

} // namespace reone
