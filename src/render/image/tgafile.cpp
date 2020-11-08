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

#include "tgafile.h"

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
            throw runtime_error("TGA: unsupported image type: " + to_string(static_cast<int>(_imageType)));
    }

    ignore(9);

    _width = readUint16();
    _height = readUint16();

    uint8_t bpp = readByte();

    if ((_imageType == ImageType::RGBA && bpp != 32) ||
        (_imageType == ImageType::Grayscale && bpp != 8)) {

        throw runtime_error("TGA: unsupported bits per pixel: " + to_string(bpp));
    }

    ignore(1);
    ignore(idLength);

    loadTexture();
}

void TgaFile::loadTexture() {
    int finalSize = 4 * _width * _height;

    _texture = make_shared<Texture>(_resRef, _texType);
    _texture->_width = _width;
    _texture->_height = _height;
    _texture->_pixelFormat = PixelFormat::BGRA;
    _texture->_layers.resize(1);

    Texture::Layer &layer = _texture->_layers.front();
    layer.mipMaps.resize(1);

    Texture::MipMap &mipMap = layer.mipMaps.front();
    mipMap.width = _width;
    mipMap.height = _height;
    mipMap.data.resize(finalSize);

    if (_imageType == ImageType::Grayscale) {
        int size = _width * _height;
        ByteArray buf(_reader->getArray<char>(size));
        char *pi = &mipMap.data[0];

        for (int i = 0; i < size; ++i) {
            pi[0] = buf[i];
            pi[1] = buf[i];
            pi[2] = buf[i];
            pi[3] = static_cast<char>(0xff);
            pi += 4;
        }
    } else {
        ByteArray data(_reader->getArray<char>(finalSize));
        mipMap.data = move(data);
    }

}

shared_ptr<Texture> TgaFile::texture() const {
    return _texture;
}

} // namespace render

} // namespace reone
