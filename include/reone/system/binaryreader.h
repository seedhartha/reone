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

    void seek(size_t pos, SeekOrigin origin = SeekOrigin::Begin) {
        _stream.seek(pos, origin);
    }

    void ignore(int count) {
        _stream.seek(count, SeekOrigin::Current);
    }

    size_t streamLength() {
        size_t pos = _stream.position();
        _stream.seek(0, SeekOrigin::End);
        size_t length = _stream.position();
        _stream.seek(pos, SeekOrigin::Begin);
        return length;
    }

    uint8_t readByte();
    char readChar();
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

    inline std::vector<uint16_t> readUint16Array(int count) {
        std::vector<uint16_t> elems;
        elems.reserve(count);
        for (int i = 0; i < count; ++i) {
            elems.push_back(readUint16());
        }
        return elems;
    }

    inline std::vector<uint32_t> readUint32Array(int count) {
        std::vector<uint32_t> elems;
        elems.reserve(count);
        for (int i = 0; i < count; ++i) {
            elems.push_back(readUint32());
        }
        return elems;
    }

    inline std::vector<uint32_t> readUint32Array(size_t offset, int count) {
        size_t pos = position();
        seek(offset);

        auto elems = readUint32Array(count);
        seek(pos);

        return elems;
    }

    inline std::vector<int32_t> readInt32Array(int count) {
        std::vector<int32_t> elems;
        elems.reserve(count);
        for (int i = 0; i < count; ++i) {
            elems.push_back(readInt32());
        }
        return elems;
    }

    inline std::vector<float> readFloatArray(int count) {
        std::vector<float> elems;
        elems.reserve(count);
        for (int i = 0; i < count; ++i) {
            elems.push_back(readFloat());
        }
        return elems;
    }

    inline std::vector<float> readFloatArray(size_t offset, int count) {
        size_t pos = position();
        seek(offset);

        auto elems = readFloatArray(count);
        seek(pos);

        return elems;
    }

    size_t position() const {
        return _stream.position();
    }

    bool eof() const {
        return _stream.eof();
    }

private:
    IInputStream &_stream;
    boost::endian::order _endianess;
};

} // namespace reone
