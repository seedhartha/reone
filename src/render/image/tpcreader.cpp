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

#include "tpcreader.h"

#include "glm/glm.hpp"

#include "../../common/streamutil.h"

#include "../textureutil.h"

#include "txireader.h"

using namespace std;

namespace fs = boost::filesystem;

namespace reone {

namespace render {

TpcReader::TpcReader(const string &resRef, TextureUsage usage, bool headless) :
    BinaryReader(0), _resRef(resRef), _usage(usage), _headless(headless) {
}

void TpcReader::doLoad() {
    uint32_t dataSize = readUint32();
    _compressed = dataSize > 0;

    ignore(4);

    _width = readUint16();
    _height = readUint16();
    _encoding = static_cast<EncodingType>(readByte());
    _mipMapCount = readByte();

    _cubeMap = _height / _width == 6;
    if (_cubeMap) _height = _width;

    if (_compressed) {
        _dataSize = dataSize;
    } else {
        int w, h;
        getMipMapSize(0, w, h);
        _dataSize = getMipMapDataSize(w, h);
    }

    loadPixels();
    loadFeatures();

    makeTexture();
}

void TpcReader::getMipMapSize(int index, int &width, int &height) const {
    width = _width >> index;
    height = _height >> index;
}

int TpcReader::getMipMapDataSize(int width, int height) const {
    if (_compressed) {
        switch (_encoding) {
            case EncodingType::RGB:
                return glm::max(8, ((width + 3) / 4) * ((height + 3) / 4) *  8);
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

    throw logic_error("TPC: unable to compute mip map size");
}

void TpcReader::loadPixels() {
    seek(128);

    int layerCount = _cubeMap ? 6 : 1;
    _pixels.reserve(layerCount);

    for (int i = 0; i < layerCount; ++i) {
        vector<Texture::MipMap> mipMaps;
        mipMaps.reserve(_mipMapCount);

        for (int i = 0; i < _mipMapCount; ++i) {
            Texture::MipMap mipMap;
            int dataSize;
            if (i == 0) {
                mipMap.width = _width;
                mipMap.height = _height;
                dataSize = _dataSize;
            } else {
                getMipMapSize(i, mipMap.width, mipMap.height);
                dataSize = getMipMapDataSize(mipMap.width, mipMap.height);
            }
            mipMap.pixels = make_shared<ByteArray>(_reader->getBytes(dataSize));
            mipMaps.push_back(move(mipMap));
        }

        Texture::Layer layer;
        layer.mipMaps = move(mipMaps);

        _pixels.push_back(move(layer));
    }

    PixelFormat format = getPixelFormat();
    if (_cubeMap) {
        prepareCubeMap(_pixels, format, format);
        _compressed = false;
    }
}

void TpcReader::loadFeatures() {
    size_t pos = tell();
    if (pos < _size) {
        _txiData = _reader->getBytes(static_cast<int>(_size - pos));

        TxiReader txi;
        txi.load(wrap(_txiData));

        _features = txi.features();
    }
}

void TpcReader::makeTexture() {
    _texture = make_shared<Texture>(_resRef, getTextureProperties(_usage, _headless));
    if (!_headless) {
        _texture->init();
        _texture->bind();
    }
    _texture->setPixels(_width, _height, getPixelFormat(), move(_pixels));
    _texture->setFeatures(move(_features));
}

PixelFormat TpcReader::getPixelFormat() const {
    if (!_compressed) {
        switch (_encoding) {
            case EncodingType::Grayscale:
                return PixelFormat::Grayscale;
            case EncodingType::RGB:
                return PixelFormat::RGB;
            case EncodingType::RGBA:
                return PixelFormat::RGBA;
            default:
                throw logic_error("TCP: unsupported texture encoding: " + to_string(static_cast<int>(_encoding)));
        }
    } else switch (_encoding) {
        case EncodingType::RGB:
            return PixelFormat::DXT1;
        case EncodingType::RGBA:
            return PixelFormat::DXT5;
        default:
            throw logic_error("TCP: unsupported compressed texture encoding: " + to_string(static_cast<int>(_encoding)));
    }
}

} // namespace render

} // namespace reone
