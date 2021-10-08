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

#define R_INSTR_TYPE_VAL(a, b) (static_cast<int>(a) | (static_cast<int>(b) << 8))

constexpr uint32_t kObjectSelf = 0;
constexpr uint32_t kObjectInvalid = 1;

enum class ByteCode {
    NOP = 0,
    CPDOWNSP = 0x01,
    RSADDx = 0x02,
    CPTOPSP = 0x03,
    CONSTx = 0x04,
    ACTION = 0x05,
    LOGANDxx = 0x06,
    LOGORxx = 0x07,
    INCORxx = 0x08,
    EXCORxx = 0x09,
    BOOLANDxx = 0x0a,
    EQUALxx = 0x0b,
    NEQUALxx = 0x0c,
    GEQxx = 0x0d,
    GTxx = 0x0e,
    LTxx = 0x0f,
    LEQxx = 0x10,
    SHLEFTxx = 0x11,
    SHRIGHTxx = 0x12,
    USHRIGHTxx = 0x13,
    ADDxx = 0x14,
    SUBxx = 0x15,
    MULxx = 0x16,
    DIVxx = 0x17,
    MODxx = 0x18,
    NEGx = 0x19,
    COMPx = 0x1a,
    MOVSP = 0x1b,
    JMP = 0x1d,
    JSR = 0x1e,
    JZ = 0x1f,
    RETN = 0x20,
    DESTRUCT = 0x21,
    NOTx = 0x22,
    DECxSP = 0x23,
    INCxSP = 0x24,
    JNZ = 0x25,
    CPDOWNBP = 0x26,
    CPTOPBP = 0x27,
    DECxBP = 0x28,
    INCxBP = 0x29,
    SAVEBP = 0x2a,
    RESTOREBP = 0x2b,
    STORE_STATE = 0x2c,
    NOP2 = 0x2d
};

enum class InstructionQualifier {
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

enum class InstructionType {
    NOP = R_INSTR_TYPE_VAL(ByteCode::NOP, 0x0c),
    CPDOWNSP = R_INSTR_TYPE_VAL(ByteCode::CPDOWNSP, 0x01),
    RSADDI = R_INSTR_TYPE_VAL(ByteCode::RSADDx, InstructionQualifier::Int),
    RSADDF = R_INSTR_TYPE_VAL(ByteCode::RSADDx, InstructionQualifier::Float),
    RSADDS = R_INSTR_TYPE_VAL(ByteCode::RSADDx, InstructionQualifier::String),
    RSADDO = R_INSTR_TYPE_VAL(ByteCode::RSADDx, InstructionQualifier::Object),
    RSADDEFF = R_INSTR_TYPE_VAL(ByteCode::RSADDx, InstructionQualifier::Effect),
    RSADDEVT = R_INSTR_TYPE_VAL(ByteCode::RSADDx, InstructionQualifier::Event),
    RSADDLOC = R_INSTR_TYPE_VAL(ByteCode::RSADDx, InstructionQualifier::Location),
    RSADDTAL = R_INSTR_TYPE_VAL(ByteCode::RSADDx, InstructionQualifier::Talent),
    CPTOPSP = R_INSTR_TYPE_VAL(ByteCode::CPTOPSP, 0x01),
    CONSTI = R_INSTR_TYPE_VAL(ByteCode::CONSTx, InstructionQualifier::Int),
    CONSTF = R_INSTR_TYPE_VAL(ByteCode::CONSTx, InstructionQualifier::Float),
    CONSTS = R_INSTR_TYPE_VAL(ByteCode::CONSTx, InstructionQualifier::String),
    CONSTO = R_INSTR_TYPE_VAL(ByteCode::CONSTx, InstructionQualifier::Object),
    ACTION = R_INSTR_TYPE_VAL(ByteCode::ACTION, 0x00),
    LOGANDII = R_INSTR_TYPE_VAL(ByteCode::LOGANDxx, InstructionQualifier::IntInt),
    LOGORII = R_INSTR_TYPE_VAL(ByteCode::LOGORxx, InstructionQualifier::IntInt),
    INCORII = R_INSTR_TYPE_VAL(ByteCode::INCORxx, InstructionQualifier::IntInt),
    EXCORII = R_INSTR_TYPE_VAL(ByteCode::EXCORxx, InstructionQualifier::IntInt),
    BOOLANDII = R_INSTR_TYPE_VAL(ByteCode::BOOLANDxx, InstructionQualifier::IntInt),
    EQUALII = R_INSTR_TYPE_VAL(ByteCode::EQUALxx, InstructionQualifier::IntInt),
    EQUALFF = R_INSTR_TYPE_VAL(ByteCode::EQUALxx, InstructionQualifier::FloatFloat),
    EQUALSS = R_INSTR_TYPE_VAL(ByteCode::EQUALxx, InstructionQualifier::StringString),
    EQUALOO = R_INSTR_TYPE_VAL(ByteCode::EQUALxx, InstructionQualifier::ObjectObject),
    EQUALTT = R_INSTR_TYPE_VAL(ByteCode::EQUALxx, InstructionQualifier::StructStruct),
    EQUALEFFEFF = R_INSTR_TYPE_VAL(ByteCode::EQUALxx, InstructionQualifier::EffectEffect),
    EQUALEVTEVT = R_INSTR_TYPE_VAL(ByteCode::EQUALxx, InstructionQualifier::EventEvent),
    EQUALLOCLOC = R_INSTR_TYPE_VAL(ByteCode::EQUALxx, InstructionQualifier::LocationLocation),
    EQUALTALTAL = R_INSTR_TYPE_VAL(ByteCode::EQUALxx, InstructionQualifier::TalentTalent),
    NEQUALII = R_INSTR_TYPE_VAL(ByteCode::NEQUALxx, InstructionQualifier::IntInt),
    NEQUALFF = R_INSTR_TYPE_VAL(ByteCode::NEQUALxx, InstructionQualifier::FloatFloat),
    NEQUALSS = R_INSTR_TYPE_VAL(ByteCode::NEQUALxx, InstructionQualifier::StringString),
    NEQUALOO = R_INSTR_TYPE_VAL(ByteCode::NEQUALxx, InstructionQualifier::ObjectObject),
    NEQUALTT = R_INSTR_TYPE_VAL(ByteCode::NEQUALxx, InstructionQualifier::StructStruct),
    NEQUALEFFEFF = R_INSTR_TYPE_VAL(ByteCode::NEQUALxx, InstructionQualifier::EffectEffect),
    NEQUALEVTEVT = R_INSTR_TYPE_VAL(ByteCode::NEQUALxx, InstructionQualifier::EventEvent),
    NEQUALLOCLOC = R_INSTR_TYPE_VAL(ByteCode::NEQUALxx, InstructionQualifier::LocationLocation),
    NEQUALTALTAL = R_INSTR_TYPE_VAL(ByteCode::NEQUALxx, InstructionQualifier::TalentTalent),
    GEQII = R_INSTR_TYPE_VAL(ByteCode::GEQxx, InstructionQualifier::IntInt),
    GEQFF = R_INSTR_TYPE_VAL(ByteCode::GEQxx, InstructionQualifier::FloatFloat),
    GTII = R_INSTR_TYPE_VAL(ByteCode::GTxx, InstructionQualifier::IntInt),
    GTFF = R_INSTR_TYPE_VAL(ByteCode::GTxx, InstructionQualifier::FloatFloat),
    LTII = R_INSTR_TYPE_VAL(ByteCode::LTxx, InstructionQualifier::IntInt),
    LTFF = R_INSTR_TYPE_VAL(ByteCode::LTxx, InstructionQualifier::FloatFloat),
    LEQII = R_INSTR_TYPE_VAL(ByteCode::LEQxx, InstructionQualifier::IntInt),
    LEQFF = R_INSTR_TYPE_VAL(ByteCode::LEQxx, InstructionQualifier::FloatFloat),
    SHLEFTII = R_INSTR_TYPE_VAL(ByteCode::SHLEFTxx, InstructionQualifier::IntInt),
    SHRIGHTII = R_INSTR_TYPE_VAL(ByteCode::SHRIGHTxx, InstructionQualifier::IntInt),
    USHRIGHTII = R_INSTR_TYPE_VAL(ByteCode::USHRIGHTxx, InstructionQualifier::IntInt),
    ADDII = R_INSTR_TYPE_VAL(ByteCode::ADDxx, InstructionQualifier::IntInt),
    ADDIF = R_INSTR_TYPE_VAL(ByteCode::ADDxx, InstructionQualifier::IntFloat),
    ADDFI = R_INSTR_TYPE_VAL(ByteCode::ADDxx, InstructionQualifier::FloatInt),
    ADDFF = R_INSTR_TYPE_VAL(ByteCode::ADDxx, InstructionQualifier::FloatFloat),
    ADDSS = R_INSTR_TYPE_VAL(ByteCode::ADDxx, InstructionQualifier::StringString),
    ADDVV = R_INSTR_TYPE_VAL(ByteCode::ADDxx, InstructionQualifier::VectorVector),
    SUBII = R_INSTR_TYPE_VAL(ByteCode::SUBxx, InstructionQualifier::IntInt),
    SUBIF = R_INSTR_TYPE_VAL(ByteCode::SUBxx, InstructionQualifier::IntFloat),
    SUBFI = R_INSTR_TYPE_VAL(ByteCode::SUBxx, InstructionQualifier::FloatInt),
    SUBFF = R_INSTR_TYPE_VAL(ByteCode::SUBxx, InstructionQualifier::FloatFloat),
    SUBVV = R_INSTR_TYPE_VAL(ByteCode::SUBxx, InstructionQualifier::VectorVector),
    MULII = R_INSTR_TYPE_VAL(ByteCode::MULxx, InstructionQualifier::IntInt),
    MULIF = R_INSTR_TYPE_VAL(ByteCode::MULxx, InstructionQualifier::IntFloat),
    MULFI = R_INSTR_TYPE_VAL(ByteCode::MULxx, InstructionQualifier::FloatInt),
    MULFF = R_INSTR_TYPE_VAL(ByteCode::MULxx, InstructionQualifier::FloatFloat),
    MULVF = R_INSTR_TYPE_VAL(ByteCode::MULxx, InstructionQualifier::VectorFloat),
    MULFV = R_INSTR_TYPE_VAL(ByteCode::MULxx, InstructionQualifier::FloatVector),
    DIVII = R_INSTR_TYPE_VAL(ByteCode::DIVxx, InstructionQualifier::IntInt),
    DIVIF = R_INSTR_TYPE_VAL(ByteCode::DIVxx, InstructionQualifier::IntFloat),
    DIVFI = R_INSTR_TYPE_VAL(ByteCode::DIVxx, InstructionQualifier::FloatInt),
    DIVFF = R_INSTR_TYPE_VAL(ByteCode::DIVxx, InstructionQualifier::FloatFloat),
    DIVVF = R_INSTR_TYPE_VAL(ByteCode::DIVxx, InstructionQualifier::VectorFloat),
    DIVFV = R_INSTR_TYPE_VAL(ByteCode::DIVxx, InstructionQualifier::FloatVector),
    MODII = R_INSTR_TYPE_VAL(ByteCode::MODxx, InstructionQualifier::IntInt),
    NEGI = R_INSTR_TYPE_VAL(ByteCode::NEGx, InstructionQualifier::Int),
    NEGF = R_INSTR_TYPE_VAL(ByteCode::NEGx, InstructionQualifier::Float),
    COMPI = R_INSTR_TYPE_VAL(ByteCode::COMPx, InstructionQualifier::Int),
    MOVSP = R_INSTR_TYPE_VAL(ByteCode::MOVSP, 0x00),
    JMP = R_INSTR_TYPE_VAL(ByteCode::JMP, 0x00),
    JSR = R_INSTR_TYPE_VAL(ByteCode::JSR, 0x00),
    JZ = R_INSTR_TYPE_VAL(ByteCode::JZ, 0x00),
    RETN = R_INSTR_TYPE_VAL(ByteCode::RETN, 0x00),
    DESTRUCT = R_INSTR_TYPE_VAL(ByteCode::DESTRUCT, 0x01),
    NOTI = R_INSTR_TYPE_VAL(ByteCode::NOTx, InstructionQualifier::Int),
    DECISP = R_INSTR_TYPE_VAL(ByteCode::DECxSP, InstructionQualifier::Int),
    INCISP = R_INSTR_TYPE_VAL(ByteCode::INCxSP, InstructionQualifier::Int),
    JNZ = R_INSTR_TYPE_VAL(ByteCode::JNZ, 0x00),
    CPDOWNBP = R_INSTR_TYPE_VAL(ByteCode::CPDOWNBP, 0x01),
    CPTOPBP = R_INSTR_TYPE_VAL(ByteCode::CPTOPBP, 0x01),
    DECIBP = R_INSTR_TYPE_VAL(ByteCode::DECxBP, InstructionQualifier::Int),
    INCIBP = R_INSTR_TYPE_VAL(ByteCode::INCxBP, InstructionQualifier::Int),
    SAVEBP = R_INSTR_TYPE_VAL(ByteCode::SAVEBP, 0x00),
    RESTOREBP = R_INSTR_TYPE_VAL(ByteCode::RESTOREBP, 0x00),
    STORE_STATE = R_INSTR_TYPE_VAL(ByteCode::STORE_STATE, 0x10),
    NOP2 = R_INSTR_TYPE_VAL(ByteCode::NOP2, 0x00)
};

#define R_INSTR_TYPE(a, b) static_cast<InstructionType>(R_INSTR_TYPE_VAL(a, b))

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
    Action
};

} // namespace script

} // namespace reone
