#include "binfile.h"

namespace fs = boost::filesystem;

namespace reone {

namespace resources {

std::vector<char> BinaryFile::readArray(std::istream &in, int n) {
    if (n == 0) {
        return ByteArray();
    }
    ByteArray arr(n);
    in.read(&arr[0], n);

    return std::move(arr);
}

std::vector<char> BinaryFile::readArray(std::istream &in, uint32_t off, int n) {
    std::streampos pos = in.tellg();
    in.seekg(off);

    ByteArray arr(readArray<char>(in, n));
    in.seekg(pos);

    return std::move(arr);
}

BinaryFile::BinaryFile(int signSize, const char *sign) : _signSize(signSize) {
    if (!sign) return;

    _sign.resize(_signSize);
    std::memcpy(&_sign[0], sign, _signSize);
}

void BinaryFile::load(const std::shared_ptr<std::istream> &in) {
    if (!in) {
        throw std::invalid_argument("Invalid input stream");
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
    _in->seekg(0, std::ios::end);
    _size = _in->tellg();
    _in->seekg(0);
}

void BinaryFile::checkSignature() {
    if (_size < _signSize) {
        throw std::runtime_error("Invalid binary file size");
    }
    char buf[16];
    _in->read(buf, _signSize);
    if (!std::equal(_sign.begin(), _sign.end(), buf)) {
        throw std::runtime_error("Invalid binary file signature");
    }
}

void BinaryFile::load(const fs::path &path) {
    if (!fs::exists(path)) {
        throw std::runtime_error("File not found: " + path.string());
    }
    _in.reset(new fs::ifstream(path, std::ios::binary));
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

uint16_t BinaryFile::readUint16() {
    uint16_t val;
    _in->read(reinterpret_cast<char *>(&val), 2);
    return val;
}

int32_t BinaryFile::readInt32() {
    int32_t val;
    _in->read(reinterpret_cast<char *>(&val), 4);
    return val;
}

uint32_t BinaryFile::readUint32() {
    uint32_t val;
    _in->read(reinterpret_cast<char *>(&val), 4);
    return val;
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

double BinaryFile::readDouble() {
    double val;
    _in->read(reinterpret_cast<char *>(&val), 4);
    return val;
}

std::string BinaryFile::readFixedString(int size) {
    std::string s;
    s.resize(size);
    _in->read(&s[0], size);

    return s.c_str();
}

std::string BinaryFile::readFixedString(uint32_t off, int size) {
    std::streampos pos = _in->tellg();
    _in->seekg(off);

    std::string s(readFixedString(size));
    _in->seekg(pos);

    return std::move(s);
}

std::string BinaryFile::readString(uint32_t off) {
    std::streampos pos = _in->tellg();
    _in->seekg(off);

    char buf[256];
    std::streamsize chRead = _in->rdbuf()->sgetn(buf, sizeof(buf));

    _in->seekg(pos);

    return std::string(buf, strnlen(buf, chRead));
}

std::string BinaryFile::readString(uint32_t off, int size) {
    std::streampos pos = _in->tellg();
    _in->seekg(off);

    std::string s;
    s.resize(size);

    _in->read(&s[0], size);
    _in->seekg(pos);

    return std::move(s);
}

} // namespace resources

} // namespace reone
