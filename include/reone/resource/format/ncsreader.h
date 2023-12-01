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

#include "reone/system/binaryreader.h"
#include "reone/system/stream/input.h"

namespace reone {

namespace script {

class ScriptProgram;

}

namespace resource {

class NcsReader : boost::noncopyable {
public:
    NcsReader(IInputStream &ncs, std::string resRef) :
        _ncs(BinaryReader(ncs, boost::endian::order::big)),
        _resRef(std::move(resRef)) {
    }

    void load();

    std::shared_ptr<script::ScriptProgram> program() const { return _program; }

private:
    BinaryReader _ncs;

    std::string _resRef;
    std::shared_ptr<script::ScriptProgram> _program;

    void readInstruction(size_t &offset);
};

} // namespace resource

} // namespace reone
