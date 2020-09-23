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

#include "creature.h"

#include <boost/algorithm/string.hpp>

using namespace std;

using namespace reone::resources;

namespace reone {

namespace game {

void CreatureBlueprint::load(const string &resRef, const GffStruct &utc) {
    _tag = utc.getString("Tag");
    boost::to_lower(_tag);

    for (auto &item : utc.getList("Equip_ItemList")) {
        string itemResRef(item.getString("EquippedRes"));
        boost::to_lower(itemResRef);

        _equipment.push_back(move(itemResRef));
    }

    _appearance = utc.getInt("Appearance_Type");
    _conversation = utc.getString("Conversation");
    _scripts[ScriptType::Spawn] = utc.getString("ScriptSpawn");
}

const string &CreatureBlueprint::tag() const {
    return _tag;
}

const vector<string> &CreatureBlueprint::equipment() const {
    return _equipment;
}

int CreatureBlueprint::appearance() const {
    return _appearance;
}

const string &CreatureBlueprint::conversation() const {
    return _conversation;
}

} // namespace game

} // namespace reone
