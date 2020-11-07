/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include "binfile.h"

using namespace std;

namespace fs = boost::filesystem;

namespace reone {

namespace resource {

vector<char> BinaryFile::readArray(istream &in, int n) {
    if (n == 0) {
        return ByteArray();
    }
    ByteArray arr(n);
    in.read(&arr[0], n);

    return move(arr);
}

vector<char> BinaryFile::readArray(istream &in, uint32_t off, int n) {
    streampos pos = in.tellg();
    in.seekg(off);

    ByteArray arr(readArray<char>(in, n));
    in.seekg(pos);

    return move(arr);
}

BinaryFile::BinaryFile(int signSize, const char *sign) : _signSize(signSize) {
    if (!sign) return;

    _sign.resize(_signSize);
    memcpy(&_sign[0], sign, _signSize);
}

void BinaryFile::load(const shared_ptr<istream> &in) {
    if (!in) {
        throw invalid_argument("Invalid input stream");
    }
    _in = in;
    _reader = make_unique<StreamReader>(in, _endianess);

    load();
}

void BinaryFile::load() {
    querySize();
    checkSignature();
    doLoad();
}

void BinaryFile::querySize() {
    _in->seekg(0, ios::end);
    _size = _in->tellg();
    _in->seekg(0);
}

void BinaryFile::checkSignature() {
    if (_size < _signSize) {
        throw runtime_error("Invalid binary file size");
    }
    char buf[16];
    _in->read(buf, _signSize);
    if (!equal(_sign.begin(), _sign.end(), buf)) {
        throw runtime_error("Invalid binary file signature");
    }
}

void BinaryFile::load(const fs::path &path) {
    if (!fs::exists(path)) {
        throw runtime_error("File not found: " + path.string());
    }
    _in = make_shared<fs::ifstream>(path, ios::binary);
    _reader = make_unique<StreamReader>(_in, _endianess);
    _path = path;

    load();
}

uint32_t BinaryFile::tell() const {
    return _reader->tell();
}

void BinaryFile::ignore(int size) {
    _reader->ignore(size);
}

uint8_t BinaryFile::readByte() {
    return _reader->getByte();
}

int16_t BinaryFile::readInt16() {
    return _reader->getInt16();
}

uint16_t BinaryFile::readUint16() {
    return _reader->getUint16();
}

int32_t BinaryFile::readInt32() {
    return _reader->getInt32();
}

uint32_t BinaryFile::readUint32() {
    return _reader->getUint32();
}

int64_t BinaryFile::readInt64() {
    return _reader->getInt64();
}

uint64_t BinaryFile::readUint64() {
    return _reader->getUint64();
}

float BinaryFile::readFloat() {
    return _reader->getFloat();
}

double BinaryFile::readDouble() {
    return _reader->getDouble();
}

string BinaryFile::readFixedString(int size) {
    string result(_reader->getString(size));
    result.erase(find(result.begin(), result.end(), '\0'), result.end());
    return move(result);
}

string BinaryFile::readFixedString(uint32_t off, int size) {
    size_t pos = _reader->tell();
    _reader->seek(off);

    string result(readFixedString(size));
    _reader->seek(pos);

    return move(result);
}

string BinaryFile::readString(uint32_t off) {
    size_t pos = _reader->tell();
    _reader->seek(off);

    string result(_reader->getCString());
    _reader->seek(pos);

    return move(result);
}

string BinaryFile::readString(uint32_t off, int size) {
    size_t pos = _reader->tell();
    _reader->seek(off);

    string result(_reader->getString(size));
    _reader->seek(pos);

    return move(result);
}

} // namespace resource

} // namespace reone
