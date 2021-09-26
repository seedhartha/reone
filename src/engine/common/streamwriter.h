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

#include "guardutil.h"
#include "types.h"

namespace reone {

/**
 * Abstraction over writing primitive data types to a standard output stream.
 */
class StreamWriter : boost::noncopyable {
public:
    StreamWriter(
        std::shared_ptr<std::ostream> stream,
        boost::endian::order endianess = boost::endian::order::little) :
        _stream(ensurePresent(stream, "stream")),
        _endianess(endianess) {
    }

    void putByte(uint8_t val);
    void putChar(char val);
    void putUint16(uint16_t val);
    void putUint32(uint32_t val);
    void putInt32(int32_t val);
    void putInt64(int64_t val);
    void putFloat(float val);
    void putString(const std::string &str);
    void putCString(const std::string &str);
    void putBytes(const ByteArray &bytes);
    void putBytes(int count, uint8_t val = 0);

    size_t tell() const;

private:
    std::shared_ptr<std::ostream> _stream;
    boost::endian::order _endianess;

    template <class T>
    void put(T val) {
        boost::endian::conditional_reverse_inplace(val, boost::endian::order::native, _endianess);
        char buf[sizeof(T)];
        memcpy(buf, &val, sizeof(T));
        _stream->write(buf, sizeof(T));
    }
};

} // namespace reone
