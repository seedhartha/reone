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

#include "reone/resource/parser/gff/utt.h"

#include "reone/resource/gff.h"

namespace reone {

namespace resource {

namespace generated {

UTT parseUTT(const Gff &gff) {
    UTT strct;
    strct.AutoRemoveKey = gff.getUint("AutoRemoveKey");
    strct.Comment = gff.getString("Comment");
    strct.Cursor = gff.getUint("Cursor");
    strct.DisarmDC = gff.getUint("DisarmDC");
    strct.Faction = gff.getUint("Faction");
    strct.HighlightHeight = gff.getFloat("HighlightHeight");
    strct.KeyName = gff.getString("KeyName");
    strct.LinkedTo = gff.getString("LinkedTo");
    strct.LinkedToFlags = gff.getUint("LinkedToFlags");
    strct.LoadScreenID = gff.getUint("LoadScreenID");
    strct.LocalizedName = std::make_pair(gff.getInt("LocalizedName"), gff.getString("LocalizedName"));
    strct.OnClick = gff.getString("OnClick");
    strct.OnDisarm = gff.getString("OnDisarm");
    strct.OnTrapTriggered = gff.getString("OnTrapTriggered");
    strct.PaletteID = gff.getUint("PaletteID");
    strct.PartyRequired = gff.getUint("PartyRequired");
    strct.Portrait = gff.getString("Portrait");
    strct.PortraitId = gff.getUint("PortraitId");
    strct.ScriptHeartbeat = gff.getString("ScriptHeartbeat");
    strct.ScriptOnEnter = gff.getString("ScriptOnEnter");
    strct.ScriptOnExit = gff.getString("ScriptOnExit");
    strct.ScriptUserDefine = gff.getString("ScriptUserDefine");
    strct.Tag = gff.getString("Tag");
    strct.TemplateResRef = gff.getString("TemplateResRef");
    strct.TrapDetectDC = gff.getUint("TrapDetectDC");
    strct.TrapDetectable = gff.getUint("TrapDetectable");
    strct.TrapDisarmable = gff.getUint("TrapDisarmable");
    strct.TrapFlag = gff.getUint("TrapFlag");
    strct.TrapOneShot = gff.getUint("TrapOneShot");
    strct.TrapType = gff.getUint("TrapType");
    strct.Type = gff.getInt("Type");
    return strct;
}

} // namespace generated

} // namespace resource

} // namespace reone
