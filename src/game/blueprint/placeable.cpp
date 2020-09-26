/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "placeable.h"

#include <boost/algorithm/string.hpp>

using namespace std;

using namespace reone::resources;

namespace reone {

namespace game {

void PlaceableBlueprint::load(const string &resRef, const GffStruct &utp) {
    _tag = utp.getString("Tag");
    boost::to_lower(_tag);

    _appearance = utp.getInt("Appearance");
    _hasInventory = utp.getInt("HasInventory") != 0;

    const GffField *itemList = utp.find("ItemList");
    if (itemList) {
        for (auto &item : itemList->children()) {
            string resRef(item.getString("InventoryRes"));
            boost::to_lower(resRef);

            _items.push_back(move(resRef));
        }
    }
}

const string &PlaceableBlueprint::tag() const {
    return _tag;
}

int PlaceableBlueprint::appearance() const {
    return _appearance;
}

bool PlaceableBlueprint::hasInventory() const {
    return _hasInventory;
}

const vector<string> &PlaceableBlueprint::items() const {
    return _items;
}

} // namespace game

} // namespace reone
