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

#include "reone/resource/2da.h"
#include "reone/resource/provider/2das.h"
#include "reone/resource/provider/textures.h"
#include "reone/resource/strings.h"

using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace game {

void Spells::init() {
    std::shared_ptr<TwoDA> spells(_twoDas.get("spells"));
    if (!spells)
        return;

    for (int row = 0; row < spells->getRowCount(); ++row) {
        std::string name(_strings.getText(spells->getInt(row, "name", -1)));
        std::string description(_strings.getText(spells->getInt(row, "spelldesc", -1)));
        std::shared_ptr<Texture> icon(_textures.get(spells->getString(row, "iconresref"), TextureUsage::GUI));
        uint32_t pips = spells->getHexInt(row, "pips");
        uint32_t maxcr = spells->getInt(row, "maxcr", 0);
        uint32_t category = spells->getHexInt(row, "category", 0);

        auto spell = std::make_shared<Spell>();
        spell->type = static_cast<SpellType>(row);
        spell->name = std::move(name);
        spell->description = std::move(description);
        spell->icon = std::move(icon);
        spell->pips = pips;
        spell->maxcr = maxcr;
        spell->category = category;

        _spellsArray.push_back(spell);
        _spells.insert(std::make_pair(spell->type, std::move(spell)));
    }

    // Sort spells by CR from highest (best spells) to lowest.
    std::sort(_spellsArray.begin(), _spellsArray.end(), [](auto lhs, auto rhs) {
        return lhs->maxcr > rhs->maxcr;
    });

    // Then sort by category, keeping order of CR for spells with the same
    // category.
    std::stable_sort(_spellsArray.begin(), _spellsArray.end(), [](auto lhs, auto rhs) {
        return lhs->category < rhs->category;
    });
}

std::shared_ptr<Spell> Spells::get(SpellType type) const {
    auto it = _spells.find(type);
    return it != _spells.end() ? it->second : nullptr;
}

} // namespace game

} // namespace reone
