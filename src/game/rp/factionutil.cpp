/*
 * Copyright (c) 2020 The reone project contributors
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

#include "factionutil.h"

#include <list>

#include "../common/log.h"

#include "object/creature.h"

using namespace std;

namespace reone {

namespace game {

constexpr static int kMaxFactionCount = 25;

const list<Faction> g_hostileFactions { Faction::Hostile1, Faction::Hostile2 };
const list<Faction> g_friendlyFactions { Faction::Friendly1, Faction::Friendly2 };
const list<Faction> g_surrenderFactions { Faction::Surrender1, Faction::Surrender2 };
const list<Faction> g_gizkaFactions { Faction::Gizka1, Faction::Gizka2 };

list<pair<Faction, Faction>> groupPairs(list<Faction> group1, list<Faction> group2) {
    list<pair<Faction, Faction>> aggr;
    for (auto &f1 : group1) {
        for (auto &f2 : group2) {
            aggr.push_back(make_pair(f1, f2));
        }
    }
    return move(aggr);
}

list<pair<Faction, Faction>> propagateHostileLinks() {
    list<pair<Faction, Faction>> aggr;

    // aggregate factions that are hostile towards each other?
    aggr.splice(aggr.end(), groupPairs(g_hostileFactions, g_friendlyFactions));
    aggr.splice(aggr.end(), groupPairs(g_hostileFactions, g_surrenderFactions));
    aggr.splice(aggr.end(), groupPairs(g_hostileFactions, g_gizkaFactions));
    aggr.splice(aggr.end(), groupPairs(g_hostileFactions, { Faction::EndarSpire }));
    aggr.splice(aggr.end(), groupPairs(g_hostileFactions, { Faction::Predator }));
    aggr.splice(aggr.end(), groupPairs(g_hostileFactions, { Faction::Prey }));
    aggr.splice(aggr.end(), groupPairs(g_friendlyFactions, { Faction::Predator }));
    aggr.splice(aggr.end(), groupPairs(g_friendlyFactions, { Faction::Prey }));
    aggr.splice(aggr.end(), groupPairs(g_friendlyFactions, { Faction::Rancor }));
    aggr.splice(aggr.end(), groupPairs(g_friendlyFactions, { Faction::Tuskan }));

    aggr.insert(aggr.end(), make_pair(Faction::Predator, Faction::Prey));

    return move(aggr);
}

vector<vector<bool>> initialize() {
    vector<vector<bool>> arr(kMaxFactionCount, vector<bool>(kMaxFactionCount, false));

    // set insane faction hostile to all factions
    for (size_t i = 0; i < kMaxFactionCount; ++i) {
        size_t j = static_cast<size_t>(Faction::Insane);
        arr[i][j] = true;
        arr[j][i] = true;
    }

    // propagate hostile links to map
    for (auto &pr : propagateHostileLinks()) {
        size_t i = static_cast<size_t>(pr.first);
        size_t j = static_cast<size_t>(pr.second);

        arr[i][j] = true;
        arr[j][i] = true;
    }

    return move(arr);
}

const vector<vector<bool>> g_hostility = initialize();

bool getIsEnemy(const Creature &left, const Creature &right) {
    int leftFaction = static_cast<int>(left.faction());
    int rightFaction = static_cast<int>(right.faction());

    if (leftFaction < 0 || leftFaction >= kMaxFactionCount || rightFaction < 0 || rightFaction >= kMaxFactionCount) {
        debug(boost::format("Source %s Faction: %d") % left.tag() % leftFaction);
        debug(boost::format("Target %s Faction: %d") % right.tag() % rightFaction);

        return false;
    }

    return g_hostility[leftFaction][rightFaction];
}

} // namespace game

} // namespace reone
