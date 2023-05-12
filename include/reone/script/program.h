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

#include "types.h"

namespace reone {

namespace script {

struct Instruction {
    uint32_t offset {0xffffffff};
    InstructionType type {InstructionType::NOP};
    uint32_t nextOffset {0xffffffff};
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

    Instruction() = default;

    Instruction(InstructionType type) :
        type(type) {
    }

    static Instruction newCPDOWNSP(int stackOffset, uint16_t size);
    static Instruction newCPTOPSP(int stackOffset, uint16_t size);
    static Instruction newCPDOWNBP(int stackOffset, uint16_t size);
    static Instruction newCPTOPBP(int stackOffset, uint16_t size);
    static Instruction newCONSTI(int value);
    static Instruction newCONSTF(float value);
    static Instruction newCONSTS(std::string value);
    static Instruction newCONSTO(int objectId);
    static Instruction newACTION(int routine, int argCount);
    static Instruction newMOVSP(int stackOffset);
    static Instruction newJMP(int jumpOffset);
    static Instruction newJSR(int jumpOffset);
    static Instruction newJZ(int jumpOffset);
    static Instruction newJNZ(int jumpOffset);
    static Instruction newDESTRUCT(uint16_t size, int stackOffset, uint16_t sizeNoDestroy);
    static Instruction newDECISP(int stackOffset);
    static Instruction newINCISP(int stackOffset);
    static Instruction newDECIBP(int stackOffset);
    static Instruction newINCIBP(int stackOffset);
    static Instruction newSTORE_STATE(uint16_t size, int sizeLocals);
    static Instruction newEQUALTT(uint16_t size);
    static Instruction newNEQUALTT(uint16_t size);
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

    uint32_t _length {13};
    std::vector<Instruction> _instructions;
    std::unordered_map<uint32_t, int> _insIdxByOffset;
};

} // namespace script

} // namespace reone
