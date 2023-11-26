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

#pragma once

namespace reone {

namespace resource {

class Gff;

}

namespace game {

namespace generated {

struct IFO_Mod_Area_list {
    std::string Area_Name;
};

struct IFO {
    uint16_t Expansion_Pack {0};
    std::vector<IFO_Mod_Area_list> Mod_Area_list;
    int Mod_Creator_ID {0};
    std::vector<void *> Mod_CutSceneList;
    uint8_t Mod_DawnHour {0};
    std::pair<int, std::string> Mod_Description;
    uint8_t Mod_DuskHour {0};
    std::string Mod_Entry_Area;
    float Mod_Entry_Dir_X {0.0f};
    float Mod_Entry_Dir_Y {0.0f};
    float Mod_Entry_X {0.0f};
    float Mod_Entry_Y {0.0f};
    float Mod_Entry_Z {0.0f};
    std::vector<void *> Mod_Expan_List;
    std::vector<void *> Mod_GVar_List;
    std::string Mod_Hak;
    std::vector<char> Mod_ID;
    uint8_t Mod_IsSaveGame {0};
    uint8_t Mod_MinPerHour {0};
    std::pair<int, std::string> Mod_Name;
    std::string Mod_OnAcquirItem;
    std::string Mod_OnActvtItem;
    std::string Mod_OnClientEntr;
    std::string Mod_OnClientLeav;
    std::string Mod_OnHeartbeat;
    std::string Mod_OnModLoad;
    std::string Mod_OnModStart;
    std::string Mod_OnPlrDeath;
    std::string Mod_OnPlrDying;
    std::string Mod_OnPlrLvlUp;
    std::string Mod_OnPlrRest;
    std::string Mod_OnSpawnBtnDn;
    std::string Mod_OnUnAqreItem;
    std::string Mod_OnUsrDefined;
    uint8_t Mod_StartDay {0};
    uint8_t Mod_StartHour {0};
    uint8_t Mod_StartMonth {0};
    std::string Mod_StartMovie;
    uint32_t Mod_StartYear {0};
    std::string Mod_Tag;
    std::string Mod_VO_ID;
    uint32_t Mod_Version {0};
    uint8_t Mod_XPScale {0};
};

IFO parseIFO(const resource::Gff &gff);

} // namespace generated

} // namespace game

} // namespace reone
