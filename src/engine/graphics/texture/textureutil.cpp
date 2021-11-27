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

#include "dxtutil.h"

using namespace std;

namespace reone {

namespace graphics {

Texture::Properties getTextureProperties(TextureUsage usage, bool headless) {
    Texture::Properties properties;
    properties.headless = headless;

    if (usage == TextureUsage::GUI ||
        usage == TextureUsage::ColorBuffer ||
        usage == TextureUsage::IrradianceMap ||
        usage == TextureUsage::BRDFLookup ||
        usage == TextureUsage::Video) {

        properties.minFilter = Texture::Filtering::Linear;
        properties.wrap = Texture::Wrapping::ClampToEdge;

    } else if (usage == TextureUsage::EnvironmentMap || usage == TextureUsage::PrefilterMap) {
        properties.wrap = Texture::Wrapping::ClampToEdge;

    } else if (usage == TextureUsage::DepthBuffer || usage == TextureUsage::CubeMapDepthBuffer) {
        properties.minFilter = Texture::Filtering::Nearest;
        properties.maxFilter = Texture::Filtering::Nearest;
        properties.wrap = Texture::Wrapping::ClampToBorder;
        properties.borderColor = glm::vec4(1.0f);

    } else if (usage == TextureUsage::NormalMap || usage == TextureUsage::HeightMap) {
        properties.minFilter = Texture::Filtering::Linear;
    }

    if (usage == TextureUsage::CubeMapDefault ||
        usage == TextureUsage::EnvironmentMap ||
        usage == TextureUsage::IrradianceMap ||
        usage == TextureUsage::PrefilterMap ||
        usage == TextureUsage::CubeMapDepthBuffer) {

        properties.cubemap = true;
    }

    return move(properties);
}

static int getBitsPerPixel(PixelFormat format) {
    switch (format) {
    case PixelFormat::Grayscale:
        return 1;
    case PixelFormat::RGB:
    case PixelFormat::BGR:
        return 3;
    case PixelFormat::RGBA:
    case PixelFormat::BGRA:
        return 4;
    default:
        throw invalid_argument("Unsupported pixel format: " + to_string(static_cast<int>(format)));
    }
}

void prepareCubeMap(vector<Texture::Layer> &layers, PixelFormat srcFormat, PixelFormat &destFormat) {
    static int rotations[] = {1, 3, 0, 2, 2, 0};

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

void decompressMipMap(Texture::MipMap &mipMap, PixelFormat srcFormat, PixelFormat &destFormat) {
    if (!isCompressed(srcFormat)) {
        throw invalid_argument("format must be either DXT1 or DXT5");
    }

    size_t pixelCount = static_cast<size_t>(mipMap.width) * mipMap.height;
    const uint8_t *srcPixels = reinterpret_cast<const uint8_t *>(mipMap.pixels->data());
    vector<uint32_t> decompPixels(pixelCount);
    uint32_t *decompPixelsPtr = &decompPixels[0];
    bool alpha;

    if (srcFormat == PixelFormat::DXT5) {
        decompressDXT5(mipMap.width, mipMap.height, srcPixels, decompPixelsPtr);
        alpha = true;
    } else {
        decompressDXT1(mipMap.width, mipMap.height, srcPixels, decompPixelsPtr);
        alpha = false;
    }

    auto destPixels = make_shared<ByteArray>((alpha ? 4ll : 3ll) * pixelCount);
    uint8_t *destPixelsPtr = reinterpret_cast<uint8_t *>(destPixels->data());
    decompPixelsPtr = &decompPixels[0];

    for (int i = 0; i < mipMap.width * mipMap.height; ++i) {
        unsigned long pixel = *(decompPixelsPtr++);
        *(destPixelsPtr++) = (pixel >> 24) & 0xff;
        *(destPixelsPtr++) = (pixel >> 16) & 0xff;
        *(destPixelsPtr++) = (pixel >> 8) & 0xff;
        if (alpha) {
            *(destPixelsPtr++) = pixel & 0xff;
        }
    }

    mipMap.pixels = move(destPixels);
    destFormat = alpha ? PixelFormat::RGBA : PixelFormat::RGB;
}

void rotateMipMap90(Texture::MipMap &mipMap, int bpp) {
    if (mipMap.width != mipMap.height) {
        throw invalid_argument("mipMap size is invalid");
    }
    size_t n = mipMap.width;
    size_t w = n / 2;
    size_t h = (n + 1) / 2;
    uint8_t *pixels = reinterpret_cast<uint8_t *>(mipMap.pixels->data());

    for (size_t x = 0; x < w; ++x) {
        for (size_t y = 0; y < h; ++y) {
            const size_t d0 = (y * n + x) * bpp;
            const size_t d1 = ((n - 1 - x) * n + y) * bpp;
            const size_t d2 = ((n - 1 - y) * n + (n - 1 - x)) * bpp;
            const size_t d3 = (x * n + (n - 1 - y)) * bpp;

            for (size_t p = 0; p < static_cast<size_t>(bpp); ++p) {
                uint8_t tmp = pixels[d0 + p];
                pixels[d0 + p] = pixels[d1 + p];
                pixels[d1 + p] = pixels[d2 + p];
                pixels[d2 + p] = pixels[d3 + p];
                pixels[d3 + p] = tmp;
            }
        }
    }
}

} // namespace graphics

} // namespace reone
