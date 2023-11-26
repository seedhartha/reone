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

#include "reone/game/generated/ute.h"

#include "reone/resource/gff.h"

using namespace reone::resource;

namespace reone {

namespace game {

namespace generated {

static UTE_CreatureList parseUTE_CreatureList(const Gff &gff) {
    UTE_CreatureList strct;
    strct.Appearance = gff.getInt("Appearance");
    strct.CR = gff.getFloat("CR");
    strct.GuaranteedCount = gff.getInt("GuaranteedCount");
    strct.ResRef = gff.getString("ResRef");
    strct.SingleSpawn = gff.getUint("SingleSpawn");
    return strct;
}

UTE parseUTE(const Gff &gff) {
    UTE strct;
    strct.Active = gff.getUint("Active");
    strct.Comment = gff.getString("Comment");
    for (auto &item : gff.getList("CreatureList")) {
        strct.CreatureList.push_back(parseUTE_CreatureList(*item));
    }
    strct.Difficulty = gff.getInt("Difficulty");
    strct.DifficultyIndex = gff.getInt("DifficultyIndex");
    strct.Faction = gff.getUint("Faction");
    strct.LocalizedName = std::make_pair(gff.getInt("LocalizedName"), gff.getString("LocalizedName"));
    strct.MaxCreatures = gff.getInt("MaxCreatures");
    strct.OnEntered = gff.getString("OnEntered");
    strct.OnExhausted = gff.getString("OnExhausted");
    strct.OnExit = gff.getString("OnExit");
    strct.OnHeartbeat = gff.getString("OnHeartbeat");
    strct.OnUserDefined = gff.getString("OnUserDefined");
    strct.PaletteID = gff.getUint("PaletteID");
    strct.PlayerOnly = gff.getUint("PlayerOnly");
    strct.RecCreatures = gff.getInt("RecCreatures");
    strct.Reset = gff.getUint("Reset");
    strct.ResetTime = gff.getInt("ResetTime");
    strct.Respawns = gff.getInt("Respawns");
    strct.SpawnOption = gff.getInt("SpawnOption");
    strct.Tag = gff.getString("Tag");
    strct.TemplateResRef = gff.getString("TemplateResRef");
    return strct;
}

} // namespace generated

} // namespace game

} // namespace reone
