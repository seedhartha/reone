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

#include "image.h"

#include "reone/system/stream/memoryoutput.h"
#include "reone/tools/legacy/tpc.h"

using namespace reone::resource;

namespace reone {

void ImageResourceViewModel::openImage(const resource::ResourceId &id, IInputStream &stream) {
    auto tgaBytes = std::make_shared<ByteBuffer>();
    auto txiBytes = std::make_shared<ByteBuffer>();
    if (id.type == ResType::Tpc) {
        auto tga = MemoryOutputStream(*tgaBytes);
        auto txi = MemoryOutputStream(*txiBytes);
        TpcTool().toTGA(stream, tga, txi, false);
    } else {
        stream.seek(0, SeekOrigin::End);
        auto length = stream.position();
        stream.seek(0, SeekOrigin::Begin);
        tgaBytes->resize(length, '\0');
        stream.read(&(*tgaBytes)[0], length);
    }
    _imageContent = ImageContent(tgaBytes, txiBytes);
}

} // namespace reone
