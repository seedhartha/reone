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

#include "reone/graphics/texture.h"

#include "reone/graphics/pixelutil.h"
#include "reone/graphics/textureutil.h"
#include "reone/system/exception/notimplemented.h"
#include "reone/system/threadutil.h"

namespace reone {

namespace graphics {

static bool isMipmapFilter(Texture::Filtering filter) {
    switch (filter) {
    case Texture::Filtering::NearestMipmapNearest:
    case Texture::Filtering::LinearMipmapNearest:
    case Texture::Filtering::NearestMipmapLinear:
    case Texture::Filtering::LinearMipmapLinear:
        return true;
    default:
        return false;
    }
}

static uint32_t getPixelFormatGL(PixelFormat format) {
    switch (format) {
    case PixelFormat::R8:
    case PixelFormat::R16F:
        return GL_RED;
    case PixelFormat::RG8:
    case PixelFormat::RG16F:
        return GL_RG;
    case PixelFormat::RGB8:
    case PixelFormat::RGB16F:
        return GL_RGB;
    case PixelFormat::RGBA8:
    case PixelFormat::RGBA16F:
    case PixelFormat::DXT1:
    case PixelFormat::DXT5:
        return GL_RGBA;
    case PixelFormat::BGR8:
        return GL_BGR;
    case PixelFormat::BGRA8:
        return GL_BGRA;
    case PixelFormat::Depth24:
    case PixelFormat::Depth32F:
        return GL_DEPTH_COMPONENT;
    case PixelFormat::Depth32FStencil8:
        return GL_DEPTH_STENCIL;
    default:
        throw std::invalid_argument("Invalid pixel format: " + std::to_string(static_cast<int>(format)));
    }
}

static uint32_t getPixelTypeGL(PixelFormat format) {
    switch (format) {
    case PixelFormat::R8:
    case PixelFormat::RG8:
    case PixelFormat::RGB8:
    case PixelFormat::RGBA8:
    case PixelFormat::BGR8:
    case PixelFormat::BGRA8:
        return GL_UNSIGNED_BYTE;
    case PixelFormat::Depth24:
        return GL_UNSIGNED_INT;
    case PixelFormat::R16F:
    case PixelFormat::RG16F:
    case PixelFormat::RGB16F:
    case PixelFormat::RGBA16F:
    case PixelFormat::Depth32F:
        return GL_FLOAT;
    case PixelFormat::Depth32FStencil8:
        return GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
    default:
        throw std::invalid_argument("Invalid pixel format: " + std::to_string(static_cast<int>(format)));
    }
}

static uint32_t getFilterGL(Texture::Filtering filter) {
    switch (filter) {
    case Texture::Filtering::Nearest:
        return GL_NEAREST;
    case Texture::Filtering::NearestMipmapNearest:
        return GL_NEAREST_MIPMAP_NEAREST;
    case Texture::Filtering::LinearMipmapNearest:
        return GL_LINEAR_MIPMAP_NEAREST;
    case Texture::Filtering::NearestMipmapLinear:
        return GL_NEAREST_MIPMAP_LINEAR;
    case Texture::Filtering::LinearMipmapLinear:
        return GL_LINEAR_MIPMAP_LINEAR;
    case Texture::Filtering::Linear:
    default:
        return GL_LINEAR;
    }
}

void Texture::init() {
    if (_inited) {
        return;
    }
    checkMainThread();

    glGenTextures(1, &_nameGL);
    glBindTexture(getTargetGL(), _nameGL);
    configure();
    refresh();

    _inited = true;
}

void Texture::deinit() {
    if (!_inited) {
        return;
    }
    checkMainThread();
    glDeleteTextures(1, &_nameGL);
    _inited = false;
}

void Texture::bind() {
    glBindTexture(getTargetGL(), _nameGL);
}

void Texture::unbind() {
    glBindTexture(getTargetGL(), 0);
}

void Texture::configure() {
    if (isCubeMap() || isCubeMapArray()) {
        configureCubeMap();
    } else if (is2D() || is2DArray()) {
        configure2D();
    } else {
        throw NotImplementedException("Unsupported texture type: " + std::to_string(static_cast<int>(_type)));
    }
}

void Texture::configure2D() {
    auto target = getTargetGL();
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, getFilterGL(_properties.minFilter));
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, getFilterGL(_properties.magFilter));

    switch (_properties.wrap) {
    case Wrapping::ClampToBorder:
        glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, &_properties.borderColor[0]);
        break;
    case Wrapping::ClampToEdge:
        glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        break;
    case Wrapping::Repeat:
    default:
        // Wrap is GL_REPEAT by default in OpenGL
        break;
    }
}

void Texture::configureCubeMap() {
    auto target = getTargetGL();
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, getFilterGL(_properties.minFilter));
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, getFilterGL(_properties.magFilter));

    switch (_properties.wrap) {
    case Wrapping::ClampToBorder:
        glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
        glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, &_properties.borderColor[0]);
        break;
    case Wrapping::ClampToEdge:
        glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        break;
    case Wrapping::Repeat:
    default:
        // Wrap is GL_REPEAT by default in OpenGL
        break;
    }
}

void Texture::refresh() {
    if (isCubeMapArray()) {
        refreshCubeMapArray();
    } else if (isCubeMap()) {
        refreshCubeMap();
    } else if (is2DArray()) {
        refresh2DArray();
    } else if (is2D()) {
        refresh2D();
    } else {
        throw NotImplementedException("Unsupported texture type: " + std::to_string(static_cast<int>(_type)));
    }
    if (isMipmapFilter(_properties.minFilter)) {
        auto target = getTargetGL();
        glGenerateMipmap(target);
        if (_properties.anisotropy > 1.0f) {
            glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, _properties.anisotropy);
        }
    }
}

void Texture::refresh2D() {
    const void *pixelsData;
    size_t pixelsSize;
    if (!_layers.empty() && _layers.front().pixels) {
        auto &pixels = _layers.front().pixels;
        pixelsData = pixels->data();
        pixelsSize = pixels->size();
    } else {
        pixelsData = nullptr;
        pixelsSize = 0;
    }
    switch (_pixelFormat) {
    case PixelFormat::DXT1:
    case PixelFormat::DXT5:
        glCompressedTexImage2D(
            GL_TEXTURE_2D,
            0,
            getInternalPixelFormatGL(_pixelFormat),
            _width, _height,
            0,
            pixelsSize, pixelsData);
        break;
    default:
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            getInternalPixelFormatGL(_pixelFormat),
            _width, _height,
            0,
            getPixelFormatGL(_pixelFormat),
            getPixelTypeGL(_pixelFormat),
            pixelsData);
        break;
    }
}

void Texture::refresh2DArray() {
    int numLayers = static_cast<int>(_layers.size());
    glTexImage3D(
        GL_TEXTURE_2D_ARRAY,
        0,
        getInternalPixelFormatGL(_pixelFormat),
        _width, _height, numLayers,
        0,
        getPixelFormatGL(_pixelFormat),
        getPixelTypeGL(_pixelFormat),
        nullptr);
    for (size_t i = 0; i < numLayers; ++i) {
        const auto &layer = _layers[i];
        if (!layer.pixels || layer.pixels->empty()) {
            continue;
        }
        glTexSubImage3D(
            GL_TEXTURE_2D_ARRAY,
            0,
            0, 0, i,
            _width, _height, 1,
            getPixelFormatGL(_pixelFormat),
            getPixelTypeGL(_pixelFormat),
            layer.pixels->data());
    }
}

void Texture::refreshCubeMap() {
    const void *pixelsData;
    size_t pixelsSize;
    for (int i = 0; i < kNumCubeFaces; ++i) {
        if (_layers.size() > i && _layers[i].pixels) {
            auto &pixels = _layers[i].pixels;
            pixelsData = pixels->data();
            pixelsSize = pixels->size();
        } else {
            pixelsData = nullptr;
            pixelsSize = 0;
        }
        switch (_pixelFormat) {
        case PixelFormat::DXT1:
        case PixelFormat::DXT5:
            glCompressedTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                getInternalPixelFormatGL(_pixelFormat),
                _width, _height,
                0,
                pixelsSize, pixelsData);
            break;
        default:
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                getInternalPixelFormatGL(_pixelFormat),
                _width, _height,
                0,
                getPixelFormatGL(_pixelFormat),
                getPixelTypeGL(_pixelFormat),
                pixelsData);
            break;
        }
    }
}

void Texture::refreshCubeMapArray() {
    // TODO: fill with pixel data
    int numLayers = static_cast<int>(_layers.size());
    glTexImage3D(
        GL_TEXTURE_CUBE_MAP_ARRAY,
        0,
        getInternalPixelFormatGL(_pixelFormat),
        _width, _height, numLayers,
        0,
        getPixelFormatGL(_pixelFormat),
        getPixelTypeGL(_pixelFormat),
        nullptr);
}

void Texture::clear(int w, int h, PixelFormat format, int numLayers, bool refresh) {
    _width = w;
    _height = h;
    _pixelFormat = format;

    _layers.clear();
    _layers.resize(numLayers);

    if (refresh) {
        this->refresh();
    }
}

void Texture::setPixels(int w, int h, PixelFormat format, Layer layer, bool refresh) {
    setPixels(w, h, format, std::vector<Layer> {std::move(layer)}, refresh);
}

void Texture::setPixels(int w, int h, PixelFormat format, std::vector<Layer> layers, bool refresh) {
    if (layers.empty()) {
        throw std::invalid_argument("layers is empty");
    }
    _width = w;
    _height = h;
    _pixelFormat = format;
    _layers = std::move(layers);

    if (refresh) {
        this->refresh();
    }
}

uint32_t Texture::getTargetGL() const {
    if (isCubeMapArray()) {
        return GL_TEXTURE_CUBE_MAP_ARRAY;
    } else if (isCubeMap()) {
        return GL_TEXTURE_CUBE_MAP;
    } else if (is2DArray()) {
        return GL_TEXTURE_2D_ARRAY;
    } else {
        return GL_TEXTURE_2D;
    }
}

void Texture::flushGPUToCPU() {
    if (!is2D()) {
        throw NotImplementedException("Flushing is only supported for 2D textures");
    }
    if (_layers.empty()) {
        _layers.push_back(Texture::Layer());
    }
    auto &layer = _layers.front();
    if (!layer.pixels) {
        layer.pixels = std::make_shared<ByteBuffer>();
    }
    int bpp;
    switch (_pixelFormat) {
    case PixelFormat::R8:
        bpp = 1;
        break;
    case PixelFormat::R16F:
        bpp = 1 * 4;
        break;
    case PixelFormat::RG8:
        bpp = 2 * 1;
        break;
    case PixelFormat::RG16F:
        bpp = 2 * 4;
        break;
    case PixelFormat::RGB8:
    case PixelFormat::BGR8:
        bpp = 3 * 1;
        break;
    case PixelFormat::RGB16F:
        bpp = 3 * 4;
        break;
    case PixelFormat::RGBA8:
    case PixelFormat::BGRA8:
        bpp = 4 * 1;
        break;
    case PixelFormat::RGBA16F:
        bpp = 4 * 4;
        break;
    default:
        throw NotImplementedException(str(boost::format("Flushing texture of pixel format %d not implemented") % static_cast<int>(_pixelFormat)));
    }
    layer.pixels->resize(bpp * _width * _height);

    glGetTexImage(GL_TEXTURE_2D, 0, getPixelFormatGL(_pixelFormat), getPixelTypeGL(_pixelFormat), &(*layer.pixels)[0]);
}

} // namespace graphics

} // namespace reone
