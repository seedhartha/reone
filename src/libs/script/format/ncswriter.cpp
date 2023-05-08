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

#include "reone/script/format/ncswriter.h"

#include "reone/common/binarywriter.h"
#include "reone/common/collectionutil.h"
#include "reone/common/exception/validation.h"
#include "reone/common/stream/bytearrayoutput.h"
#include "reone/common/stream/fileoutput.h"

#include "reone/script/program.h"

using namespace std;

namespace endian = boost::endian;
namespace fs = boost::filesystem;

namespace reone {

namespace script {

void NcsWriter::save(const fs::path &path) {
    auto ncs = make_shared<FileOutputStream>(path, OpenMode::Binary);
    save(ncs);
}

void NcsWriter::save(std::shared_ptr<IOutputStream> out) {
    auto bytes = ByteArray();
    auto stream = ByteArrayOutputStream(bytes);
    BinaryWriter writer(stream, endian::order::big);

    for (auto &ins : _program.instructions()) {
        auto pos = 13 + static_cast<uint32_t>(writer.tell());
        if (ins.offset != pos) {
            throw ValidationException(str(boost::format("Instruction offset mismatch: expected=%08x, actual=%08x") % ins.offset % pos));
        }

        writer.putByte(static_cast<int>(ins.type) & 0xff);
        writer.putByte((static_cast<int>(ins.type) >> 8) & 0xff);

        switch (ins.type) {
        case InstructionType::CPDOWNSP:
        case InstructionType::CPTOPSP:
        case InstructionType::CPDOWNBP:
        case InstructionType::CPTOPBP:
            writer.putInt32(ins.stackOffset);
            writer.putUint16(ins.size);
            break;
        case InstructionType::CONSTI:
            writer.putInt32(ins.intValue);
            break;
        case InstructionType::CONSTF:
            writer.putFloat(ins.floatValue);
            break;
        case InstructionType::CONSTS: {
            writer.putUint16(ins.strValue.length());
            writer.putString(ins.strValue);
            break;
        }
        case InstructionType::CONSTO:
            writer.putInt32(ins.objectId);
            break;
        case InstructionType::ACTION:
            writer.putUint16(ins.routine);
            writer.putByte(ins.argCount);
            break;
        case InstructionType::EQUALTT:
        case InstructionType::NEQUALTT:
            writer.putUint16(ins.size);
            break;
        case InstructionType::MOVSP:
            writer.putInt32(ins.stackOffset);
            break;
        case InstructionType::JMP:
        case InstructionType::JSR:
        case InstructionType::JZ:
        case InstructionType::JNZ:
            writer.putInt32(ins.jumpOffset);
            break;
        case InstructionType::DESTRUCT:
            writer.putUint16(ins.size);
            writer.putInt16(ins.stackOffset);
            writer.putUint16(ins.sizeNoDestroy);
            break;
        case InstructionType::DECISP:
        case InstructionType::INCISP:
        case InstructionType::DECIBP:
        case InstructionType::INCIBP:
            writer.putInt32(ins.stackOffset);
            break;
        case InstructionType::STORE_STATE:
            writer.putUint32(ins.size);
            writer.putUint32(ins.sizeLocals);
            break;
        default:
            break;
        }
    }

    BinaryWriter ncsWriter(*out, endian::order::big);

    ncsWriter.putString(string("NCS V1.0", 8));
    ncsWriter.putByte(0x42);
    ncsWriter.putUint32(13 + writer.tell());
    ncsWriter.putString(string(&bytes[0], bytes.size()));
}

} // namespace script

} // namespace reone
