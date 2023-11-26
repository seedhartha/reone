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

#include "reone/game/generated/utm.h"

#include "reone/resource/gff.h"

using namespace reone::resource;

namespace reone {

namespace game {

namespace generated {

static UTM_ItemList parseUTM_ItemList(const Gff &gff) {
    UTM_ItemList strct;
    strct.Infinite = gff.getUint("Infinite");
    strct.InventoryRes = gff.getString("InventoryRes");
    strct.Repos_PosX = gff.getUint("Repos_PosX");
    strct.Repos_Posy = gff.getUint("Repos_Posy");
    return strct;
}

UTM parseUTM(const Gff &gff) {
    UTM strct;
    strct.BuySellFlag = gff.getUint("BuySellFlag");
    strct.Comment = gff.getString("Comment");
    strct.ID = gff.getUint("ID");
    for (auto &item : gff.getList("ItemList")) {
        strct.ItemList.push_back(parseUTM_ItemList(*item));
    }
    strct.LocName = std::make_pair(gff.getInt("LocName"), gff.getString("LocName"));
    strct.MarkDown = gff.getInt("MarkDown");
    strct.MarkUp = gff.getInt("MarkUp");
    strct.OnOpenStore = gff.getString("OnOpenStore");
    strct.ResRef = gff.getString("ResRef");
    strct.Tag = gff.getString("Tag");
    return strct;
}

} // namespace generated

} // namespace game

} // namespace reone
