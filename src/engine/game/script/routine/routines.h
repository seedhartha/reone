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

#include "../../../common/collectionutil.h"
#include "../../../script/routine.h"
#include "../../../script/variable.h"

#include "context.h"
#include "iroutines.h"

namespace reone {

namespace resource {

class Strings;

}

namespace game {

class ActionFactory;
class Combat;
class EffectFactory;
class Game;
class Party;
class Reputes;
class ScriptRunner;

class Routines : public IRoutines, boost::noncopyable {
public:
    Routines(
        ActionFactory &actionFactory,
        Combat &combat,
        EffectFactory &effectFactory,
        Party &party,
        Reputes &reputes,
        ScriptRunner &scriptRunner,
        resource::Strings &strings) :
        _actionFactory(actionFactory),
        _combat(combat),
        _effectFactory(effectFactory),
        _party(party),
        _reputes(reputes),
        _scriptRunner(scriptRunner),
        _strings(strings) {
    }

    const script::Routine &get(int index) const override {
        if (isOutOfRange(_routines, index)) {
            throw std::out_of_range("index is out of range");
        }
        return _routines[index];
    }

    void setGame(Game &game) { _game = &game; }

    void add(
        std::string name,
        script::VariableType retType,
        std::vector<script::VariableType> argTypes,
        script::Variable (*fn)(const std::vector<script::Variable> &args, const RoutineContext &ctx)) override {

        _routines.emplace_back(
            std::move(name),
            retType,
            std::move(argTypes),
            [this, fn](auto &args, auto &execution) {
                RoutineContext ctx(
                    _actionFactory,
                    _combat,
                    _effectFactory,
                    *_game,
                    _party,
                    _reputes,
                    _scriptRunner,
                    _strings,
                    execution);

                return fn(args, std::move(ctx));
            });
    }

private:
    std::vector<script::Routine> _routines;

    // Services

    ActionFactory &_actionFactory;
    Combat &_combat;
    EffectFactory &_effectFactory;
    Game *_game;
    Party &_party;
    Reputes &_reputes;
    ScriptRunner &_scriptRunner;

    resource::Strings &_strings;

    // END Services
};

} // namespace game

} // namespace reone
