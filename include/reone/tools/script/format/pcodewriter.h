/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/script/program.h"
#include "reone/system/stream/output.h"
#include "reone/system/textwriter.h"

namespace reone {

namespace script {

class IRoutines;

class PcodeWriter {
public:
    PcodeWriter(ScriptProgram &program, IRoutines &routines) :
        _program(program),
        _routines(routines) {
    }

    void save(IOutputStream &pcode);

private:
    ScriptProgram &_program;
    IRoutines &_routines;

    void writeInstruction(const Instruction &ins, TextWriter &pcode, const std::set<uint32_t> &jumpOffsets);
};

} // namespace script

} // namespace reone
