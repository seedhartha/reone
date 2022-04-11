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

#include "../action.h"

namespace reone {

namespace script {

struct ExecutionContext;

}

namespace game {

class CommandAction : public Action {
public:
    CommandAction(Game &game, ServicesView &services, std::shared_ptr<script::ExecutionContext> context) :
        Action(game, services, ActionType::DoCommand),
        _context(move(context)) {
    }

    void execute(std::shared_ptr<Action> self, Object &actor, float dt) override;

    std::shared_ptr<script::ExecutionContext> context() const { return _context; }

private:
    std::shared_ptr<script::ExecutionContext> _context;
};

} // namespace game

} // namespace reone
