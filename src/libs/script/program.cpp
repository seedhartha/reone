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

#include "reone/script/program.h"

#include "reone/script/instrutil.h"

namespace reone {

namespace script {

void ScriptProgram::add(Instruction instr) {
    if (instr.offset == 0xffffffff) {
        instr.offset = _length;
    }
    auto size = getInstructionSize(instr);
    if (instr.nextOffset == 0xffffffff) {
        instr.nextOffset = instr.offset + size;
    }
    _length += size;
    _insIdxByOffset.insert(std::make_pair(instr.offset, static_cast<int>(_instructions.size())));
    _instructions.push_back(std::move(instr));
}

const Instruction &ScriptProgram::getInstruction(uint32_t offset) const {
    int idx = _insIdxByOffset.find(offset)->second;
    return _instructions[idx];
}

Instruction Instruction::newCPDOWNSP(int stackOffset, uint16_t size) {
    Instruction val;
    val.type = InstructionType::CPDOWNSP;
    val.stackOffset = stackOffset;
    val.size = size;
    return val;
}

Instruction Instruction::newCPTOPSP(int stackOffset, uint16_t size) {
    Instruction val;
    val.type = InstructionType::CPTOPSP;
    val.stackOffset = stackOffset;
    val.size = size;
    return val;
}

Instruction Instruction::newCPDOWNBP(int stackOffset, uint16_t size) {
    Instruction val;
    val.type = InstructionType::CPDOWNBP;
    val.stackOffset = stackOffset;
    val.size = size;
    return val;
}

Instruction Instruction::newCPTOPBP(int stackOffset, uint16_t size) {
    Instruction val;
    val.type = InstructionType::CPTOPBP;
    val.stackOffset = stackOffset;
    val.size = size;
    return val;
}

Instruction Instruction::newCONSTI(int value) {
    Instruction val;
    val.type = InstructionType::CONSTI;
    val.intValue = value;
    return val;
}

Instruction Instruction::newCONSTF(float value) {
    Instruction val;
    val.type = InstructionType::CONSTF;
    val.floatValue = value;
    return val;
}

Instruction Instruction::newCONSTS(std::string value) {
    Instruction val;
    val.type = InstructionType::CONSTS;
    val.strValue = std::move(value);
    return val;
}

Instruction Instruction::newCONSTO(int objectId) {
    Instruction val;
    val.type = InstructionType::CONSTO;
    val.objectId = objectId;
    return val;
}

Instruction Instruction::newACTION(int routine, int argCount) {
    Instruction val;
    val.type = InstructionType::ACTION;
    val.routine = routine;
    val.argCount = argCount;
    return val;
}

Instruction Instruction::newMOVSP(int stackOffset) {
    Instruction val;
    val.type = InstructionType::MOVSP;
    val.stackOffset = stackOffset;
    return val;
}

Instruction Instruction::newJMP(int jumpOffset) {
    Instruction val;
    val.type = InstructionType::JMP;
    val.jumpOffset = jumpOffset;
    return val;
}

Instruction Instruction::newJSR(int jumpOffset) {
    Instruction val;
    val.type = InstructionType::JSR;
    val.jumpOffset = jumpOffset;
    return val;
}

Instruction Instruction::newJZ(int jumpOffset) {
    Instruction val;
    val.type = InstructionType::JZ;
    val.jumpOffset = jumpOffset;
    return val;
}

Instruction Instruction::newJNZ(int jumpOffset) {
    Instruction val;
    val.type = InstructionType::JNZ;
    val.jumpOffset = jumpOffset;
    return val;
}

Instruction Instruction::newDESTRUCT(uint16_t size, int stackOffset, uint16_t sizeNoDestroy) {
    Instruction val;
    val.type = InstructionType::DESTRUCT;
    val.size = size;
    val.stackOffset = stackOffset;
    val.sizeNoDestroy = sizeNoDestroy;
    return val;
}

Instruction Instruction::newDECISP(int stackOffset) {
    Instruction val;
    val.type = InstructionType::DECISP;
    val.stackOffset = stackOffset;
    return val;
}

Instruction Instruction::newINCISP(int stackOffset) {
    Instruction val;
    val.type = InstructionType::INCISP;
    val.stackOffset = stackOffset;
    return val;
}

Instruction Instruction::newDECIBP(int stackOffset) {
    Instruction val;
    val.type = InstructionType::DECIBP;
    val.stackOffset = stackOffset;
    return val;
}

Instruction Instruction::newINCIBP(int stackOffset) {
    Instruction val;
    val.type = InstructionType::INCIBP;
    val.stackOffset = stackOffset;
    return val;
}

Instruction Instruction::newSTORE_STATE(uint16_t size, int sizeLocals) {
    Instruction val;
    val.type = InstructionType::STORE_STATE;
    val.size = size;
    val.sizeLocals = sizeLocals;
    return val;
}

Instruction Instruction::newEQUALTT(uint16_t size) {
    Instruction val;
    val.type = InstructionType::EQUALTT;
    val.size = size;
    return val;
}

Instruction Instruction::newNEQUALTT(uint16_t size) {
    Instruction val;
    val.type = InstructionType::NEQUALTT;
    val.size = size;
    return val;
}

} // namespace script

} // namespace reone
