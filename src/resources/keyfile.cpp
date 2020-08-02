#include "keyfile.h"

#include <boost/algorithm/string.hpp>

namespace fs = boost::filesystem;

namespace reone {

namespace resources {

static const int kSignatureSize = 8;
static const char kSignature[] = "KEY V1  ";

KeyFile::KeyFile() : BinaryFile(kSignatureSize, kSignature) {
}

void KeyFile::doLoad() {
    _bifCount = readUint32();
    _keyCount = readUint32();
    _filesOffset = readUint32();
    _keysOffset = readUint32();

    loadFiles();
    loadKeys();
}

void KeyFile::loadFiles() {
    _files.reserve(_bifCount);
    seek(_filesOffset);

    for (int i = 0; i < _bifCount; ++i) {
        _files.push_back(readFileEntry());
    }
}

KeyFile::FileEntry KeyFile::readFileEntry() {
    uint32_t fileSize = readUint32();
    uint32_t filenameOffset = readUint32();
    uint16_t filenameSize = readUint16();
    ignore(2);

    FileEntry entry;
    entry.fileSize = fileSize;
    entry.filename = readString(filenameOffset, filenameSize);

    return std::move(entry);
}

void KeyFile::loadKeys() {
    _keys.reserve(_keyCount);
    seek(_keysOffset);

    for (int i = 0; i < _keyCount; ++i) {
        _keys.push_back(readKeyEntry());
    }
}

KeyFile::KeyEntry KeyFile::readKeyEntry() {
    std::string resRef(readFixedString(16));
    uint16_t resType = readUint16();
    uint32_t resId = readUint32();

    KeyEntry entry;
    entry.resRef = boost::to_lower_copy(resRef);
    entry.resType = static_cast<ResourceType>(resType);
    entry.bifIdx = resId >> 20;
    entry.resIdx = resId & 0xfffff;

    return entry;
}

const std::string &KeyFile::getFilename(int idx) const {
    if (idx >= _files.size()) {
        throw std::out_of_range("KEY: file index out of range: " + std::to_string(idx));
    }
    return _files[idx].filename;
}

bool KeyFile::find(const std::string &resRef, ResourceType type, KeyEntry &key) const {
    std::string lcResRef(boost::to_lower_copy(resRef));

    auto it = std::find_if(
        _keys.begin(),
        _keys.end(),
        [&](const KeyEntry &e) { return e.resRef == lcResRef && e.resType == type; });

    if (it == _keys.end()) return false;

    key = *it;

    return true;
}

const std::vector<KeyFile::FileEntry> &KeyFile::files() const {
    return _files;
}

const std::vector<KeyFile::KeyEntry> &KeyFile::keys() const {
    return _keys;
}

} // namespace resources

} // namespace reone