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

#include "reone/system/stream/output.h"

namespace reone {

namespace script {

class ScriptProgram;

}

namespace resource {

class NcsWriter {
public:
    NcsWriter(script::ScriptProgram &program) :
        _program(program) {
    }

    void save(const std::filesystem::path &path);
    void save(std::shared_ptr<IOutputStream> out);

private:
    script::ScriptProgram &_program;
};

} // namespace resource

} // namespace reone
