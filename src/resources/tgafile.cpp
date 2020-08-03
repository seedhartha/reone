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

#include "tgafile.h"

using namespace reone::render;

namespace reone {

namespace resources {

TgaFile::TgaFile(const std::string &resRef, TextureType type) : BinaryFile(0), _resRef(resRef), _texType(type) {
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
            throw std::runtime_error("TGA: unsupported image type: " + std::to_string(static_cast<int>(_imageType)));
    }

    ignore(9);

    _width = readUint16();
    _height = readUint16();

    uint8_t bpp = readByte();

    if (_imageType == ImageType::RGBA && bpp != 32 ||
        _imageType == ImageType::Grayscale && bpp != 8) {

        throw std::runtime_error("TGA: unsupported bits per pixel: " + std::to_string(bpp));
    }

    ignore(1);
    ignore(idLength);

    loadTexture();
}

void TgaFile::loadTexture() {
    int resultSize = 4 * _width * _height;

    _texture = std::make_shared<Texture>(_resRef, _texType);
    _texture->_width = _width;
    _texture->_height = _height;
    _texture->_pixelFormat = PixelFormat::BGRA;
    _texture->_images.resize(1);

    ByteArray &image = _texture->_images.front();
    image.resize(resultSize);

    if (_imageType == ImageType::Grayscale) {
        int size = _width * _height;
        ByteArray buf(size);
        _in->read(&buf[0], size);

        char *pi = &image[0];

        for (int i = 0; i < size; ++i) {
            pi[0] = buf[i];
            pi[1] = buf[i];
            pi[2] = buf[i];
            pi[3] = 0xff;
            pi += 4;
        }

    } else {
        _in->read(&image[0], resultSize);
    }

}

std::shared_ptr<Texture> TgaFile::texture() const {
    return _texture;
}

} // namespace resources

} // namespace reone
