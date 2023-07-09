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

#include "reone/system/stream/output.h"
#include "reone/system/types.h"

namespace reone {

class BinaryWriter : boost::noncopyable {
public:
    BinaryWriter(
        IOutputStream &stream,
        boost::endian::order endianess = boost::endian::order::little) :
        _stream(stream),
        _endianess(endianess) {
    }

    void writeByte(uint8_t val);
    void writeChar(char val);
    void writeUint16(uint16_t val);
    void writeUint32(uint32_t val);
    void writeInt16(int16_t val);
    void writeInt32(int32_t val);
    void writeInt64(int64_t val);
    void writeFloat(float val);
    void writeString(const std::string &str);
    void writeCString(const std::string &str);

    void write(const ByteArray &bytes);
    void write(int count, uint8_t val);

    size_t position() const;

private:
    IOutputStream &_stream;
    boost::endian::order _endianess;

    template <class T>
    void write(T val) {
        boost::endian::conditional_reverse_inplace(val, boost::endian::order::native, _endianess);
        char buf[sizeof(T)];
        memcpy(buf, &val, sizeof(T));
        _stream.write(buf, sizeof(T));
    }
};

} // namespace reone
