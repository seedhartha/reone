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

#include "textureutil.h"

#include <stdexcept>
#include <string>

#include "glm/vec4.hpp"

#include "s3tc.h"

using namespace std;

namespace reone {

namespace render {

Texture::Properties getTextureProperties(TextureUsage usage, bool headless) {
    Texture::Properties properties;
    properties.headless = headless;

    if (usage == TextureUsage::GUI || usage == TextureUsage::ColorBuffer || usage == TextureUsage::IrradianceMap || usage == TextureUsage::BRDFLookup) {
        properties.minFilter = Texture::Filtering::Linear;
        properties.wrap = Texture::Wrapping::ClampToEdge;

    } else if (usage == TextureUsage::EnvironmentMap || usage == TextureUsage::PrefilterMap) {
        properties.wrap = Texture::Wrapping::ClampToEdge;

    } else if (usage == TextureUsage::DepthBuffer || usage == TextureUsage::CubeMapDepthBuffer) {
        properties.minFilter = Texture::Filtering::Nearest;
        properties.maxFilter = Texture::Filtering::Nearest;
        properties.wrap = Texture::Wrapping::ClampToBorder;
        properties.borderColor = glm::vec4(1.0f);
    }

    if (usage == TextureUsage::EnvironmentMap ||
        usage == TextureUsage::IrradianceMap ||
        usage == TextureUsage::PrefilterMap ||
        usage == TextureUsage::CubeMapDepthBuffer) {

        properties.cubemap = true;
    }

    return move(properties);
}

static bool isCompressed(Texture::PixelFormat format) {
    return format == Texture::PixelFormat::DXT1 || format == Texture::PixelFormat::DXT5;
}

static int getBitsPerPixel(Texture::PixelFormat format) {
    switch (format) {
        case Texture::PixelFormat::Grayscale:
            return 1;
        case Texture::PixelFormat::RGB:
        case Texture::PixelFormat::BGR:
            return 3;
        case Texture::PixelFormat::RGBA:
        case Texture::PixelFormat::BGRA:
            return 4;
        default:
            throw invalid_argument("Unsupported pixel format: " + to_string(static_cast<int>(format)));
    }
}

void prepareCubeMap(vector<Texture::Layer> &layers, Texture::PixelFormat srcFormat, Texture::PixelFormat &destFormat) {
    static int rotations[] = { 1, 3, 0, 2, 2, 0 };

    int layerCount = static_cast<int>(layers.size());
    if (layerCount != kNumCubeFaces) {
        throw invalid_argument("layer count is invalid");
    }

    swap(layers[0], layers[1]);

    destFormat = srcFormat;

    bool compressed = isCompressed(srcFormat);
    for (int i = 0; i < kNumCubeFaces; ++i) {
        Texture::Layer &layer = layers[i];

        if (layer.mipMaps.empty()) {
            throw invalid_argument("layer has no mip maps: " + to_string(i));
        }

        // Cube maps only ever use the base mip map level
        Texture::MipMap &mipMap = layer.mipMaps.front();
        if (compressed) {
            decompressMipMap(mipMap, srcFormat, destFormat);
        }
        for (int j = 0; j < rotations[i]; ++j) {
            rotateMipMap90(mipMap, getBitsPerPixel(destFormat));
        }
        layer.mipMaps.erase(layer.mipMaps.begin() + 1, layer.mipMaps.end());
    }
}

void decompressMipMap(Texture::MipMap &mipMap, Texture::PixelFormat srcFormat, Texture::PixelFormat &destFormat) {
    if (!isCompressed(srcFormat)) {
        throw invalid_argument("format must be either DXT1 or DXT5");
    }

    const uint8_t *srcMipMapData = reinterpret_cast<const uint8_t *>(&mipMap.data[0]);
    size_t pixelCount = static_cast<size_t>(mipMap.width) * mipMap.height;
    vector<unsigned long> pixels(pixelCount);
    unsigned long *pixelsPtr = &pixels[0];
    bool alpha;

    if (srcFormat == Texture::PixelFormat::DXT5) {
        BlockDecompressImageDXT5(mipMap.width, mipMap.height, srcMipMapData, pixelsPtr);
        alpha = true;
    } else {
        BlockDecompressImageDXT1(mipMap.width, mipMap.height, srcMipMapData, pixelsPtr);
        alpha = false;
    }

    mipMap.data.resize((alpha ? 4ll : 3ll) * pixelCount);
    pixelsPtr = &pixels[0];
    uint8_t *destMipMapData = reinterpret_cast<uint8_t *>(&mipMap.data[0]);

    for (int i = 0; i < mipMap.width * mipMap.height; ++i) {
        unsigned long pixel = *(pixelsPtr++);
        *(destMipMapData++) = (pixel >> 24) & 0xff;
        *(destMipMapData++) = (pixel >> 16) & 0xff;
        *(destMipMapData++) = (pixel >> 8) & 0xff;
        if (alpha) {
            *(destMipMapData++) = pixel & 0xff;
        }
    }

    destFormat = alpha ? Texture::PixelFormat::RGBA : Texture::PixelFormat::RGB;
}

void rotateMipMap90(Texture::MipMap &mipMap, int bpp) {
    if (mipMap.width != mipMap.height) {
        throw invalid_argument("mipMap size is invalid");
    }
    size_t n = mipMap.width;
    size_t w = n / 2;
    size_t h = (n + 1) / 2;
    uint8_t *mipMapData = reinterpret_cast<uint8_t *>(&mipMap.data[0]);

    for (size_t x = 0; x < w; ++x) {
        for (size_t y = 0; y < h; ++y) {
            const size_t d0 = ( y          * n +  x         ) * bpp;
            const size_t d1 = ((n - 1 - x) * n +  y         ) * bpp;
            const size_t d2 = ((n - 1 - y) * n + (n - 1 - x)) * bpp;
            const size_t d3 = ( x          * n + (n - 1 - y)) * bpp;

            for (size_t p = 0; p < static_cast<size_t>(bpp); ++p) {
                uint8_t tmp = mipMapData[d0 + p];
                mipMapData[d0 + p] = mipMapData[d1 + p];
                mipMapData[d1 + p] = mipMapData[d2 + p];
                mipMapData[d2 + p] = mipMapData[d3 + p];
                mipMapData[d3 + p] = tmp;
            }
        }
    }
}

} // namespace render

} // namespace reone
