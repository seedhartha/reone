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

    inline void seek(size_t pos, SeekOrigin origin = SeekOrigin::Begin) {
        _stream.seek(pos, origin);
    }

    inline void ignore(int count) {
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
    std::string readCString();
    std::string readCString(int maxlen);
    ByteArray readBytes(int count);

    std::string readStringAt(size_t offset, int len) {
        return readAt<std::string>(offset, [this, &len]() {
            return readString(len);
        });
    }

    std::string readCStringAt(size_t offset, int len) {
        return readAt<std::string>(offset, [this, &len]() {
            return readCString(len);
        });
    }

    std::string readCStringAt(size_t offset) {
        return readAt<std::string>(offset, [this]() {
            return readCString();
        });
    }

    ByteArray readBytesAt(size_t offset, int count) {
        return readAt<ByteArray>(offset, [this, &count]() {
            return readBytes(count);
        });
    }

    std::vector<uint16_t> readUint16Array(int count) {
        std::vector<uint16_t> elems;
        elems.reserve(count);
        for (int i = 0; i < count; ++i) {
            elems.push_back(readUint16());
        }
        return elems;
    }

    std::vector<uint32_t> readUint32Array(int count) {
        return readArray<uint32_t>(count, [this]() {
            return readUint32();
        });
    }

    std::vector<uint32_t> readUint32ArrayAt(size_t offset, int count) {
        return readArrayAt<uint32_t>(offset, count, [this]() {
            return readUint32();
        });
    }

    std::vector<int32_t> readInt32Array(int count) {
        return readArray<int32_t>(count, [this]() {
            return readInt32();
        });
    }

    std::vector<float> readFloatArray(int count) {
        return readArray<float>(count, [this]() {
            return readFloat();
        });
    }

    std::vector<float> readFloatArrayAt(size_t offset, int count) {
        return readArrayAt<float>(offset, count, [this]() {
            return readFloat();
        });
    }

    inline size_t position() const {
        return _stream.position();
    }

    inline bool eof() const {
        return _stream.eof();
    }

    template <class T>
    T readAt(size_t offset, std::function<T()> read) {
        size_t pos = _stream.position();
        seek(offset);
        auto retval = read();
        seek(pos);
        return retval;
    }

    template <class T>
    std::vector<T> readArray(int size, std::function<T()> read) {
        std::vector<T> array;
        array.reserve(size);
        for (int i = 0; i < size; ++i) {
            array.push_back(read());
        }
        return array;
    }

    template <class T>
    std::vector<T> readArrayAt(size_t offset, int size, std::function<T()> read) {
        return readAt<std::vector<T>>(offset, [this, &size, &read]() {
            return readArray<T>(size, read);
        });
    }

private:
    IInputStream &_stream;
    boost::endian::order _endianess;
};

} // namespace reone
