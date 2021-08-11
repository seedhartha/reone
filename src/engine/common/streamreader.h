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

#include "types.h"

namespace reone {

/**
 * Abstraction over reading primitive data types from a standard input stream.
 */
class StreamReader : boost::noncopyable {
public:
    StreamReader(const std::shared_ptr<std::istream> &stream, boost::endian::order endianess = boost::endian::order::little);

    size_t tell();
    void seek(size_t pos);
    void ignore(int count);

    uint8_t getByte();
    uint16_t getUint16();
    uint32_t getUint32();
    uint64_t getUint64();
    int16_t getInt16();
    int32_t getInt32();
    int64_t getInt64();
    float getFloat();
    double getDouble();
    std::string getCString();
    std::string getString(int len);
    std::u16string getNullTerminatedUTF16String();
    ByteArray getBytes(int count);

    bool eof() const;

    inline std::vector<uint16_t> getUint16Array(int count) {
        std::vector<uint16_t> result;
        result.reserve(count);
        for (int i = 0; i < count; ++i) {
            result.push_back(getUint16());
        }
        return std::move(result);
    }

    inline std::vector<uint32_t> getUint32Array(int count) {
        std::vector<uint32_t> result;
        result.reserve(count);
        for (int i = 0; i < count; ++i) {
            result.push_back(getUint32());
        }
        return std::move(result);
    }

    inline std::vector<uint32_t> getUint32Array(size_t offset, int count) {
        size_t pos = tell();
        seek(offset);

        std::vector<uint32_t> result(getUint32Array(count));
        seek(pos);

        return std::move(result);
    }

    inline std::vector<float> getFloatArray(int count) {
        std::vector<float> result;
        result.reserve(count);
        for (int i = 0; i < count; ++i) {
            result.push_back(getFloat());
        }
        return std::move(result);
    }

    inline std::vector<float> getFloatArray(size_t offset, int count) {
        size_t pos = tell();
        seek(offset);

        std::vector<float> result(getFloatArray(count));
        seek(pos);

        return std::move(result);
    }

private:
    std::shared_ptr<std::istream> _stream;
    boost::endian::order _endianess;
};

} // namespace reone
