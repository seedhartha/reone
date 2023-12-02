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

#include "reone/graphics/format/tpcreader.h"

#include "reone/graphics/format/txireader.h"
#include "reone/graphics/textureutil.h"
#include "reone/system/exception/validation.h"
#include "reone/system/stream/memoryinput.h"

namespace reone {

namespace graphics {

void TpcReader::load() {
    uint32_t dataSize = _tpc.readUint32();

    _tpc.skipBytes(4);

    uint16_t width = _tpc.readUint16();
    uint16_t height = _tpc.readUint16();
    _encoding = static_cast<EncodingType>(_tpc.readByte());
    _numMipMaps = _tpc.readByte();

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

    if (dataSize > 0) {
        _dataSize = dataSize;
        _compressed = true;
    } else {
        int w, h;
        getMipMapSize(0, w, h);
        _dataSize = getMipMapDataSize(w, h);
    }

    loadLayers();
    loadFeatures();
    loadTexture();
}

void TpcReader::loadLayers() {
    _tpc.seek(128);

    _layers.reserve(_numLayers);

    for (int i = 0; i < _numLayers; ++i) {
        auto pixels = std::make_shared<ByteBuffer>(_tpc.readBytes(_dataSize));

        // Ignore mip maps
        for (int j = 1; j < _numMipMaps; ++j) {
            int w, h;
            getMipMapSize(j, w, h);
            _tpc.skipBytes(getMipMapDataSize(w, h));
        }

        _layers.push_back(Texture::Layer {std::move(pixels)});
    }
}

void TpcReader::loadFeatures() {
    auto pos = _tpc.position();
    auto length = _tpc.length();
    if (pos >= length) {
        return;
    }
    _txiData = _tpc.readBytes(static_cast<int>(length - pos));

    auto txi = MemoryInputStream(_txiData);
    auto reader = TxiReader();
    reader.load(txi);

    _features = reader.features();
}

void TpcReader::loadTexture() {
    _texture = std::make_shared<Texture>(_resRef, getTextureProperties(_usage));
    _texture->setPixels(_width, _height, getPixelFormat(), _layers);
    _texture->setFeatures(_features);
}

void TpcReader::getMipMapSize(int index, int &width, int &height) const {
    width = glm::max(1, _width >> index);
    height = glm::max(1, _height >> index);
}

int TpcReader::getMipMapDataSize(int width, int height) const {
    if (_compressed) {
        switch (_encoding) {
        case EncodingType::RGB:
            return glm::max(8, ((width + 3) / 4) * ((height + 3) / 4) * 8);
        case EncodingType::RGBA:
            return glm::max(16, ((width + 3) / 4) * ((height + 3) / 4) * 16);
        default:
            break;
        }
    } else {
        switch (_encoding) {
        case EncodingType::Grayscale:
            return width * height;
        case EncodingType::RGB:
            return 3 * width * height;
        case EncodingType::RGBA:
            return 4 * width * height;
        }
    }

    throw std::logic_error("Unable to compute TPC mip map size");
}

PixelFormat TpcReader::getPixelFormat() const {
    if (!_compressed) {
        switch (_encoding) {
        case EncodingType::Grayscale:
            return PixelFormat::R8;
        case EncodingType::RGB:
            return PixelFormat::RGB8;
        case EncodingType::RGBA:
            return PixelFormat::RGBA8;
        default:
            throw ValidationException("Unsupported uncompressed TPC encoding: " + std::to_string(static_cast<int>(_encoding)));
        }
    } else
        switch (_encoding) {
        case EncodingType::RGB:
            return PixelFormat::DXT1;
        case EncodingType::RGBA:
            return PixelFormat::DXT5;
        default:
            throw ValidationException("Unsupported compressed TPC encoding: " + std::to_string(static_cast<int>(_encoding)));
        }
}

} // namespace graphics

} // namespace reone
