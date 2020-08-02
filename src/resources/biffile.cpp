#include "biffile.h"

namespace fs = boost::filesystem;

namespace reone {

namespace resources {

static const int kSignatureSize = 8;
static const char kSignature[] = "BIFFV1  ";

BifFile::BifFile() : BinaryFile(kSignatureSize, kSignature) {
}

void BifFile::doLoad() {
    _resourceCount = readUint32();
    ignore(4);
    _tableOffset = readUint32();
}

ByteArray BifFile::getResourceData(int idx) {
    if (idx >= _resourceCount) {
        throw std::out_of_range("BIF: resource index out of range: " + std::to_string(idx));
    }
    ResourceEntry entry(readResourceEntry(idx));

    return readArray<char>(entry.offset, entry.fileSize);
}

BifFile::ResourceEntry BifFile::readResourceEntry(int idx) {
    seek(_tableOffset + 16 * idx);

    ignore(4);
    uint32_t offset = readUint32();
    uint32_t fileSize = readUint32();

    ResourceEntry entry;
    entry.offset = offset;
    entry.fileSize = fileSize;

    return std::move(entry);
}

} // namespace bioware

} // namespace reone
