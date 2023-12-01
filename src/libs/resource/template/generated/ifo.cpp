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

#include "reone/resource/template/generated/ifo.h"

#include "reone/resource/gff.h"

namespace reone {

namespace resource {

namespace generated {

static IFO_Mod_Area_list parseIFO_Mod_Area_list(const Gff &gff) {
    IFO_Mod_Area_list strct;
    strct.Area_Name = gff.getString("Area_Name");
    return strct;
}

IFO parseIFO(const Gff &gff) {
    IFO strct;
    strct.Expansion_Pack = gff.getUint("Expansion_Pack");
    for (auto &item : gff.getList("Mod_Area_list")) {
        strct.Mod_Area_list.push_back(parseIFO_Mod_Area_list(*item));
    }
    strct.Mod_Creator_ID = gff.getInt("Mod_Creator_ID");
    strct.Mod_DawnHour = gff.getUint("Mod_DawnHour");
    strct.Mod_Description = std::make_pair(gff.getInt("Mod_Description"), gff.getString("Mod_Description"));
    strct.Mod_DuskHour = gff.getUint("Mod_DuskHour");
    strct.Mod_Entry_Area = gff.getString("Mod_Entry_Area");
    strct.Mod_Entry_Dir_X = gff.getFloat("Mod_Entry_Dir_X");
    strct.Mod_Entry_Dir_Y = gff.getFloat("Mod_Entry_Dir_Y");
    strct.Mod_Entry_X = gff.getFloat("Mod_Entry_X");
    strct.Mod_Entry_Y = gff.getFloat("Mod_Entry_Y");
    strct.Mod_Entry_Z = gff.getFloat("Mod_Entry_Z");
    strct.Mod_Hak = gff.getString("Mod_Hak");
    strct.Mod_ID = gff.getData("Mod_ID");
    strct.Mod_IsSaveGame = gff.getUint("Mod_IsSaveGame");
    strct.Mod_MinPerHour = gff.getUint("Mod_MinPerHour");
    strct.Mod_Name = std::make_pair(gff.getInt("Mod_Name"), gff.getString("Mod_Name"));
    strct.Mod_OnAcquirItem = gff.getString("Mod_OnAcquirItem");
    strct.Mod_OnActvtItem = gff.getString("Mod_OnActvtItem");
    strct.Mod_OnClientEntr = gff.getString("Mod_OnClientEntr");
    strct.Mod_OnClientLeav = gff.getString("Mod_OnClientLeav");
    strct.Mod_OnHeartbeat = gff.getString("Mod_OnHeartbeat");
    strct.Mod_OnModLoad = gff.getString("Mod_OnModLoad");
    strct.Mod_OnModStart = gff.getString("Mod_OnModStart");
    strct.Mod_OnPlrDeath = gff.getString("Mod_OnPlrDeath");
    strct.Mod_OnPlrDying = gff.getString("Mod_OnPlrDying");
    strct.Mod_OnPlrLvlUp = gff.getString("Mod_OnPlrLvlUp");
    strct.Mod_OnPlrRest = gff.getString("Mod_OnPlrRest");
    strct.Mod_OnSpawnBtnDn = gff.getString("Mod_OnSpawnBtnDn");
    strct.Mod_OnUnAqreItem = gff.getString("Mod_OnUnAqreItem");
    strct.Mod_OnUsrDefined = gff.getString("Mod_OnUsrDefined");
    strct.Mod_StartDay = gff.getUint("Mod_StartDay");
    strct.Mod_StartHour = gff.getUint("Mod_StartHour");
    strct.Mod_StartMonth = gff.getUint("Mod_StartMonth");
    strct.Mod_StartMovie = gff.getString("Mod_StartMovie");
    strct.Mod_StartYear = gff.getUint("Mod_StartYear");
    strct.Mod_Tag = gff.getString("Mod_Tag");
    strct.Mod_VO_ID = gff.getString("Mod_VO_ID");
    strct.Mod_Version = gff.getUint("Mod_Version");
    strct.Mod_XPScale = gff.getUint("Mod_XPScale");
    return strct;
}

} // namespace generated

} // namespace resource

} // namespace reone
