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

#include "../../common/stream/input.h"
#include "../../common/streamreader.h"
#include "../../common/types.h"

namespace reone {

namespace resource {

/**
 * Abstract class with utility methods for reading binary files. Descendants are
 * expected to specify the file signature through the constructor and override
 * the doLoad function.
 */
class BinaryReader : boost::noncopyable {
public:
    void load(std::shared_ptr<IInputStream> in);
    void load(boost::filesystem::path path);

protected:
    boost::endian::order _endianess {boost::endian::order::little};
    boost::filesystem::path _path;
    std::shared_ptr<IInputStream> _in;
    std::unique_ptr<StreamReader> _reader;
    size_t _size {0};

    BinaryReader(int signSize, const char *sign = 0);

    virtual void doLoad() = 0;

    size_t tell() const;
    void seek(size_t off);
    void ignore(int count);

    uint8_t readByte();
    uint16_t readUint16();
    uint32_t readUint32();
    uint64_t readUint64();
    int16_t readInt16();
    int32_t readInt32();
    float readFloat();
    std::string readCString(int len);
    std::string readCString(size_t off, int len);
    std::string readCStringAt(size_t off);
    std::string readString(int len);
    std::string readString(size_t off, int len);
    ByteArray readBytes(int count);
    ByteArray readBytes(size_t off, int count);

    inline std::vector<uint16_t> readUint16Array(int count) {
        return _reader->getUint16Array(count);
    }

    inline std::vector<uint32_t> readUint32Array(int count) {
        return _reader->getUint32Array(count);
    }

    inline std::vector<uint32_t> readUint32Array(size_t offset, int count) {
        return _reader->getUint32Array(offset, count);
    }

    inline std::vector<float> readFloatArray(int count) {
        return _reader->getFloatArray(count);
    }

    inline std::vector<float> readFloatArray(size_t offset, int count) {
        return _reader->getFloatArray(offset, count);
    }

private:
    int _signSize {0};
    ByteArray _sign;

    void load();
    void querySize();
    void checkSignature();
};

} // namespace resource

} // namespace reone
