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
 *  Waypoint functions related to blueprint loading.
 */

#include "waypoint.h"

#include <boost/algorithm/string.hpp>

#include "../../resource/strings.h"

using namespace reone::resource;

namespace reone {

namespace game {

void Waypoint::loadUTW(const GffStruct &utw) {
    _appearance = utw.getInt("Appearance");
    _blueprintResRef = boost::to_lower_copy(utw.getString("TemplateResRef"));
    _tag = boost::to_lower_copy(utw.getString("Tag"));
    _name = Strings::instance().get(utw.getInt("LocalizedName"));
    _hasMapNote = utw.getBool("HasMapNote");
    _mapNote = Strings::instance().get(utw.getInt("MapNote"));
    _mapNoteEnabled = utw.getInt("MapNoteEnabled");

    // Unused fields:
    //
    // - Description
    // - PaletteID
    // - Comment
}

} // namespace resource

} // namespace reone
