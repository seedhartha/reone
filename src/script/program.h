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

struct Instruction {
    uint32_t offset {0};
    InstructionType type {InstructionType::NOP};
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

    static Instruction newCPDOWNSP(uint32_t offset, int stackOffset, uint16_t size);
    static Instruction newCPTOPSP(uint32_t offset, int stackOffset, uint16_t size);
    static Instruction newCPDOWNBP(uint32_t offset, int stackOffset, uint16_t size);
    static Instruction newCPTOPBP(uint32_t offset, int stackOffset, uint16_t size);
    static Instruction newCONSTI(uint32_t offset, int value);
    static Instruction newCONSTF(uint32_t offset, float value);
    static Instruction newCONSTS(uint32_t offset, std::string value);
    static Instruction newCONSTO(uint32_t offset, int objectId);
    static Instruction newACTION(uint32_t offset, int routine, int argCount);
    static Instruction newMOVSP(uint32_t offset, int stackOffset);
    static Instruction newJMP(uint32_t offset, int jumpOffset);
    static Instruction newJSR(uint32_t offset, int jumpOffset);
    static Instruction newJZ(uint32_t offset, int jumpOffset);
    static Instruction newJNZ(uint32_t offset, int jumpOffset);
    static Instruction newDESTRUCT(uint32_t offset, uint16_t size, int stackOffset, uint16_t sizeNoDestroy);
    static Instruction newDECISP(uint32_t offset, int stackOffset);
    static Instruction newINCISP(uint32_t offset, int stackOffset);
    static Instruction newDECIBP(uint32_t offset, int stackOffset);
    static Instruction newINCIBP(uint32_t offset, int stackOffset);
    static Instruction newSTORE_STATE(uint32_t offset, uint16_t size, int sizeLocals);
    static Instruction newEQUALTT(uint32_t offset, uint16_t size);
    static Instruction newNEQUALTT(uint32_t offset, uint16_t size);
};

class ScriptProgram : boost::noncopyable {
public:
    ScriptProgram(std::string name) :
        _name(std::move(name)) {
    }

    void add(Instruction instr);

    const std::string &name() const { return _name; }
    uint32_t length() const { return _length; }
    const std::vector<Instruction> instructions() const { return _instructions; }

    const Instruction &getInstruction(uint32_t offset) const;

    void setLength(uint32_t length) { _length = length; }

private:
    std::string _name;

    uint32_t _length {0};
    std::vector<Instruction> _instructions;
    std::unordered_map<uint32_t, int> _insIdxByOffset;
};

} // namespace script

} // namespace reone
