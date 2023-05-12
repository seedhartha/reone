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

#include "reone/game/d20/spells.h"

#include "reone/system/collectionutil.h"
#include "reone/graphics/textures.h"
#include "reone/resource/2da.h"
#include "reone/resource/2das.h"
#include "reone/resource/strings.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace game {

void Spells::init() {
    shared_ptr<TwoDa> spells(_twoDas.get("spells"));
    if (!spells)
        return;

    for (int row = 0; row < spells->getRowCount(); ++row) {
        string name(_strings.get(spells->getInt(row, "name", -1)));
        string description(_strings.get(spells->getInt(row, "spelldesc", -1)));
        shared_ptr<Texture> icon(_textures.get(spells->getString(row, "iconresref"), TextureUsage::GUI));
        uint32_t pips = spells->getUint(row, "pips");

        auto spell = make_shared<Spell>();
        spell->name = move(name);
        spell->description = move(description);
        spell->icon = move(icon);
        spell->pips = pips;
        _spells.insert(make_pair(static_cast<SpellType>(row), move(spell)));
    }
}

shared_ptr<Spell> Spells::get(SpellType type) const {
    return getFromLookupOrNull(_spells, type);
}

} // namespace game

} // namespace reone
