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

#include "tpcfile.h"

#include "../../core/streamutil.h"

#include "txifile.h"

using namespace std;

using namespace reone::render;

namespace fs = boost::filesystem;

namespace reone {

namespace resource {

TpcFile::TpcFile(const string &resRef, TextureType type) : BinaryFile(0), _resRef(resRef), _type(type) {
}

void TpcFile::doLoad() {
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

    loadTexture();
}

void TpcFile::getMipMapSize(int index, int &width, int &height) const {
    width = _width >> index;
    height = _height >> index;
}

int TpcFile::getMipMapDataSize(int width, int height) const {
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

void TpcFile::loadTexture() {
    _texture = make_shared<Texture>(_resRef, _type);
    _texture->_width = _width;
    _texture->_height = _height;
    _texture->_pixelFormat = pixelFormat();
    _texture->_layers.resize(_cubeMap ? 6 : 1);

    seek(128);

    for (auto &layer : _texture->_layers) {
        layer.mipMaps.resize(_mipMapCount);

        for (int j = 0; j < _mipMapCount; ++j) {
            Texture::MipMap &mipMap = layer.mipMaps[j];
            int dataSize;
            if (j == 0) {
                mipMap.width = _width;
                mipMap.height = _height;
                dataSize = _dataSize;
            } else {
                getMipMapSize(j, mipMap.width, mipMap.height);
                dataSize = getMipMapDataSize(mipMap.width, mipMap.height);
            }
            ByteArray &data = mipMap.data;
            data.resize(dataSize);

            _in->read(&data[0], dataSize);
        }
    }

    uint32_t pos = tell();

    if (pos < _size) {
        ByteArray data(_size - pos);
        _in->read(&data[0], data.size());

        TxiFile txi;
        txi.load(wrap(data));

        _texture->_features = txi.features();
    }
}

PixelFormat TpcFile::pixelFormat() const {
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

shared_ptr<Texture> TpcFile::texture() const {
    return _texture;
}

} // namespace resources

} // namespace reone
