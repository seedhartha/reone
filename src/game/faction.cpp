/*
 * Copyright (c) 2020 uwadmin12
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

#include "faction.h"
#include "../common/log.h"

namespace reone {

namespace game {

const std::list<Faction> HOSTILE = {
    Faction::STANDARD_FACTION_HOSTILE_1, Faction::STANDARD_FACTION_HOSTILE_2 };

const std::list<Faction> FRIENDLY = {
    Faction::STANDARD_FACTION_FRIENDLY_1, Faction::STANDARD_FACTION_FRIENDLY_2 };

const std::list<Faction> SURRENDER = {
    Faction::STANDARD_FACTION_SURRENDER_1, Faction::STANDARD_FACTION_SURRENDER_2 };

const std::list<Faction> GIZKA = {
    Faction::STANDARD_FACTION_GIZKA_1, Faction::STANDARD_FACTION_GIZKA_2 };

std::list<std::pair<Faction, Faction>> groupPairs(std::list<Faction> group1, std::list<Faction> group2) {
    std::list<std::pair<Faction, Faction>> aggr;
    for (auto f1 : group1) {
        for (auto f2 : group2) {
            aggr.push_back(std::make_pair(f1, f2));
        }
    }
    return std::move(aggr);
}

std::list<std::pair<Faction, Faction>> propagateHostileLinks() {
    std::list<std::pair<Faction, Faction>> aggr;

    // aggregate factions that are hostile towards each other?
    aggr.splice(aggr.end(), groupPairs(HOSTILE, FRIENDLY));
    aggr.splice(aggr.end(), groupPairs(HOSTILE, SURRENDER));
    aggr.splice(aggr.end(), groupPairs(HOSTILE, GIZKA));
    aggr.splice(aggr.end(), groupPairs(HOSTILE, { Faction::STANDARD_FACTION_ENDAR_SPIRE }));
    aggr.splice(aggr.end(), groupPairs(HOSTILE, { Faction::STANDARD_FACTION_PREDATOR }));
    aggr.splice(aggr.end(), groupPairs(HOSTILE, { Faction::STANDARD_FACTION_PREY }));
    aggr.splice(aggr.end(), groupPairs(FRIENDLY, { Faction::STANDARD_FACTION_PREDATOR }));
    aggr.splice(aggr.end(), groupPairs(FRIENDLY, { Faction::STANDARD_FACTION_PREY }));
    aggr.splice(aggr.end(), groupPairs(FRIENDLY, { Faction::STANDARD_FACTION_RANCOR }));
    aggr.splice(aggr.end(), groupPairs(FRIENDLY, { Faction::STANDARD_FACTION_PTAT_TUSKAN }));

    aggr.insert(aggr.end(), std::make_pair(Faction::STANDARD_FACTION_PREDATOR, Faction::STANDARD_FACTION_PREY));

    return std::move(aggr);
}

std::vector<std::vector<bool>> initialize() {
    std::vector<std::vector<bool>> arr(MAX_NUM_FACTION, std::vector<bool>(MAX_NUM_FACTION, false));

    // set insane faction hostile to all factions
    for (size_t i = 0; i < MAX_NUM_FACTION; ++i) {
        size_t j = static_cast<size_t>(Faction::STANDARD_FACTION_INSANE);
        arr[i][j] = true;
        arr[j][i] = true;
    }

    // propagate hostile links to map
    for (auto& pr : propagateHostileLinks()) {
        size_t i = static_cast<size_t>(pr.first);
        size_t j = static_cast<size_t>(pr.second);

        arr[i][j] = true;
        arr[j][i] = true;
    }

    return std::move(arr);
}

const std::vector<std::vector<bool>> _hostility = initialize();

bool getIsEnemy(const std::shared_ptr<Creature>& oTarget, const std::shared_ptr<Creature>& oSource) {
    if (!oTarget || !oSource) {
        debug("getIsEnemy, oTarget or OSource is nullptr");
        return false;
    }
        

    int s = static_cast<int>(oSource->getFaction());
    int t = static_cast<int>(oTarget->getFaction());
    if (s < 0 || s >= MAX_NUM_FACTION || t < 0 || t >= MAX_NUM_FACTION) {
        debug(boost::format("Source %s Faction: %d") % oSource->tag() % s);
        debug(boost::format("Target %s Faction: %d") % oTarget->tag() % t);

        return false;
    }

    return _hostility[s][t];
}

} // namespace game

} // namespace reone
