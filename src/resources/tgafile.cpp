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
