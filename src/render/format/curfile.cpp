/*
 * Copyright (c) 2020 The reone project contributors
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

#include "curfile.h"

using namespace std;

namespace reone {

namespace render {

CurFile::CurFile() : BinaryFile(0) {
}

void CurFile::doLoad() {
    loadHeader();
    loadData();
}

void CurFile::loadHeader() {
    ignore(4);

    uint32_t size = readUint32();

    _width = readInt32();
    _height = readInt32();

    uint16_t planes = readUint16();

    _bitCount = readUint16();

    uint32_t compression = readUint32();
}

void CurFile::loadData() {
    seek(44);

    int pixelCount = _width * _width;
    int colorCount = _bitCount == 8 ? 256 : 16;

    ByteArray palette(_reader->getArray<char>(4 * colorCount));
    ByteArray xorData(_reader->getArray<char>(pixelCount));
    ByteArray andData(_reader->getArray<char>(pixelCount / 8));

    Texture::MipMap mipMap;
    mipMap.width = _width;
    mipMap.height = _width;
    mipMap.data.resize(4 * pixelCount);

    for (int y = 0; y < _width; ++y) {
        for (int x = 0; x < _width; ++x) {
            int pixelIdx = (y * _width) + x;
            int offMipMap = 4 * pixelIdx;
            int offPalette = 4 * static_cast<uint8_t>(xorData[pixelIdx]);

            mipMap.data[offMipMap + 0] = palette[offPalette + 0];
            mipMap.data[offMipMap + 1] = palette[offPalette + 1];
            mipMap.data[offMipMap + 2] = palette[offPalette + 2];
            mipMap.data[offMipMap + 3] = (andData[pixelIdx / 8] & (1 << (7 - x % 8))) ? 0 : 0xff;
        }
    }

    Texture::Layer layer;
    layer.mipMaps.push_back(move(mipMap));

    _texture = make_shared<Texture>("", TextureType::Cursor);
    _texture->_width = mipMap.width;
    _texture->_height = mipMap.height;
    _texture->_pixelFormat = PixelFormat::BGRA;
    _texture->_layers.push_back(move(layer));
}

shared_ptr<Texture> CurFile::texture() {
    return _texture;
}

} // namespace render

} // namespace reone
