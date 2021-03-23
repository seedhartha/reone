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

#include "routines.h"

#include "../../common/log.h"

#include "../object/creature.h"
#include "../types.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

Variable Routines::getNPCAIStyle(const VariablesList &args, ExecutionContext &ctx) {
    Variable result;
    result.type = VariableType::Int;

    auto creature = getCreature(args, 0);
    if (creature) {
        result.intValue = static_cast<int>(creature->aiStyle());
    } else {
        warn("Script: getNPCAIStyle: creature is invalid");
    }

    return move(result);
}

Variable Routines::setNPCAIStyle(const VariablesList &args, ExecutionContext &ctx) {
    auto creature = getCreature(args, 0);
    if (creature) {
        auto style = static_cast<NPCAIStyle>(getInt(args, 1));
        // TODO: currently we don't want that, because the default style is the only one that works
        //creature->setAIStyle(style);
    } else {
        warn("Script: setNPCAIStyle: creature is invalid");
    }

    return Variable();
}

} // namespace game

} // namespace reone
