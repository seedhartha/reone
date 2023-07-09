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

namespace reone {

namespace script {

class IRoutines;

class PcodeReader {
public:
    PcodeReader(std::filesystem::path path, IRoutines &routines) :
        _path(std::move(path)),
        _routines(routines) {
    }

    void load();

    std::shared_ptr<ScriptProgram> program() { return _program; }

private:
    std::filesystem::path _path;
    IRoutines &_routines;

    std::shared_ptr<ScriptProgram> _program;
    std::map<std::string, uint32_t> _addrByLabel;

    int getInstructionSize(const std::string &line);

    Instruction parseInstruction(const std::string &line, uint32_t addr) const;

    void applyArguments(const std::string &line, const std::string &restr, int numArgs, const std::function<void(const std::vector<std::string> &)> &fn) const;
};

} // namespace script

} // namespace reone
