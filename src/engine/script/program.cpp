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

#include "program.h"

using namespace std;

namespace reone {

namespace script {

ScriptProgram::ScriptProgram(const string &name) : _name(name) {
}

void ScriptProgram::add(Instruction instr) {
    _instructions.insert(make_pair(instr.offset, instr));
}

const Instruction &ScriptProgram::getInstruction(uint32_t offset) const {
    return _instructions.find(offset)->second;
}

void ScriptProgram::setLength(uint32_t length) {
    _length = length;
}

} // namespace script

} // namespace reone
