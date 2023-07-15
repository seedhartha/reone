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

#include "reone/graphics/format/tgareader.h"

#include "reone/graphics/texture.h"
#include "reone/graphics/textureutil.h"
#include "reone/resource/exception/format.h"
#include "reone/system/logutil.h"

using namespace reone::resource;

namespace reone {

namespace graphics {

void TgaReader::load() {
    auto idLength = _tga.readByte();

    _tga.skipBytes(1);

    _dataType = static_cast<TGADataType>(_tga.readByte());
    switch (_dataType) {
    case TGADataType::Grayscale:
    case TGADataType::RGBA:
    case TGADataType::RGBA_RLE:
        break;
    default:
        warn("Unsupported TGA data type: " + std::to_string(static_cast<int>(_dataType)));
        return;
    }

    _tga.skipBytes(9);

    auto width = _tga.readUint16();
    auto height = _tga.readUint16();

    uint8_t bpp = _tga.readByte();
    if ((isRGBA() && bpp != 24 && bpp != 32) || (isGrayscale() && bpp != 8)) {
        throw FormatException("Unsupported bits per pixel: " + std::to_string(bpp));
    }

    uint8_t descriptor = _tga.readByte();
    bool flipY = (descriptor & 0x10) != 0;
    if (flipY) {
        throw FormatException("Vertically flipped images are not supported");
    }

    bool cubemap = height / width == kNumCubeFaces;
    if (cubemap) {
        _width = width;
        _height = width;
        _numLayers = kNumCubeFaces;
    } else {
        _width = width;
        _height = height;
        _numLayers = 1;
    }
    _alpha = isRGBA() && bpp == 32;

    _tga.skipBytes(idLength);
    loadTexture();
}

void TgaReader::loadTexture() {
    std::vector<Texture::Layer> layers;
    layers.reserve(_numLayers);
    for (int i = 0; i < _numLayers; ++i) {
        auto pixels = std::make_shared<ByteBuffer>(readPixels(_width, _height));
        layers.push_back(Texture::Layer {std::move(pixels)});
    }

    PixelFormat format = isGrayscale() ? PixelFormat::R8 : (_alpha ? PixelFormat::BGRA8 : PixelFormat::BGR8);
    _texture = std::make_shared<Texture>(_resRef, getTextureProperties(_usage));
    _texture->setPixels(_width, _height, format, std::move(layers));
}

ByteBuffer TgaReader::readPixels(int w, int h) {
    if (isRLE()) {
        return readPixelsRLE(w, h);
    }
    int dataSize = (isRGBA() ? (_alpha ? 4 : 3) : 1) * w * h;
    return _tga.readBytes(dataSize);
}

ByteBuffer TgaReader::readPixelsRLE(int w, int h) {
    ByteBuffer result;

    int count = w * h;
    while (count > 0) {
        uint8_t code = _tga.readByte();
        int length = glm::min((code & 0x7f) + 1, count);

        count -= length;

        if (code & 0x80) {
            uint8_t b = _tga.readByte();
            uint8_t g = _tga.readByte();
            uint8_t r = _tga.readByte();
            uint8_t a = _alpha ? _tga.readByte() : 0;
            while (length--) {
                result.push_back(b);
                result.push_back(g);
                result.push_back(r);
                if (_alpha) {
                    result.push_back(a);
                }
            }
        } else {
            while (length--) {
                uint8_t b = _tga.readByte();
                uint8_t g = _tga.readByte();
                uint8_t r = _tga.readByte();
                uint8_t a = _alpha ? _tga.readByte() : 0;
                result.push_back(b);
                result.push_back(g);
                result.push_back(r);
                if (_alpha) {
                    result.push_back(a);
                }
            }
        }
    }

    return std::move(result);
}

bool TgaReader::isGrayscale() const {
    return _dataType == TGADataType::Grayscale;
}

bool TgaReader::isRGBA() const {
    return _dataType == TGADataType::RGBA || _dataType == TGADataType::RGBA_RLE;
}

bool TgaReader::isRLE() const {
    return _dataType == TGADataType::RGBA_RLE;
}

} // namespace graphics

} // namespace reone
