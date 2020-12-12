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

#include "../../common/log.h"

#include "../object/creature.h"

using namespace std;

namespace reone {

namespace game {

enum class Disposition {
    Neutral,
    Friend,
    Enemy
};

typedef unordered_map<Faction, unordered_map<Faction, Disposition>> DispositionsMap;

static const list<Faction> g_hostileFactions { Faction::Hostile1, Faction::Hostile2 };
static const list<Faction> g_friendlyFactions { Faction::Friendly1, Faction::Friendly2 };
static const list<Faction> g_surrenderFactions { Faction::Surrender1, Faction::Surrender2 };
static const list<Faction> g_gizkaFactions { Faction::Gizka1, Faction::Gizka2 };

static void setMutualDisposition(Faction left, Faction right, Disposition disposition, DispositionsMap &dispositions) {
    dispositions[left][right] = disposition;
    dispositions[right][left] = disposition;
}

static void setMutualFriends(Faction left, Faction right, DispositionsMap &dispositions) {
    setMutualDisposition(left, right, Disposition::Friend, dispositions);
}

static void setMutualEnemies(Faction left, Faction right, DispositionsMap &dispositions) {
    setMutualDisposition(left, right, Disposition::Enemy, dispositions);
}

static DispositionsMap initDispositions() {
    DispositionsMap result;

    // Friendlies

    setMutualFriends(Faction::Friendly1, Faction::Friendly2, result);

    setMutualEnemies(Faction::Friendly1, Faction::Hostile1, result);
    setMutualEnemies(Faction::Friendly1, Faction::Hostile2, result);
    setMutualEnemies(Faction::Friendly1, Faction::Insane, result);
    setMutualEnemies(Faction::Friendly1, Faction::Predator, result);
    setMutualEnemies(Faction::Friendly1, Faction::Rancor, result);
    setMutualEnemies(Faction::Friendly1, Faction::Tuskan, result);

    setMutualEnemies(Faction::Friendly2, Faction::Hostile1, result);
    setMutualEnemies(Faction::Friendly2, Faction::Hostile2, result);
    setMutualEnemies(Faction::Friendly2, Faction::Insane, result);
    setMutualEnemies(Faction::Friendly2, Faction::Predator, result);
    setMutualEnemies(Faction::Friendly2, Faction::Rancor, result);
    setMutualEnemies(Faction::Friendly2, Faction::Tuskan, result);

    // END Friendlies

    // Hostiles

    setMutualFriends(Faction::Hostile1, Faction::Hostile2, result);

    setMutualEnemies(Faction::Hostile1, Faction::EndarSpire, result);
    setMutualEnemies(Faction::Hostile1, Faction::Insane, result);
    setMutualEnemies(Faction::Hostile1, Faction::Predator, result);
    setMutualEnemies(Faction::Hostile1, Faction::Tuskan, result);

    setMutualEnemies(Faction::Hostile2, Faction::EndarSpire, result);
    setMutualEnemies(Faction::Hostile2, Faction::Insane, result);
    setMutualEnemies(Faction::Hostile2, Faction::Predator, result);
    setMutualEnemies(Faction::Hostile2, Faction::Rancor, result);
    setMutualEnemies(Faction::Hostile2, Faction::Tuskan, result);

    // END Hostiles

    setMutualFriends(Faction::Surrender1, Faction::Surrender2, result);
    setMutualFriends(Faction::Gizka1, Faction::Gizka2, result);
    setMutualEnemies(Faction::Predator, Faction::Prey, result);

    return move(result);
}

static DispositionsMap g_dispositions = initDispositions();

static Disposition getDisposition(Faction target, Faction source) {
    if (target == source) return Disposition::Friend;

    auto maybeSourceDispositions = g_dispositions.find(source);
    if (maybeSourceDispositions == g_dispositions.end()) return Disposition::Neutral;

    auto maybeDisposition = maybeSourceDispositions->second.find(target);
    if (maybeDisposition == maybeSourceDispositions->second.end()) return Disposition::Neutral;

    return maybeDisposition->second;
}

bool getIsEnemy(const Creature &target, const Creature &source) {
    return getDisposition(target.faction(), source.faction()) == Disposition::Enemy;
}

bool getIsFriend(const Creature &target, const Creature &source) {
    return getDisposition(target.faction(), source.faction()) == Disposition::Friend;
}

bool getIsNeutral(const Creature &target, const Creature &source) {
    return getDisposition(target.faction(), source.faction()) == Disposition::Neutral;
}

} // namespace game

} // namespace reone
