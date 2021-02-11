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

#include "texture.h"

#include <stdexcept>

#include "GL/glew.h"

#include "SDL2/SDL_opengl.h"

using namespace std;

namespace reone {

namespace render {

Texture::Texture(string name, TextureType type, int w, int h, bool headless) :
    _name(move(name)), _type(type), _width(w), _height(h), _headless(headless) {
}

void Texture::init() {
    if (_inited) return;

    glGenTextures(1, &_textureId);

    if (isCubeMap()) {
        configureCubeMap();
    } else {
        configure2D();
    }

    _inited = true;
}

bool Texture::isCubeMap() const {
    return
        _type == TextureType::EnvironmentMap ||
        _type == TextureType::IrradianceMap ||
        _type == TextureType::CubeMapDepthBuffer;
}

void Texture::configureCubeMap() {
    glBindTexture(GL_TEXTURE_CUBE_MAP, _textureId);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, getMinFilter());
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, getMagFilter());

    if (_type == TextureType::CubeMapDepthBuffer) {
        static float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
        glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, borderColor);
    } else {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
}

void Texture::configure2D() {
    glBindTexture(GL_TEXTURE_2D, _textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getMinFilter());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getMagFilter());

    switch (_type) {
        case TextureType::GUI:
        case TextureType::ColorBuffer:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            break;
        case TextureType::DepthBuffer: {
            static float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
            break;
        }
        default:
            break;
    }
}

int Texture::getMinFilter() const {
    if (isDepthBuffer()) return GL_NEAREST;

    return hasMipMaps() ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
}

bool Texture::hasMipMaps() const {
    return
        _type != TextureType::GUI &&
        _type != TextureType::Cursor &&
        _type != TextureType::ColorBuffer &&
        _type != TextureType::DepthBuffer &&
        _type != TextureType::EnvironmentMap &&
        _type != TextureType::IrradianceMap;
}

bool Texture::isDepthBuffer() const {
    return _type == TextureType::DepthBuffer || _type == TextureType::CubeMapDepthBuffer;
}

int Texture::getMagFilter() const {
    return isDepthBuffer() ? GL_NEAREST : GL_LINEAR;
}

Texture::~Texture() {
    deinit();
}

void Texture::deinit() {
    if (!_inited) return;

    glDeleteTextures(1, &_textureId);

    _inited = false;
}

void Texture::clearPixels(PixelFormat format) {
    if (!_headless && !_inited) {
        throw logic_error("Texture has not been initialized: " + _name);
    }
    _layers.clear();
    _pixelFormat = format;

    if (!_headless) {
        refresh();
    }
}

void Texture::refresh() {
    if (isCubeMap()) {
        refreshCubeMap();
    } else {
        refresh2D();
    }
}

void Texture::refreshCubeMap() {
    glBindTexture(GL_TEXTURE_CUBE_MAP, _textureId);

    for (int i = 0; i < kNumCubeFaces; ++i) {
        if (i < _layers.size()) {
            const MipMap &mipMap = _layers[i].mipMaps.front();
            fillTarget(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, mipMap.width, mipMap.height, &mipMap.data[0], mipMap.data.size());
        } else {
            fillTarget(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, _width, _height);
        }
    }
}

void Texture::fillTarget(uint32_t target, int level, int width, int height, const void *pixels, int size) {
    switch (_pixelFormat) {
        case PixelFormat::Grayscale:
        case PixelFormat::RGB:
        case PixelFormat::RGBA:
        case PixelFormat::BGR:
        case PixelFormat::BGRA:
            glTexImage2D(target, level, getInternalPixelFormat(), width, height, 0, getPixelFormat(), GL_UNSIGNED_BYTE, pixels);
            break;
        case PixelFormat::DXT1:
        case PixelFormat::DXT5:
            glCompressedTexImage2D(target, level, getInternalPixelFormat(), width, height, 0, size, pixels);
            break;
        case PixelFormat::Depth:
            glTexImage2D(target, level, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, pixels);
            break;
        default:
            throw logic_error("Unsupported pixel format: " + to_string(static_cast<int>(_pixelFormat)));
    }
}

int Texture::getInternalPixelFormat() const {
    switch (_pixelFormat) {
        case PixelFormat::Grayscale:
            return GL_RED;
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
            throw logic_error("Unsupported pixel format: " + to_string(static_cast<int>(_pixelFormat)));
    }
}

uint32_t Texture::getPixelFormat() const {
    switch (_pixelFormat) {
        case PixelFormat::Grayscale:
            return GL_RED;
        case PixelFormat::RGB:
            return GL_RGB;
        case PixelFormat::RGBA:
        case PixelFormat::DXT1:
        case PixelFormat::DXT5:
            return GL_RGBA;
        case PixelFormat::BGR:
            return GL_BGR;
        case PixelFormat::BGRA:
            return GL_BGRA;
        default:
            throw logic_error("Unsupported pixel format: " + to_string(static_cast<int>(_pixelFormat)));
    }
}

void Texture::refresh2D() {
    glBindTexture(GL_TEXTURE_2D, _textureId);

    if (!_layers.empty()) {
        const Layer &layer = _layers.front();
        int mipMapCount = layer.mipMaps.size();
        if (mipMapCount > 1) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipMapCount - 1);
        }
        for (int i = 0; i < layer.mipMaps.size(); ++i) {
            const MipMap &mipMap = layer.mipMaps[i];
            fillTarget(GL_TEXTURE_2D, i, mipMap.width, mipMap.height, &mipMap.data[0], mipMap.data.size());
        }
        if (mipMapCount == 1 && hasMipMaps()) {
            glGenerateMipmap(GL_TEXTURE_2D);
        }
    } else {
        fillTarget(GL_TEXTURE_2D, 0, _width, _height);
    }
}

void Texture::bind() const {
    glBindTexture(isCubeMap() ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, _textureId);
}

void Texture::unbind() const {
    glBindTexture(isCubeMap() ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, 0);
}

bool Texture::isAdditive() const {
    return _features.blending == TextureBlending::Additive;
}

bool Texture::isGrayscale() const {
    return _pixelFormat == PixelFormat::Grayscale;
}

void Texture::setPixels(vector<Layer> layers, PixelFormat format) {
    if (!_headless && !_inited) {
        throw logic_error("Texture has not been initialized: " + _name);
    }
    if (layers.empty()) {
        throw invalid_argument("layers is empty");
    }
    _layers = move(layers);
    _pixelFormat = format;

    if (!_headless) {
        refresh();
    }
}

void Texture::setFeatures(TextureFeatures features) {
    _features = move(features);
}

} // namespace render

} // namespace reone
