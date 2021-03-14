/*
 * Copyright (c) 2020-2021 The reone project contributors
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

#include "reputes.h"

#include <unordered_map>

#include <boost/algorithm/string.hpp>

#include "../resource/resources.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace game {

constexpr int kDefaultRepute = 50;

static vector<string> g_factionLabels;
static vector<vector<int>> g_factionValues;

Reputes &Reputes::instance() {
    static Reputes instance;
    return instance;
}

void Reputes::init() {
    shared_ptr<TwoDA> repute(Resources::instance().get2DA("repute"));
    for (int row = 0; row < repute->getRowCount(); ++row) {
        g_factionLabels.push_back(boost::to_lower_copy(repute->getString(row, "label")));
    }
    for (int row = 0; row < repute->getRowCount(); ++row) {
        vector<int> values;
        for (size_t i = 0; i < g_factionLabels.size(); ++i) {
            int value = repute->getInt(row, g_factionLabels[i]);
            values.push_back(value);
        }
        g_factionValues.push_back(move(values));
    }
}

bool Reputes::getIsEnemy(const Creature &left, const Creature &right) {
    return getRepute(left, right) < 50;
}

int Reputes::getRepute(const Creature &left, const Creature &right) const {
    int leftFaction = static_cast<int>(left.faction());
    int rightFaction = static_cast<int>(right.faction());

    if (leftFaction < 0 || leftFaction >= g_factionValues.size() ||
        rightFaction < 0 || rightFaction >= g_factionValues[leftFaction].size()) return kDefaultRepute;

    return g_factionValues[leftFaction][rightFaction];
}

bool Reputes::getIsFriend(const Creature &left, const Creature &right) {
    return getRepute(left, right) > 50;
}

bool Reputes::getIsNeutral(const Creature &left, const Creature &right) {
    return getRepute(left, right) == 50;
}

} // namespace game

} // namespace reone
