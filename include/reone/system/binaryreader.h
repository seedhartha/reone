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

#include "reone/system/stream/input.h"
#include "reone/system/types.h"

namespace reone {

class BinaryReader : boost::noncopyable {
public:
    BinaryReader(
        IInputStream &stream,
        boost::endian::order endianess = boost::endian::order::little) :
        _stream(stream),
        _endianess(endianess) {
    }

    size_t tell();
    void seek(size_t pos, SeekOrigin origin = SeekOrigin::Begin);
    void ignore(int count);

    uint8_t readByte();
    uint16_t readUint16();
    uint32_t readUint32();
    uint64_t readUint64();
    int16_t readInt16();
    int32_t readInt32();
    int64_t readInt64();
    float readFloat();
    double readDouble();
    std::string readString(int len);
    std::string readNullTerminatedString();
    ByteArray readBytes(int count);

    bool eof() const;

    inline std::vector<uint16_t> readUint16Array(int count) {
        std::vector<uint16_t> result;
        result.reserve(count);
        for (int i = 0; i < count; ++i) {
            result.push_back(readUint16());
        }
        return std::move(result);
    }

    inline std::vector<uint32_t> readUint32Array(int count) {
        std::vector<uint32_t> result;
        result.reserve(count);
        for (int i = 0; i < count; ++i) {
            result.push_back(readUint32());
        }
        return std::move(result);
    }

    inline std::vector<uint32_t> readUint32Array(size_t offset, int count) {
        size_t pos = tell();
        seek(offset);

        std::vector<uint32_t> result(readUint32Array(count));
        seek(pos);

        return std::move(result);
    }

    inline std::vector<int32_t> readInt32Array(int count) {
        std::vector<int32_t> result;
        result.reserve(count);
        for (int i = 0; i < count; ++i) {
            result.push_back(readInt32());
        }
        return std::move(result);
    }

    inline std::vector<float> readFloatArray(int count) {
        std::vector<float> result;
        result.reserve(count);
        for (int i = 0; i < count; ++i) {
            result.push_back(readFloat());
        }
        return std::move(result);
    }

    inline std::vector<float> readFloatArray(size_t offset, int count) {
        size_t pos = tell();
        seek(offset);

        std::vector<float> result(readFloatArray(count));
        seek(pos);

        return std::move(result);
    }

private:
    IInputStream &_stream;
    boost::endian::order _endianess;
};

} // namespace reone
