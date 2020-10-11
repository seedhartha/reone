/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "commandaction.h"

using namespace reone::script;

namespace reone {

namespace game {

CommandAction::CommandAction(const ExecutionContext &ctx) : Action(ActionType::DoCommand), _context(ctx) {
}

const ExecutionContext &CommandAction::context() const {
    return _context;
}

} // namespace game

} // namespace reone
