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

#include "reone/game/reputes.h"

#include "reone/resource/2da.h"
#include "reone/resource/provider/2das.h"

#include "reone/game/object/creature.h"

using namespace reone::resource;

namespace reone {

namespace game {

static constexpr int kDefaultRepute = 50;

static std::vector<std::string> g_factionLabels;
static std::vector<std::vector<int>> g_factionValues;

void Reputes::init() {
    std::shared_ptr<TwoDa> repute(_twoDas.get("repute"));
    if (!repute) {
        return;
    }

    for (int row = 0; row < repute->getRowCount(); ++row) {
        g_factionLabels.push_back(boost::to_lower_copy(repute->getString(row, "label")));
    }

    for (int row = 0; row < repute->getRowCount(); ++row) {
        std::vector<int> values;
        for (size_t i = 0; i < g_factionLabels.size(); ++i) {
            int value;

            const std::string &label = g_factionLabels[i];
            if (label == "player" || label == "glb_xor") {
                value = kDefaultRepute;
            } else {
                value = repute->getInt(row, g_factionLabels[i], kDefaultRepute);
            }

            values.push_back(value);
        }
        g_factionValues.push_back(std::move(values));
    }
}

bool Reputes::getIsEnemy(const Creature &left, const Creature &right) const {
    // HACK: friendlies must not attack each other, unless both are immortal
    if ((left.faction() == Faction::Friendly1 && right.faction() == Faction::Friendly2) ||
        (left.faction() == Faction::Friendly2 && right.faction() == Faction::Friendly1)) {

        return left.isMinOneHP() && right.isMinOneHP();
    }

    return getRepute(left, right) < 50;
}

bool Reputes::getIsFriend(const Creature &left, const Creature &right) const {
    return getRepute(left, right) > 50;
}

bool Reputes::getIsNeutral(const Creature &left, const Creature &right) const {
    return getRepute(left, right) == 50;
}

int Reputes::getRepute(const Creature &left, const Creature &right) const {
    int leftFaction = static_cast<int>(left.faction());
    int rightFaction = static_cast<int>(right.faction());

    if (leftFaction < 0 || leftFaction >= g_factionValues.size() ||
        rightFaction < 0 || rightFaction >= g_factionValues[leftFaction].size())
        return kDefaultRepute;

    return g_factionValues[leftFaction][rightFaction];
}

} // namespace game

} // namespace reone
