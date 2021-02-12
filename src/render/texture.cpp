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
#include <string>

#include "GL/glew.h"
#include "SDL2/SDL_opengl.h"

using namespace std;

namespace reone {

namespace render {

Texture::Texture(string name, Properties properties) :
    _name(move(name)),
    _properties(move(properties)) {
}

void Texture::init() {
    if (!_inited) {
        glGenTextures(1, &_textureId);
        bind();

        if (isCubeMap()) {
            configureCubeMap();
        } else {
            configure2D();
        }

        unbind();
        _inited = true;
    }
}

bool Texture::isCubeMap() const {
    return _properties.cubemap;
}

void Texture::configureCubeMap() {
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, getFilterGL(_properties.minFilter));
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, getFilterGL(_properties.maxFilter));

    switch (_properties.wrap) {
        case Wrapping::ClampToBorder:
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
            glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, &_properties.borderColor[0]);
            break;
        case Wrapping::ClampToEdge:
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            break;
        case Wrapping::Repeat:
        default:
            // Wrap is GL_REPEAT by default in OpenGL
            break;
    }
}

uint32_t Texture::getFilterGL(Filtering filter) const {
    switch (filter) {
        case Filtering::Nearest:
            return GL_NEAREST;
        case Filtering::NearestMipmapNearest:
            return GL_NEAREST_MIPMAP_NEAREST;
        case Filtering::LinearMipmapNearest:
            return GL_LINEAR_MIPMAP_NEAREST;
        case Filtering::NearestMipmapLinear:
            return GL_NEAREST_MIPMAP_LINEAR;
        case Filtering::LinearMipmapLinear:
            return GL_LINEAR_MIPMAP_LINEAR;
        case Filtering::Linear:
        default:
            return GL_LINEAR;
    }
}

void Texture::configure2D() {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getFilterGL(_properties.minFilter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getFilterGL(_properties.maxFilter));

    switch (_properties.wrap) {
        case Wrapping::ClampToBorder:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &_properties.borderColor[0]);
            break;
        case Wrapping::ClampToEdge:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            break;
        case Wrapping::Repeat:
        default:
            // Wrap is GL_REPEAT by default in OpenGL
            break;
    }
}

Texture::~Texture() {
    deinit();
}

void Texture::deinit() {
    if (_inited) {
        glDeleteTextures(1, &_textureId);
        _inited = false;
    }
}

void Texture::clearPixels(int w, int h, PixelFormat format) {
    if (!_properties.headless && !_inited) {
        throw logic_error("Texture has not been initialized: " + _name);
    }
    _width = w;
    _height = h;
    _pixelFormat = format;
    _layers.clear();

    if (!_properties.headless) {
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
    for (int i = 0; i < kNumCubeFaces; ++i) {
        if (i < _layers.size()) {
            const MipMap &mipMap = _layers[i].mipMaps.front();
            fillTarget(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, mipMap.width, mipMap.height, &mipMap.data[0], mipMap.data.size());
        } else {
            fillTarget(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, _width, _height);
        }
    }

    // Generate mip maps, if required
    if (isMipmapFilter(_properties.minFilter)) {
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }
}

void Texture::fillTarget(uint32_t target, int level, int width, int height, const void *pixels, int size) {
    switch (_pixelFormat) {
        case PixelFormat::DXT1:
        case PixelFormat::DXT5:
            glCompressedTexImage2D(target, level, getPixelInternalFormatGL(), width, height, 0, size, pixels);
            break;
        case PixelFormat::Grayscale:
        case PixelFormat::RGB:
        case PixelFormat::RGBA:
        case PixelFormat::BGR:
        case PixelFormat::BGRA:
        case PixelFormat::Depth:
        case PixelFormat::RG16F:
        case PixelFormat::RGB16F:
            glTexImage2D(target, level, getPixelInternalFormatGL(), width, height, 0, getPixelFormatGL(), getPixelTypeGL(), pixels);
            break;
        default:
            break;
    }
}

int Texture::getPixelInternalFormatGL() const {
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
        case PixelFormat::Depth:
            return GL_DEPTH_COMPONENT;
        case PixelFormat::RG16F:
            return GL_RG16F;
        case PixelFormat::RGB16F:
            return GL_RGB16F;
        default:
            throw logic_error("Unsupported pixel format: " + to_string(static_cast<int>(_pixelFormat)));
    }
}

uint32_t Texture::getPixelFormatGL() const {
    switch (_pixelFormat) {
        case PixelFormat::Grayscale:
            return GL_RED;
        case PixelFormat::RGB:
        case PixelFormat::RGB16F:
            return GL_RGB;
        case PixelFormat::RGBA:
        case PixelFormat::DXT1:
        case PixelFormat::DXT5:
            return GL_RGBA;
        case PixelFormat::BGR:
            return GL_BGR;
        case PixelFormat::BGRA:
            return GL_BGRA;
        case PixelFormat::Depth:
            return GL_DEPTH_COMPONENT;
        case PixelFormat::RG16F:
            return GL_RG;
        default:
            throw logic_error("Unsupported pixel format: " + to_string(static_cast<int>(_pixelFormat)));
    }
}

uint32_t Texture::getPixelTypeGL() const {
    switch (_pixelFormat) {
        case PixelFormat::Grayscale:
        case PixelFormat::RGB:
        case PixelFormat::RGBA:
        case PixelFormat::BGR:
        case PixelFormat::BGRA:
            return GL_UNSIGNED_BYTE;
        case PixelFormat::Depth:
        case PixelFormat::RG16F:
        case PixelFormat::RGB16F:
            return GL_FLOAT;
        default:
            throw logic_error("Unsupported pixel format: " + to_string(static_cast<int>(_pixelFormat)));
    }
}

bool Texture::isMipmapFilter(Filtering filter) const {
    switch (filter) {
        case Filtering::NearestMipmapNearest:
        case Filtering::LinearMipmapNearest:
        case Filtering::NearestMipmapLinear:
        case Filtering::LinearMipmapLinear:
            return true;
        default:
            return false;
    }
}

void Texture::refresh2D() {
    int numMipMaps = 0;

    if (!_layers.empty()) {
        numMipMaps = _layers[0].mipMaps.size();
        for (int i = 0; i < numMipMaps; ++i) {
            const MipMap &mipMap = _layers[0].mipMaps[i];
            fillTarget(GL_TEXTURE_2D, i, mipMap.width, mipMap.height, &mipMap.data[0], mipMap.data.size());
        }
    } else {
        fillTarget(GL_TEXTURE_2D, 0, _width, _height);
    }

    if (numMipMaps > 1) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, numMipMaps - 1);

    } else if (isMipmapFilter(_properties.minFilter)) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
}

void Texture::bind() const {
    glBindTexture(isCubeMap() ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, _textureId);
}

void Texture::unbind() const {
    glBindTexture(isCubeMap() ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, 0);
}

bool Texture::isAdditive() const {
    return _features.blending == Blending::Additive;
}

bool Texture::isGrayscale() const {
    return _pixelFormat == PixelFormat::Grayscale;
}

void Texture::setPixels(int w, int h, PixelFormat format, vector<Layer> layers) {
    if (!_properties.headless && !_inited) {
        throw logic_error("Texture has not been initialized: " + _name);
    }
    if (layers.empty()) {
        throw invalid_argument("layers is empty");
    }
    _width = w;
    _height = h;
    _pixelFormat = format;
    _layers = move(layers);

    if (!_properties.headless) {
        refresh();
    }
}

void Texture::setFeatures(Features features) {
    _features = move(features);
}

} // namespace render

} // namespace reone
