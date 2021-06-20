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

namespace reone {

namespace script {

constexpr uint32_t kObjectSelf = 0;
constexpr uint32_t kObjectInvalid = 1;

enum class ByteCode {
    CopyDownSP = 0x01,
    Reserve = 0x02,
    CopyTopSP = 0x03,
    PushConstant = 0x04,
    CallRoutine = 0x05,
    LogicalAnd = 0x06,
    LogicalOr = 0x07,
    InclusiveBitwiseOr = 0x08,
    ExclusiveBitwiseOr = 0x09,
    BitwiseAnd = 0x0a,
    Equal = 0x0b,
    NotEqual = 0x0c,
    GreaterThanOrEqual = 0x0d,
    GreaterThan = 0x0e,
    LessThan = 0x0f,
    LessThanOrEqual = 0x10,
    ShiftLeft = 0x11,
    ShiftRight = 0x12,
    UnsignedShiftRight = 0x13,
    Add = 0x14,
    Subtract = 0x15,
    Multiply = 0x16,
    Divide = 0x17,
    Mod = 0x18,
    Negate = 0x19,
    OnesComplement = 0x1a,
    AdjustSP = 0x1b,
    Jump = 0x1d,
    JumpToSubroutine = 0x1e,
    JumpIfZero = 0x1f,
    Return = 0x20,
    Destruct = 0x21,
    LogicalNot = 0x22,
    DecRelToSP = 0x23,
    IncRelToSP = 0x24,
    JumpIfNonZero = 0x25,
    CopyDownBP = 0x26,
    CopyTopBP = 0x27,
    DecRelToBP = 0x28,
    IncRelToBP = 0x29,
    SaveBP = 0x2a,
    RestoreBP = 0x2b,
    StoreState = 0x2c,
    Noop = 0x2d,
    Invalid = 0xff
};

enum class InstructionType {
    None = 0,
    One = 0x01,
    Int = 0x03,
    Float = 0x04,
    String = 0x05,
    Object = 0x06,
    Effect = 0x10,
    Event = 0x11,
    Location = 0x12,
    Talent = 0x13,
    IntInt = 0x20,
    FloatFloat = 0x21,
    ObjectObject = 0x22,
    StringString = 0x23,
    StructStruct = 0x24,
    IntFloat = 0x25,
    FloatInt = 0x26,
    EffectEffect = 0x30,
    EventEvent = 0x31,
    LocationLocation = 0x32,
    TalentTalent = 0x33,
    VectorVector = 0x3a,
    VectorFloat = 0x3b,
    FloatVector = 0x3c
};

enum class VariableType {
    Void,
    Int,
    Float,
    String,
    Vector,
    Object,
    Effect,
    Event,
    Location,
    Talent,
    Action,

    NotImplemented // used to return default values from placeholder routines
};

} // namespace script

} // namespace reone
