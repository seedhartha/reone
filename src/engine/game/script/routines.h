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

#include "../../script/routine.h"
#include "../../script/routineprovider.h"
#include "../../script/types.h"
#include "../../script/variable.h"

namespace reone {

namespace game {

class Game;

class Routines : public script::IRoutineProvider, boost::noncopyable {
public:
    const script::Routine &get(int index) override {
        return _routines[index];
    }

    void setGame(Game &game) { _game = &game; }

    template <class T>
    void add(
        std::string name,
        script::VariableType retType,
        std::vector<script::VariableType> argTypes,
        const T &fn) {

        _routines.emplace_back(std::move(name), retType, std::move(argTypes), std::bind(fn, std::ref(*_game), std::placeholders::_1, std::placeholders::_2));
    }

private:
    Game *_game {nullptr};
    std::vector<script::Routine> _routines;
};

} // namespace game

} // namespace reone
