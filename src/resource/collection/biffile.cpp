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

#include "biffile.h"

using namespace std;

namespace fs = boost::filesystem;

namespace reone {

namespace resource {

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
        throw out_of_range("BIF: resource index out of range: " + to_string(idx));
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

    return move(entry);
}

} // namespace bioware

} // namespace reone
