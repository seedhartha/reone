/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "reone/resource/format/binreader.h"

namespace reone {

namespace script {

class ScriptProgram;

class NcsReader : public resource::BinaryResourceReader {
public:
    NcsReader(std::string resRef) :
        resource::BinaryResourceReader(boost::endian::order::big),
        _resRef(std::move(resRef)) {
    }

    void onLoad() override;

    std::shared_ptr<ScriptProgram> program() const { return _program; }

private:
    std::string _resRef;
    std::shared_ptr<ScriptProgram> _program;

    void readInstruction(size_t &offset);
};

} // namespace script

} // namespace reone