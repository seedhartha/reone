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
 *  Implementation of main routines.
 */

#include "declarations.h"

#include "../../../core/effect/factory.h"
#include "../../../core/script/routine/context.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

namespace routine {

Variable effectForceBody(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newForceBody();
    return Variable::ofEffect(move(effect));
}

Variable effectFury(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newFury();
    return Variable::ofEffect(move(effect));
}

Variable effectBlind(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newBlind();
    return Variable::ofEffect(move(effect));
}

Variable effectFPRegenModifier(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newFPRegenModifier();
    return Variable::ofEffect(move(effect));
}

Variable effectVPRegenModifier(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newVPRegenModifier();
    return Variable::ofEffect(move(effect));
}

Variable effectCrush(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newCrush();
    return Variable::ofEffect(move(effect));
}

Variable effectDroidConfused(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newDroidConfused();
    return Variable::ofEffect(move(effect));
}

Variable effectForceSight(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newForceSight();
    return Variable::ofEffect(move(effect));
}

Variable effectMindTrick(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newMindTrick();
    return Variable::ofEffect(move(effect));
}

Variable effectFactionModifier(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newFactionModifier();
    return Variable::ofEffect(move(effect));
}

Variable effectDroidScramble(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.effectFactory.newDroidScramble();
    return Variable::ofEffect(move(effect));
}

} // namespace routine

} // namespace game

} // namespace reone
