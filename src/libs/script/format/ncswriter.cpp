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

#include "reone/script/format/ncswriter.h"

#include "reone/resource/exception/format.h"
#include "reone/script/program.h"
#include "reone/system/binarywriter.h"
#include "reone/system/collectionutil.h"
#include "reone/system/stream/fileoutput.h"
#include "reone/system/stream/memoryoutput.h"

using namespace reone::resource;

namespace reone {

namespace script {

void NcsWriter::save(const boost::filesystem::path &path) {
    auto ncs = std::make_shared<FileOutputStream>(path, OpenMode::Binary);
    save(ncs);
}

void NcsWriter::save(std::shared_ptr<IOutputStream> out) {
    auto bytes = ByteArray();
    auto stream = MemoryOutputStream(bytes);
    BinaryWriter writer(stream, boost::endian::order::big);

    for (auto &ins : _program.instructions()) {
        auto pos = 13 + static_cast<uint32_t>(writer.position());
        if (ins.offset != pos) {
            throw FormatException(str(boost::format("Instruction offset mismatch: expected=%08x, actual=%08x") % ins.offset % pos));
        }

        writer.writeByte(static_cast<int>(ins.type) & 0xff);
        writer.writeByte((static_cast<int>(ins.type) >> 8) & 0xff);

        switch (ins.type) {
        case InstructionType::CPDOWNSP:
        case InstructionType::CPTOPSP:
        case InstructionType::CPDOWNBP:
        case InstructionType::CPTOPBP:
            writer.writeInt32(ins.stackOffset);
            writer.writeUint16(ins.size);
            break;
        case InstructionType::CONSTI:
            writer.writeInt32(ins.intValue);
            break;
        case InstructionType::CONSTF:
            writer.writeFloat(ins.floatValue);
            break;
        case InstructionType::CONSTS: {
            writer.writeUint16(ins.strValue.length());
            writer.writeString(ins.strValue);
            break;
        }
        case InstructionType::CONSTO:
            writer.writeInt32(ins.objectId);
            break;
        case InstructionType::ACTION:
            writer.writeUint16(ins.routine);
            writer.writeByte(ins.argCount);
            break;
        case InstructionType::EQUALTT:
        case InstructionType::NEQUALTT:
            writer.writeUint16(ins.size);
            break;
        case InstructionType::MOVSP:
            writer.writeInt32(ins.stackOffset);
            break;
        case InstructionType::JMP:
        case InstructionType::JSR:
        case InstructionType::JZ:
        case InstructionType::JNZ:
            writer.writeInt32(ins.jumpOffset);
            break;
        case InstructionType::DESTRUCT:
            writer.writeUint16(ins.size);
            writer.writeInt16(ins.stackOffset);
            writer.writeUint16(ins.sizeNoDestroy);
            break;
        case InstructionType::DECISP:
        case InstructionType::INCISP:
        case InstructionType::DECIBP:
        case InstructionType::INCIBP:
            writer.writeInt32(ins.stackOffset);
            break;
        case InstructionType::STORE_STATE:
            writer.writeUint32(ins.size);
            writer.writeUint32(ins.sizeLocals);
            break;
        default:
            break;
        }
    }

    BinaryWriter ncsWriter(*out, boost::endian::order::big);

    ncsWriter.writeString(std::string("NCS V1.0", 8));
    ncsWriter.writeByte(0x42);
    ncsWriter.writeUint32(13 + writer.position());
    ncsWriter.writeString(std::string(&bytes[0], bytes.size()));
}

} // namespace script

} // namespace reone
