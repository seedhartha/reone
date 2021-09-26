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

#include "types.h"

namespace reone {

namespace script {

class NcsReader;

struct Instruction {
    uint32_t offset {0};
    ByteCode byteCode {ByteCode::Invalid};
    InstructionType type {InstructionType::None};
    uint32_t nextOffset {0};
    std::string strValue;

    union {
        int jumpOffset {0};
        int stackOffset;
    };

    union {
        uint16_t size {0};
        int argCount;
    };

    union {
        int routine {0};
        int intValue;
        float floatValue;
        int objectId; // used only for CONSTO
        int sizeLocals;
        int sizeNoDestroy;
    };
};

class ScriptProgram : boost::noncopyable {
public:
    ScriptProgram(const std::string &name);

    void add(Instruction instr);

    const Instruction &getInstruction(uint32_t offset) const;

    const std::string &name() const { return _name; }
    uint32_t length() const { return _length; }

    void setLength(uint32_t length);

private:
    std::string _name;
    uint32_t _length {0};
    std::unordered_map<uint32_t, Instruction> _instructions;

    friend class NcsReader;
};

} // namespace script

} // namespace reone
