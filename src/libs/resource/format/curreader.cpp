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

#include "reone/resource/format/curreader.h"

#include "reone/graphics/texture.h"
#include "reone/graphics/textureutil.h"
#include "reone/resource/format/signutil.h"

using namespace reone::graphics;

namespace reone {

namespace resource {

void CurReader::load() {
    loadHeader();
    loadData();
}

void CurReader::loadHeader() {
    _cur.skipBytes(4);

    uint32_t size = _cur.readUint32();

    _width = _cur.readInt32();
    _height = _cur.readInt32();

    uint16_t planes = _cur.readUint16();

    _bitCount = _cur.readUint16();

    uint32_t compression = _cur.readUint32();
}

void CurReader::loadData() {
    _cur.seek(44);

    int numPixels = _width * _width;
    int colorCount = _bitCount == 8 ? 256 : 16;

    ByteBuffer palette(_cur.readBytes(4 * colorCount));
    ByteBuffer xorData(_cur.readBytes(numPixels));
    ByteBuffer andData(_cur.readBytes(numPixels / 8));

    auto pixels = std::make_shared<ByteBuffer>(4 * numPixels, '\0');

    for (int y = 0; y < _width; ++y) {
        for (int x = 0; x < _width; ++x) {
            int pixelIdx = (y * _width) + x;
            int offMipMap = 4 * pixelIdx;
            int offPalette = 4 * static_cast<uint8_t>(xorData[pixelIdx]);

            *(pixels->data() + offMipMap + 0) = palette[offPalette + 0];
            *(pixels->data() + offMipMap + 1) = palette[offPalette + 1];
            *(pixels->data() + offMipMap + 2) = palette[offPalette + 2];
            *(pixels->data() + offMipMap + 3) = (andData[pixelIdx / 8] & (1 << (7 - x % 8))) ? 0 : 0xff;
        }
    }

    _texture = std::make_shared<Texture>("", getTextureProperties(TextureUsage::GUI));
    _texture->setPixels(_width, _width, PixelFormat::BGRA8, Texture::Layer {std::move(pixels)});
}

} // namespace resource

} // namespace reone
