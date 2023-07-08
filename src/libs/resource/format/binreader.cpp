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

#include "reone/resource/format/binreader.h"

#include "reone/resource/exception/format.h"

namespace reone {

namespace resource {

void BinaryResourceReader::load(IInputStream &in) {
    _reader = std::make_unique<BinaryReader>(in, _endianess);
    querySize();
    onLoad();
}

void BinaryResourceReader::querySize() {
    _reader->seek(0, SeekOrigin::End);
    _size = _reader->position();
    _reader->seek(0, SeekOrigin::Begin);
}

void BinaryResourceReader::checkSignature(const std::string &expected) {
    if (_size < expected.size()) {
        throw FormatException("Invalid binary resource size");
    }
    auto actual = readString(expected.size());
    if (expected != actual) {
        throw FormatException(str(boost::format("Invalid binary resource signature: expected '%s', got '%s'") % expected % actual));
    }
}

size_t BinaryResourceReader::tell() const {
    return _reader->position();
}

void BinaryResourceReader::seek(size_t pos) {
    _reader->seek(pos);
}

void BinaryResourceReader::ignore(int count) {
    _reader->ignore(count);
}

uint8_t BinaryResourceReader::readByte() {
    return _reader->readByte();
}

uint16_t BinaryResourceReader::readUint16() {
    return _reader->readUint16();
}

uint32_t BinaryResourceReader::readUint32() {
    return _reader->readUint32();
}

uint64_t BinaryResourceReader::readUint64() {
    return _reader->readUint64();
}

int16_t BinaryResourceReader::readInt16() {
    return _reader->readInt16();
}

int32_t BinaryResourceReader::readInt32() {
    return _reader->readInt32();
}

float BinaryResourceReader::readFloat() {
    return _reader->readFloat();
}

std::string BinaryResourceReader::readCString(int len) {
    std::string result(_reader->readString(len));
    result.erase(find(result.begin(), result.end(), '\0'), result.end());
    return std::move(result);
}

std::string BinaryResourceReader::readCString(size_t off, int len) {
    size_t pos = _reader->position();
    _reader->seek(off);

    std::string result(readCString(len));
    _reader->seek(pos);

    return std::move(result);
}

std::string BinaryResourceReader::readCStringAt(size_t off) {
    size_t pos = _reader->position();
    _reader->seek(off);

    std::string result(_reader->readNullTerminatedString());
    _reader->seek(pos);

    return std::move(result);
}

std::string BinaryResourceReader::readString(int len) {
    return _reader->readString(len);
}

std::string BinaryResourceReader::readString(size_t off, int len) {
    size_t pos = _reader->position();
    _reader->seek(off);

    std::string result(_reader->readString(len));
    _reader->seek(pos);

    return std::move(result);
}

ByteArray BinaryResourceReader::readBytes(int count) {
    return _reader->readBytes(count);
}

ByteArray BinaryResourceReader::readBytes(size_t off, int count) {
    size_t pos = _reader->position();
    _reader->seek(off);

    ByteArray result(_reader->readBytes(count));
    _reader->seek(pos);

    return std::move(result);
}

} // namespace resource

} // namespace reone
