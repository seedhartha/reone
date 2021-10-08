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

#include "instrutil.h"

#include "program.h"
#include "routine.h"
#include "routineprovider.h"

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

string describeInstruction(const Instruction &ins, const IRoutineProvider &routines) {
    string desc(str(boost::format("%04x") % ins.offset));

    auto maybeDesc = g_descByInstrType.find(ins.type);
    if (maybeDesc == g_descByInstrType.end()) {
        throw runtime_error(str(boost::format("Unsupported instruction type: %04x") % static_cast<int>(ins.type)));
    }

    desc += " " + maybeDesc->second;

    switch (ins.type) {
    case InstructionType::CPDOWNSP:
    case InstructionType::CPTOPSP:
    case InstructionType::CPDOWNBP:
    case InstructionType::CPTOPBP:
        desc += str(boost::format(" %d %02x") % ins.stackOffset % ins.size);
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
        desc += str(boost::format(" %s:%d %d") % routines.get(ins.routine).name() % ins.routine % ins.argCount);
        break;
    case InstructionType::MOVSP:
        desc += str(boost::format(" %d") % ins.stackOffset);
        break;
    case InstructionType::JMP:
    case InstructionType::JSR:
    case InstructionType::JZ:
    case InstructionType::JNZ:
        desc += str(boost::format(" %04x") % ins.jumpOffset);
        break;
    case InstructionType::DESTRUCT:
        desc += str(boost::format(" %02x %02x %02x") % ins.size % ins.stackOffset % ins.sizeNoDestroy);
        break;
    case InstructionType::DECISP:
    case InstructionType::INCISP:
    case InstructionType::DECIBP:
    case InstructionType::INCIBP:
        desc += str(boost::format(" %04x") % ins.stackOffset);
        break;
    case InstructionType::STORE_STATE:
        desc += str(boost::format(" %04x %04x") % ins.size % ins.sizeLocals);
        break;
    default:
        break;
    }

    return move(desc);
}

} // namespace script

} // namespace reone
