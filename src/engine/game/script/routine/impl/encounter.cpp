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

/** @file
 *  Implementation of encounter-related routines.
 */

#include "declarations.h"

#include "../../../../script/exception/notimpl.h"

#include "../context.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

namespace routine {

Variable getEncounterActive(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setEncounterActive(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getEncounterSpawnsMax(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setEncounterSpawnsMax(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getEncounterSpawnsCurrent(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setEncounterSpawnsCurrent(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable setEncounterDifficulty(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable getEncounterDifficulty(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

} // namespace routine

} // namespace game

} // namespace reone
