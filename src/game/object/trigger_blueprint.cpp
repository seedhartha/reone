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
 *  Trigger functions related to blueprint loading.
 */

#include "trigger.h"

#include <boost/algorithm/string.hpp>

#include "../../resource/strings.h"

using namespace reone::resource;

namespace reone {

namespace game {

void Trigger::loadUTT(const GffStruct &utt) {
    _tag = boost::to_lower_copy(utt.getString("Tag"));
    _blueprintResRef = boost::to_lower_copy(utt.getString("TemplateResRef"));
    _name = Strings::instance().get(utt.getInt("LocalizedName"));
    _faction = utt.getEnum("Faction", Faction::Invalid);
    _cursor = utt.getInt("Cursor");
    _hilightHeight = utt.getFloat("HighlightHeight");
    _keyName = utt.getString("KeyName");
    _triggerType = utt.getInt("Type");
    _trapDetectable = utt.getBool("TrapDetectable");
    _trapDetectDC = utt.getInt("TrapDetectDC");
    _trapDisarmable = utt.getBool("TrapDisarmable");
    _disarmDC = utt.getInt("DisarmDC");
    _trapFlag = utt.getBool("TrapFlag");
    _trapType = utt.getInt("TrapType");

    _onClick = boost::to_lower_copy(utt.getString("OnClick"));
    _onHeartbeat = boost::to_lower_copy(utt.getString("ScriptHeartbeat"));
    _onEnter = boost::to_lower_copy(utt.getString("ScriptOnEnter"));
    _onExit = boost::to_lower_copy(utt.getString("ScriptOnExit"));

    // Unused fields:
    //
    // - AutoRemoveKey
    // - LoadScreenID
    // - PortraitId
    // - TrapOneShot
    // - OnDisarm
    // - OnTrapTriggered
    // - ScriptUserDefine
    // - PaletteID
    // - Comment
}

} // namespace resource

} // namespace reone
