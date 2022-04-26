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

void ScriptProgram::add(Instruction instr) {
    _instructions.push_back(move(instr));
    _insIdxByOffset.insert(make_pair(instr.offset, static_cast<int>(_instructions.size() - 1)));
}

const Instruction &ScriptProgram::getInstruction(uint32_t offset) const {
    int idx = _insIdxByOffset.find(offset)->second;
    return _instructions[idx];
}

Instruction Instruction::newCPDOWNSP(uint32_t offset, int stackOffset, uint16_t size) {
    Instruction val;
    val.offset = offset;
    val.type = InstructionType::CPDOWNSP;
    val.stackOffset = stackOffset;
    val.size = size;
    return move(val);
}

Instruction Instruction::newCPTOPSP(uint32_t offset, int stackOffset, uint16_t size) {
    Instruction val;
    val.offset = offset;
    val.type = InstructionType::CPTOPSP;
    val.stackOffset = stackOffset;
    val.size = size;
    return move(val);
}

Instruction Instruction::newCPDOWNBP(uint32_t offset, int stackOffset, uint16_t size) {
    Instruction val;
    val.offset = offset;
    val.type = InstructionType::CPDOWNBP;
    val.stackOffset = stackOffset;
    val.size = size;
    return move(val);
}

Instruction Instruction::newCPTOPBP(uint32_t offset, int stackOffset, uint16_t size) {
    Instruction val;
    val.offset = offset;
    val.type = InstructionType::CPTOPBP;
    val.stackOffset = stackOffset;
    val.size = size;
    return move(val);
}

Instruction Instruction::newCONSTI(uint32_t offset, int value) {
    Instruction val;
    val.offset = offset;
    val.type = InstructionType::CONSTI;
    val.intValue = value;
    return move(val);
}

Instruction Instruction::newCONSTF(uint32_t offset, float value) {
    Instruction val;
    val.offset = offset;
    val.type = InstructionType::CONSTF;
    val.floatValue = value;
    return move(val);
}

Instruction Instruction::newCONSTS(uint32_t offset, string value) {
    Instruction val;
    val.offset = offset;
    val.type = InstructionType::CONSTS;
    val.strValue = move(value);
    return move(val);
}

Instruction Instruction::newCONSTO(uint32_t offset, int objectId) {
    Instruction val;
    val.offset = offset;
    val.type = InstructionType::CONSTO;
    val.objectId = objectId;
    return move(val);
}

Instruction Instruction::newACTION(uint32_t offset, int routine, int argCount) {
    Instruction val;
    val.offset = offset;
    val.type = InstructionType::ACTION;
    val.routine = routine;
    val.argCount = argCount;
    return move(val);
}

Instruction Instruction::newMOVSP(uint32_t offset, int stackOffset) {
    Instruction val;
    val.offset = offset;
    val.type = InstructionType::MOVSP;
    val.stackOffset = stackOffset;
    return move(val);
}

Instruction Instruction::newJMP(uint32_t offset, int jumpOffset) {
    Instruction val;
    val.offset = offset;
    val.type = InstructionType::JMP;
    val.jumpOffset = jumpOffset;
    return move(val);
}

Instruction Instruction::newJSR(uint32_t offset, int jumpOffset) {
    Instruction val;
    val.offset = offset;
    val.type = InstructionType::JSR;
    val.jumpOffset = jumpOffset;
    return move(val);
}

Instruction Instruction::newJZ(uint32_t offset, int jumpOffset) {
    Instruction val;
    val.offset = offset;
    val.type = InstructionType::JZ;
    val.jumpOffset = jumpOffset;
    return move(val);
}

Instruction Instruction::newJNZ(uint32_t offset, int jumpOffset) {
    Instruction val;
    val.offset = offset;
    val.type = InstructionType::JNZ;
    val.jumpOffset = jumpOffset;
    return move(val);
}

Instruction Instruction::newDESTRUCT(uint32_t offset, uint16_t size, int stackOffset, uint16_t sizeNoDestroy) {
    Instruction val;
    val.offset = offset;
    val.type = InstructionType::DESTRUCT;
    val.size = size;
    val.stackOffset = stackOffset;
    val.sizeNoDestroy = sizeNoDestroy;
    return move(val);
}

Instruction Instruction::newDECISP(uint32_t offset, int stackOffset) {
    Instruction val;
    val.offset = offset;
    val.type = InstructionType::DECISP;
    val.stackOffset = stackOffset;
    return move(val);
}

Instruction Instruction::newINCISP(uint32_t offset, int stackOffset) {
    Instruction val;
    val.offset = offset;
    val.type = InstructionType::INCISP;
    val.stackOffset = stackOffset;
    return move(val);
}

Instruction Instruction::newDECIBP(uint32_t offset, int stackOffset) {
    Instruction val;
    val.offset = offset;
    val.type = InstructionType::DECIBP;
    val.stackOffset = stackOffset;
    return move(val);
}

Instruction Instruction::newINCIBP(uint32_t offset, int stackOffset) {
    Instruction val;
    val.offset = offset;
    val.type = InstructionType::INCIBP;
    val.stackOffset = stackOffset;
    return move(val);
}

Instruction Instruction::newSTORE_STATE(uint32_t offset, uint16_t size, int sizeLocals) {
    Instruction val;
    val.offset = offset;
    val.type = InstructionType::STORE_STATE;
    val.size = size;
    val.sizeLocals = sizeLocals;
    return move(val);
}

Instruction Instruction::newEQUALTT(uint32_t offset, uint16_t size) {
    Instruction val;
    val.offset = offset;
    val.type = InstructionType::EQUALTT;
    val.size = size;
    return move(val);
}

Instruction Instruction::newNEQUALTT(uint32_t offset, uint16_t size) {
    Instruction val;
    val.offset = offset;
    val.type = InstructionType::NEQUALTT;
    val.size = size;
    return move(val);
}

} // namespace script

} // namespace reone
