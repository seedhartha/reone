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

#include "reone/script/instrutil.h"

#include "reone/system/collectionutil.h"

#include "reone/script/program.h"
#include "reone/script/routine.h"
#include "reone/script/routines.h"

using namespace std;

namespace reone {

namespace script {

static unordered_map<InstructionType, string> g_descByInstrType {
    {InstructionType::NOP, "NOP"},
    {InstructionType::CPDOWNSP, "CPDOWNSP"},
    {InstructionType::RSADDI, "RSADDI"},
    {InstructionType::RSADDF, "RSADDF"},
    {InstructionType::RSADDS, "RSADDS"},
    {InstructionType::RSADDO, "RSADDO"},
    {InstructionType::RSADDEFF, "RSADDEFF"},
    {InstructionType::RSADDEVT, "RSADDEVT"},
    {InstructionType::RSADDLOC, "RSADDLOC"},
    {InstructionType::RSADDTAL, "RSADDTAL"},
    {InstructionType::CPTOPSP, "CPTOPSP"},
    {InstructionType::CONSTI, "CONSTI"},
    {InstructionType::CONSTF, "CONSTF"},
    {InstructionType::CONSTS, "CONSTS"},
    {InstructionType::CONSTO, "CONSTO"},
    {InstructionType::ACTION, "ACTION"},
    {InstructionType::LOGANDII, "LOGANDII"},
    {InstructionType::LOGORII, "LOGORII"},
    {InstructionType::INCORII, "INCORII"},
    {InstructionType::EXCORII, "EXCORII"},
    {InstructionType::BOOLANDII, "BOOLANDII"},
    {InstructionType::EQUALII, "EQUALII"},
    {InstructionType::EQUALFF, "EQUALFF"},
    {InstructionType::EQUALSS, "EQUALSS"},
    {InstructionType::EQUALOO, "EQUALOO"},
    {InstructionType::EQUALTT, "EQUALTT"},
    {InstructionType::EQUALEFFEFF, "EQUALEFFEFF"},
    {InstructionType::EQUALEVTEVT, "EQUALEVTEVT"},
    {InstructionType::EQUALLOCLOC, "EQUALLOCLOC"},
    {InstructionType::EQUALTALTAL, "EQUALTALTAL"},
    {InstructionType::NEQUALII, "NEQUALII"},
    {InstructionType::NEQUALFF, "NEQUALFF"},
    {InstructionType::NEQUALSS, "NEQUALSS"},
    {InstructionType::NEQUALOO, "NEQUALOO"},
    {InstructionType::NEQUALTT, "NEQUALTT"},
    {InstructionType::NEQUALEFFEFF, "NEQUALEFFEFF"},
    {InstructionType::NEQUALEVTEVT, "NEQUALEVTEVT"},
    {InstructionType::NEQUALLOCLOC, "NEQUALLOCLOC"},
    {InstructionType::NEQUALTALTAL, "NEQUALTALTAL"},
    {InstructionType::GEQII, "GEQII"},
    {InstructionType::GEQFF, "GEQFF"},
    {InstructionType::GTII, "GTII"},
    {InstructionType::GTFF, "GTFF"},
    {InstructionType::LTII, "LTII"},
    {InstructionType::LTFF, "LTFF"},
    {InstructionType::LEQII, "LEQII"},
    {InstructionType::LEQFF, "LEQFF"},
    {InstructionType::SHLEFTII, "SHLEFTII"},
    {InstructionType::SHRIGHTII, "SHRIGHTII"},
    {InstructionType::USHRIGHTII, "USHRIGHTII"},
    {InstructionType::ADDII, "ADDII"},
    {InstructionType::ADDIF, "ADDIF"},
    {InstructionType::ADDFI, "ADDFI"},
    {InstructionType::ADDFF, "ADDFF"},
    {InstructionType::ADDSS, "ADDSS"},
    {InstructionType::ADDVV, "ADDVV"},
    {InstructionType::SUBII, "SUBII"},
    {InstructionType::SUBIF, "SUBIF"},
    {InstructionType::SUBFI, "SUBFI"},
    {InstructionType::SUBFF, "SUBFF"},
    {InstructionType::SUBVV, "SUBVV"},
    {InstructionType::MULII, "MULII"},
    {InstructionType::MULIF, "MULIF"},
    {InstructionType::MULFI, "MULFI"},
    {InstructionType::MULFF, "MULFF"},
    {InstructionType::MULVF, "MULVF"},
    {InstructionType::MULFV, "MULFV"},
    {InstructionType::DIVII, "DIVII"},
    {InstructionType::DIVIF, "DIVIF"},
    {InstructionType::DIVFI, "DIVFI"},
    {InstructionType::DIVFF, "DIVFF"},
    {InstructionType::DIVVF, "DIVVF"},
    {InstructionType::DIVFV, "DIVFV"},
    {InstructionType::MODII, "MODII"},
    {InstructionType::NEGI, "NEGI"},
    {InstructionType::NEGF, "NEGF"},
    {InstructionType::COMPI, "COMPI"},
    {InstructionType::MOVSP, "MOVSP"},
    {InstructionType::JMP, "JMP"},
    {InstructionType::JSR, "JSR"},
    {InstructionType::JZ, "JZ"},
    {InstructionType::RETN, "RETN"},
    {InstructionType::DESTRUCT, "DESTRUCT"},
    {InstructionType::NOTI, "NOTI"},
    {InstructionType::DECISP, "DECISP"},
    {InstructionType::INCISP, "INCISP"},
    {InstructionType::JNZ, "JNZ"},
    {InstructionType::CPDOWNBP, "CPDOWNBP"},
    {InstructionType::CPTOPBP, "CPTOPBP"},
    {InstructionType::DECIBP, "DECIBP"},
    {InstructionType::INCIBP, "INCIBP"},
    {InstructionType::SAVEBP, "SAVEBP"},
    {InstructionType::RESTOREBP, "RESTOREBP"},
    {InstructionType::STORE_STATE, "STORE_STATE"},
    {InstructionType::NOP2, "NOP2"}};

static map<string, InstructionType> g_instrTypeByDesc = associate<pair<InstructionType, string>, string, InstructionType>(
    mapToEntries(g_descByInstrType),
    [](auto &pair) { return pair.second; },
    [](auto &pair) { return pair.first; });

string describeInstruction(const Instruction &ins, IRoutines &routines) {
    string desc(str(boost::format("%08x %s") % ins.offset % describeInstructionType(ins.type)));

    switch (ins.type) {
    case InstructionType::CPDOWNSP:
    case InstructionType::CPTOPSP:
    case InstructionType::CPDOWNBP:
    case InstructionType::CPTOPBP:
        desc += str(boost::format(" %d, %d") % ins.stackOffset % ins.size);
        break;
    case InstructionType::CONSTI:
        desc += " " + to_string(ins.intValue);
        break;
    case InstructionType::CONSTF:
        desc += " " + to_string(ins.floatValue);
        break;
    case InstructionType::CONSTS:
        desc += " \"" + ins.strValue + "\"";
        break;
    case InstructionType::CONSTO:
        desc += " " + to_string(ins.objectId);
        break;
    case InstructionType::ACTION:
        desc += str(boost::format(" %s(%d), %d") % routines.get(ins.routine).name() % ins.routine % ins.argCount);
        break;
    case InstructionType::EQUALTT:
    case InstructionType::NEQUALTT:
        desc += " " + to_string(ins.size);
        break;
    case InstructionType::MOVSP:
        desc += " " + to_string(ins.stackOffset);
        break;
    case InstructionType::JMP:
    case InstructionType::JSR:
    case InstructionType::JZ:
    case InstructionType::JNZ: {
        uint32_t jumpAddr = ins.offset + ins.jumpOffset;
        desc += str(boost::format(" %08x(%d)") % jumpAddr % ins.jumpOffset);
        break;
    }
    case InstructionType::DESTRUCT:
        desc += str(boost::format(" %d, %d, %d") % ins.size % ins.stackOffset % ins.sizeNoDestroy);
        break;
    case InstructionType::DECISP:
    case InstructionType::INCISP:
    case InstructionType::DECIBP:
    case InstructionType::INCIBP:
        desc += " " + to_string(ins.stackOffset);
        break;
    case InstructionType::STORE_STATE:
        desc += str(boost::format(" %d, %d") % ins.size % ins.sizeLocals);
        break;
    default:
        break;
    }

    return move(desc);
}

const string &describeInstructionType(InstructionType type) {
    auto maybeDesc = g_descByInstrType.find(type);
    if (maybeDesc == g_descByInstrType.end()) {
        throw invalid_argument("Unsupported instruction type: " + to_string(static_cast<int>(type)));
    }
    return maybeDesc->second;
}

InstructionType parseInstructionType(const string &desc) {
    auto maybeInstrType = g_instrTypeByDesc.find(desc);
    if (maybeInstrType == g_instrTypeByDesc.end()) {
        throw invalid_argument("Unrecognized instruction type: " + desc);
    }
    return maybeInstrType->second;
}

int getInstructionSize(const Instruction &ins) {
    int result = 2;
    switch (ins.type) {
    case InstructionType::CPDOWNSP:
    case InstructionType::CPTOPSP:
    case InstructionType::CPDOWNBP:
    case InstructionType::CPTOPBP:
    case InstructionType::DESTRUCT:
        result += 6;
        break;
    case InstructionType::CONSTI:
    case InstructionType::CONSTF:
    case InstructionType::CONSTO:
    case InstructionType::MOVSP:
    case InstructionType::JMP:
    case InstructionType::JSR:
    case InstructionType::JZ:
    case InstructionType::JNZ:
    case InstructionType::DECISP:
    case InstructionType::INCISP:
    case InstructionType::DECIBP:
    case InstructionType::INCIBP:
        result += 4;
        break;
    case InstructionType::CONSTS:
        result += 2 + ins.strValue.length();
        break;
    case InstructionType::ACTION:
        result += 3;
        break;
    case InstructionType::STORE_STATE:
        result += 8;
        break;
    case InstructionType::EQUALTT:
    case InstructionType::NEQUALTT:
        result += 2;
        break;
    default:
        break;
    };
    return result;
}

} // namespace script

} // namespace reone
