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

#include "binreader.h"

#include "../../common/stream/fileinput.h"
#include "../../common/binaryreader.h"

using namespace std;

namespace fs = boost::filesystem;

namespace reone {

namespace resource {

BinaryResourceReader::BinaryResourceReader(int signSize, const char *sign) :
    _signSize(signSize) {
    if (!sign)
        return;

    _sign.resize(_signSize);
    memcpy(&_sign[0], sign, _signSize);
}

void BinaryResourceReader::load(IInputStream &in) {
    _in = &in;
    _reader = make_unique<BinaryReader>(in, _endianess);

    load();
}

void BinaryResourceReader::load() {
    querySize();
    checkSignature();
    doLoad();
}

void BinaryResourceReader::querySize() {
    _in->seek(0, SeekOrigin::End);
    _size = _in->position();
    _in->seek(0, SeekOrigin::Begin);
}

void BinaryResourceReader::checkSignature() {
    if (_size < _signSize) {
        throw runtime_error("Invalid binary file size");
    }
    char buf[16];
    _in->read(buf, _signSize);
    if (!equal(_sign.begin(), _sign.end(), buf)) {
        throw runtime_error(str(boost::format("Invalid binary file signature: %s") % string(buf, _signSize)));
    }
}

void BinaryResourceReader::load(fs::path path) {
    if (!fs::exists(path)) {
        throw runtime_error("File not found: " + path.string());
    }
    _in = new FileInputStream(path, OpenMode::Binary); // TODO: free
    _reader = make_unique<BinaryReader>(*_in, _endianess);
    _path = path;

    load();
}

size_t BinaryResourceReader::tell() const {
    return _reader->tell();
}

void BinaryResourceReader::seek(size_t pos) {
    _reader->seek(pos);
}

void BinaryResourceReader::ignore(int count) {
    _reader->ignore(count);
}

uint8_t BinaryResourceReader::readByte() {
    return _reader->getByte();
}

uint16_t BinaryResourceReader::readUint16() {
    return _reader->getUint16();
}

uint32_t BinaryResourceReader::readUint32() {
    return _reader->getUint32();
}

uint64_t BinaryResourceReader::readUint64() {
    return _reader->getUint64();
}

int16_t BinaryResourceReader::readInt16() {
    return _reader->getInt16();
}

int32_t BinaryResourceReader::readInt32() {
    return _reader->getInt32();
}

float BinaryResourceReader::readFloat() {
    return _reader->getFloat();
}

string BinaryResourceReader::readCString(int len) {
    string result(_reader->getString(len));
    result.erase(find(result.begin(), result.end(), '\0'), result.end());
    return move(result);
}

string BinaryResourceReader::readCString(size_t off, int len) {
    size_t pos = _reader->tell();
    _reader->seek(off);

    string result(readCString(len));
    _reader->seek(pos);

    return move(result);
}

string BinaryResourceReader::readCStringAt(size_t off) {
    size_t pos = _reader->tell();
    _reader->seek(off);

    string result(_reader->getNullTerminatedString());
    _reader->seek(pos);

    return move(result);
}

string BinaryResourceReader::readString(int len) {
    return _reader->getString(len);
}

string BinaryResourceReader::readString(size_t off, int len) {
    size_t pos = _reader->tell();
    _reader->seek(off);

    string result(_reader->getString(len));
    _reader->seek(pos);

    return move(result);
}

ByteArray BinaryResourceReader::readBytes(int count) {
    return _reader->getBytes(count);
}

ByteArray BinaryResourceReader::readBytes(size_t off, int count) {
    size_t pos = _reader->tell();
    _reader->seek(off);

    ByteArray result(_reader->getBytes(count));
    _reader->seek(pos);

    return move(result);
}

} // namespace resource

} // namespace reone
