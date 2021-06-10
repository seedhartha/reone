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

#include "../../script/scripts.h"

#include "routines.h"

namespace reone {

namespace game {

class ScriptRunner {
public:
    ScriptRunner(Routines &routines, script::Scripts &scripts);

    int run(
        const std::string &resRef,
        uint32_t callerId = script::kObjectInvalid,
        uint32_t triggerrerId = script::kObjectInvalid,
        int userDefinedEventNumber = -1,
        int scriptVar = -1);

private:
    Routines &_routines;
    script::Scripts &_scripts;
};

} // namespace game

} // namespace reone
