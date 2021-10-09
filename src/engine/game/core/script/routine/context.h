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

#pragma once

namespace reone {

namespace resource {

class Strings;

}

namespace script {

struct ExecutionContext;

}

namespace game {

class ActionFactory;
class Combat;
class EffectFactory;
class Game;
class Party;
class Reputes;
class ScriptRunner;

struct RoutineContext {
    ActionFactory &actionFactory;
    Combat &combat;
    EffectFactory &effectFactory;
    Game &game;
    Party &party;
    Reputes &reputes;
    ScriptRunner &scriptRunner;

    resource::Strings &strings;
    script::ExecutionContext &execution;

    RoutineContext(
        ActionFactory &actionFactory,
        Combat &combat,
        EffectFactory &effectFactory,
        Game &game,
        Party &party,
        Reputes &reputes,
        ScriptRunner &scriptRunner,
        resource::Strings &strings,
        script::ExecutionContext &execution) :
        actionFactory(actionFactory),
        combat(combat),
        effectFactory(effectFactory),
        game(game),
        party(party),
        reputes(reputes),
        scriptRunner(scriptRunner),
        strings(strings),
        execution(execution) {
    }
};

} // namespace game

} // namespace reone