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

#include "../../common/timer.h"

#include "action.h"

namespace reone {

namespace game {

class WaitAction : public Action {
public:
    WaitAction(Game &game, float seconds) : Action(game, ActionType::Wait) {
        _timer.setTimeout(seconds);
    }

    /**
     * Advances an internal timer.
     *
     * @return `true` if timer times out, `false` otherwise
     */
    bool advance(float dt) {
        return _timer.advance(dt);
    }

    void execute(Object &actor, float dt) override;

private:
    Timer _timer;
};

} // namespace game

} // namespace reone
