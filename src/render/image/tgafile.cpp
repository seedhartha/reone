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

#include "tgafile.h"

#include "../textureutil.h"

using namespace std;

namespace reone {

namespace render {

TgaFile::TgaFile(const string &resRef, TextureType type) : BinaryFile(0), _resRef(resRef), _texType(type) {
}

void TgaFile::doLoad() {
    uint8_t idLength = readByte();

    ignore(1);

    _imageType = static_cast<ImageType>(readByte());
    switch (_imageType) {
        case ImageType::Grayscale:
        case ImageType::RGBA:
            break;
        default:
            throw runtime_error("Unsupported image type: " + to_string(static_cast<int>(_imageType)));
    }

    ignore(9);

    _width = readUint16();
    _height = readUint16();

    uint8_t bpp = readByte();
    _alpha = _imageType == ImageType::RGBA && bpp == 32;

    if ((_imageType == ImageType::RGBA && bpp != 24 && bpp != 32) ||
        (_imageType == ImageType::Grayscale && bpp != 8)) {

        throw runtime_error("Unsupported bits per pixel: " + to_string(bpp));
    }

    uint8_t descriptor = readByte();

    bool flipY = (descriptor & 0x10) != 0;
    if (flipY) {
        throw runtime_error("Vertically flipped images are not supported");
    }

    ignore(idLength);

    loadTexture();
}

void TgaFile::loadTexture() {
    bool cubeMap = _height / _width == 6;
    int layerCount = cubeMap ? 6 : 1;

    vector<Texture::Layer> layers;
    layers.reserve(layerCount);

    for (int i = 0; i < layerCount; ++i) {
        Texture::MipMap mipMap;
        mipMap.width = _width;
        mipMap.height = cubeMap ? _width : _height;

        int pixelCount = mipMap.width * mipMap.height;
        int dataSize = (_imageType == ImageType::Grayscale ? 1 : (_alpha ? 4 : 3)) * pixelCount;

        mipMap.data = _reader->getArray<char>(dataSize);

        Texture::Layer layer;
        layer.mipMaps.push_back(move(mipMap));

        layers.push_back(move(layer));
    }

    PixelFormat format = _imageType == ImageType::Grayscale ?
        PixelFormat::Grayscale :
        (_alpha ? PixelFormat::BGRA : PixelFormat::BGR);

    if (cubeMap) {
        prepareCubeMap(layers, format, format);
    }

    _texture = make_shared<Texture>(_resRef, _texType, _width, _height);
    _texture->init();
    _texture->setPixels(move(layers), format);
}

} // namespace render

} // namespace reone
