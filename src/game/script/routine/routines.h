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
#include "../../../script/routineprovider.h"
#include "../../../script/types.h"
#include "../../../script/variable.h"

#include "context.h"

namespace reone {

namespace game {

class Game;
class Services;

class IRoutines : public script::IRoutineProvider {
public:
    virtual void add(
        std::string name,
        script::VariableType retType,
        std::vector<script::VariableType> argTypes,
        script::Variable (*fn)(const std::vector<script::Variable> &args, const RoutineContext &ctx)) = 0;
};

class Routines : public IRoutines, boost::noncopyable {
public:
    Routines(Game &game, Services &services) :
        _game(game),
        _services(services) {
    }

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
                RoutineContext ctx(_game, _services, execution);
                return fn(args, std::move(ctx));
            });
    }

    int getIndexByName(const std::string &name) const override {
        for (size_t i = 0; i < _routines.size(); ++i) {
            if (_routines[i].name() == name) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    const script::Routine &get(int index) const override {
        if (isOutOfRange(_routines, index)) {
            throw std::out_of_range("index is out of range");
        }
        return _routines[index];
    }

private:
    Game &_game;
    Services &_services;

    std::vector<script::Routine> _routines;
};

} // namespace game

} // namespace reone