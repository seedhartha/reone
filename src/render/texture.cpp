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

#include "texture.h"

#include <stdexcept>

#include "GL/glew.h"

#include "SDL2/SDL_opengl.h"

namespace reone {

namespace render {

Texture::Texture(const std::string &name, TextureType type) : _name(name), _type(type) {
}

void Texture::initGL() {
    if (_glInited) return;

    glGenTextures(1, &_textureId);

    if (_cubeMap) {
        glBindTexture(GL_TEXTURE_CUBE_MAP, _textureId);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        for (int i = 0; i < 6; ++i) {
            fillTextureTarget(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, _images[i]);
        }

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    } else {
        glBindTexture(GL_TEXTURE_2D, _textureId);

        if (_type == TextureType::Font) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        } else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }

        fillTextureTarget(GL_TEXTURE_2D, _images.front());

        if (_type != TextureType::Font) {
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    _glInited = true;
}

void Texture::fillTextureTarget(unsigned int target, const ByteArray &data) {
    switch (_pixelFormat) {
        case PixelFormat::Grayscale:
        case PixelFormat::RGB:
        case PixelFormat::RGBA:
        case PixelFormat::BGR:
        case PixelFormat::BGRA:
            glTexImage2D(target, 0, glInternalPixelFormat(), _width, _height, 0, glPixelFormat(), GL_UNSIGNED_BYTE, &data[0]);
            break;

        case PixelFormat::DXT1:
        case PixelFormat::DXT5:
            glCompressedTexImage2D(target, 0, glInternalPixelFormat(), _width, _height, 0, data.size(), &data[0]);
            break;
    }
}

int Texture::glInternalPixelFormat() const {
    switch (_pixelFormat) {
        case PixelFormat::Grayscale:
            return GL_LUMINANCE;
        case PixelFormat::RGB:
        case PixelFormat::BGR:
            return GL_RGB8;
        case PixelFormat::RGBA:
        case PixelFormat::BGRA:
            return GL_RGBA8;
        case PixelFormat::DXT1:
            return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
        case PixelFormat::DXT5:
            return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        default:
            throw std::runtime_error("Unsupported pixel format: " + std::to_string(static_cast<int>(_pixelFormat)));
    }
}

unsigned int Texture::glPixelFormat() const {
    switch (_pixelFormat) {
        case PixelFormat::RGB:
            return GL_RGB;
        case PixelFormat::Grayscale:
        case PixelFormat::RGBA:
        case PixelFormat::DXT1:
        case PixelFormat::DXT5:
            return GL_RGBA;
        case PixelFormat::BGR:
            return GL_BGR;
        case PixelFormat::BGRA:
            return GL_BGRA;

        default:
            throw std::runtime_error("Unsupported pixel format: " + std::to_string(static_cast<int>(_pixelFormat)));
    }
}

Texture::~Texture() {
    deinitGL();
}

void Texture::deinitGL() {
    if (!_glInited) return;

    glDeleteTextures(1, &_textureId);

    _glInited = false;
}

void Texture::bind() {
    assert(_glInited);
    glBindTexture(_cubeMap ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, _textureId);
}

void Texture::unbind() {
    assert(_glInited);
    glBindTexture(_cubeMap ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, 0);
}

bool Texture::isAdditive() const {
    return _features.blending == TextureBlending::Additive;
}

const std::string &Texture::name() const {
    return _name;
}

int Texture::width() const {
    return _width;
}

int Texture::height() const {
    return _height;
}

PixelFormat Texture::pixelFormat() const {
    return _pixelFormat;
}

const TextureFeatures &Texture::features() const {
    return _features;
}

} // namespace render

} // namespace reone
