/*
 * Copyright © 2020 Vsevolod Kremianskii
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

namespace resources {

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
    _in.reset(new fs::ifstream(path, ios::binary));
    _path = path;

    load();
}

uint32_t BinaryFile::tell() const {
    return _in->tellg();
}

void BinaryFile::ignore(int size) {
    _in->ignore(size);
}

uint8_t BinaryFile::readByte() {
    uint8_t val;
    _in->read(reinterpret_cast<char *>(&val), 1);
    return val;
}

int16_t BinaryFile::readInt16() {
    int16_t val;
    _in->read(reinterpret_cast<char *>(&val), 2);
    return val;
}

int16_t BinaryFile::readInt16BE() {
    int16_t val;
    _in->read(reinterpret_cast<char *>(&val), 2);
    return ((val << 8) & 0xff00) | ((val >> 8) & 0x00ff);
}

uint16_t BinaryFile::readUint16() {
    uint16_t val;
    _in->read(reinterpret_cast<char *>(&val), 2);
    return val;
}

uint16_t BinaryFile::readUint16BE() {
    uint16_t val;
    _in->read(reinterpret_cast<char *>(&val), 2);
    return ((val << 8) & 0xff00) | ((val >> 8) & 0x00ff);
}

int32_t BinaryFile::readInt32() {
    int32_t val;
    _in->read(reinterpret_cast<char *>(&val), 4);
    return val;
}

int32_t BinaryFile::readInt32BE() {
    int32_t val;
    _in->read(reinterpret_cast<char *>(&val), 4);

    return
        ((val << 24) & 0xff000000) |
        ((val << 8) & 0x00ff0000) |
        ((val >> 8) & 0x0000ff00) |
        ((val >> 24) & 0x000000ff);
}

uint32_t BinaryFile::readUint32() {
    uint32_t val;
    _in->read(reinterpret_cast<char *>(&val), 4);
    return val;
}

uint32_t BinaryFile::readUint32BE() {
    uint32_t val;
    _in->read(reinterpret_cast<char *>(&val), 4);

    return
        ((val << 24) & 0xff000000) |
        ((val << 8) & 0x00ff0000) |
        ((val >> 8) & 0x0000ff00) |
        ((val >> 24) & 0x000000ff);
}

int64_t BinaryFile::readInt64() {
    int64_t val;
    _in->read(reinterpret_cast<char *>(&val), 8);
    return val;
}

uint64_t BinaryFile::readUint64() {
    uint64_t val;
    _in->read(reinterpret_cast<char *>(&val), 8);
    return val;
}

float BinaryFile::readFloat() {
    float val;
    _in->read(reinterpret_cast<char *>(&val), 4);
    return val;
}

float BinaryFile::readFloatBE() {
    uint32_t val;
    _in->read(reinterpret_cast<char *>(&val), 4);
    val =
        ((val << 24) & 0xff000000) |
        ((val << 8) & 0x00ff0000) |
        ((val >> 8) & 0x0000ff00) |
        ((val >> 24) & 0x000000ff);

    return *reinterpret_cast<float *>(&val);
}

double BinaryFile::readDouble() {
    double val;
    _in->read(reinterpret_cast<char *>(&val), 4);
    return val;
}

string BinaryFile::readFixedString(int size) {
    string s;
    s.resize(size);
    _in->read(&s[0], size);

    return s.c_str();
}

string BinaryFile::readFixedString(uint32_t off, int size) {
    streampos pos = _in->tellg();
    _in->seekg(off);

    string s(readFixedString(size));
    _in->seekg(pos);

    return move(s);
}

string BinaryFile::readString(uint32_t off) {
    streampos pos = _in->tellg();
    _in->seekg(off);

    char buf[256];
    streamsize chRead = _in->rdbuf()->sgetn(buf, sizeof(buf));

    _in->seekg(pos);

    return string(buf, strnlen(buf, chRead));
}

string BinaryFile::readString(uint32_t off, int size) {
    streampos pos = _in->tellg();
    _in->seekg(off);

    string s;
    s.resize(size);

    _in->read(&s[0], size);
    _in->seekg(pos);

    return move(s);
}

} // namespace resources

} // namespace reone
