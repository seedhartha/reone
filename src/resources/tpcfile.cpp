#include "tpcfile.h"

#include "../core/streamutil.h"

#include "txifile.h"

using namespace reone::render;

namespace fs = boost::filesystem;

namespace reone {

namespace resources {

TpcFile::TpcFile(const std::string &resRef, TextureType type) : BinaryFile(0), _resRef(resRef), _type(type) {
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

    _dataSize = _compressed ? dataSize : getMipMapSize(0);

    loadTexture();
}

int TpcFile::getMipMapSize(int idx) {
    int width = _width >> idx;
    int height = _height >> idx;

    if (_compressed) {
        switch (_encoding) {
            case EncodingType::RGB:
                return glm::max(8, ((width + 3) / 4) * ((height + 3) / 4) *  8);
            case EncodingType::RGBA:
                return glm::max(16, ((width + 3) / 4) * ((height + 3) / 4) * 16);
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

    throw std::logic_error("TPC: unable to compute mip map size");
}

void TpcFile::loadTexture() {
    seek(128);

    _texture = std::make_shared<Texture>(_resRef, _type);
    _texture->_width = _width;
    _texture->_height = _height;
    _texture->_cubeMap = _cubeMap;
    _texture->_pixelFormat = pixelFormat();
    _texture->_images.resize(_cubeMap ? 6 : 1);

    for (auto &image : _texture->_images) {
        image.resize(_dataSize);

        _in->read(&image[0], _dataSize);

        for (int i = 1; i < _mipMapCount; ++i) {
            int mipMapDataSize = getMipMapSize(i);
            ignore(mipMapDataSize);
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
                throw std::logic_error("TCP: unsupported texture encoding: " + std::to_string(static_cast<int>(_encoding)));
        }
    } else switch (_encoding) {
        case EncodingType::RGB:
            return PixelFormat::DXT1;
        case EncodingType::RGBA:
            return PixelFormat::DXT5;
        default:
            throw std::logic_error("TCP: unsupported compressed texture encoding: " + std::to_string(static_cast<int>(_encoding)));
    }
}

std::shared_ptr<Texture> TpcFile::texture() const {
    return _texture;
}

} // namespace resources

} // namespace reone
