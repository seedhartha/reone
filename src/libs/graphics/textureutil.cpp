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

#include "reone/graphics/textureutil.h"

#include "reone/graphics/dxtutil.h"

namespace reone {

namespace graphics {

static void decompressLayer(int width, int height, Texture::Layer &layer, PixelFormat srcFormat, PixelFormat &dstFormat) {
    if (!isCompressed(srcFormat)) {
        throw std::invalid_argument("format must be either DXT1 or DXT5");
    }

    size_t numPixels = static_cast<size_t>(width) * height;
    const uint8_t *srcPixels = reinterpret_cast<const uint8_t *>(layer.pixels->data());
    std::vector<uint32_t> decompPixels(numPixels);
    uint32_t *decompPixelsPtr = &decompPixels[0];
    bool alpha;

    if (srcFormat == PixelFormat::DXT5) {
        decompressDXT5(width, height, srcPixels, decompPixelsPtr);
        alpha = true;
    } else {
        decompressDXT1(width, height, srcPixels, decompPixelsPtr);
        alpha = false;
    }

    auto destPixels = std::make_shared<ByteBuffer>((alpha ? 4ll : 3ll) * numPixels, '\0');
    uint8_t *destPixelsPtr = reinterpret_cast<uint8_t *>(destPixels->data());
    decompPixelsPtr = &decompPixels[0];

    for (size_t i = 0; i < numPixels; ++i) {
        unsigned long pixel = *(decompPixelsPtr++);
        *(destPixelsPtr++) = (pixel >> 24) & 0xff;
        *(destPixelsPtr++) = (pixel >> 16) & 0xff;
        *(destPixelsPtr++) = (pixel >> 8) & 0xff;
        if (alpha) {
            *(destPixelsPtr++) = pixel & 0xff;
        }
    }

    layer.pixels = std::move(destPixels);
    dstFormat = alpha ? PixelFormat::RGBA8 : PixelFormat::RGB8;
}

static void rotateLayer90(int width, int height, Texture::Layer &layer, int bpp) {
    if (width != height) {
        throw std::invalid_argument(str(boost::format("Invalid texture size: width=%d, height=%d") % width % height));
    }
    size_t n = width;
    size_t w = n / 2;
    size_t h = (n + 1) / 2;
    uint8_t *pixels = reinterpret_cast<uint8_t *>(layer.pixels->data());

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

static int getBitsPerPixel(PixelFormat format) {
    switch (format) {
    case PixelFormat::R8:
        return 1;
    case PixelFormat::RGB8:
    case PixelFormat::BGR8:
        return 3;
    case PixelFormat::RGBA8:
    case PixelFormat::BGRA8:
        return 4;
    default:
        throw std::invalid_argument("Unsupported pixel format: " + std::to_string(static_cast<int>(format)));
    }
}

void prepareCubemap(Texture &texture) {
    static constexpr int rotations[] = {1, 3, 0, 2, 2, 0};

    PixelFormat srcFormat = texture.pixelFormat();
    PixelFormat dstFormat = texture.pixelFormat();
    bool compressed = isCompressed(srcFormat);

    auto &layers = texture.layers();
    int numLayers = static_cast<int>(layers.size());
    if (numLayers == kNumCubeFaces) {
        std::swap(layers[0], layers[1]);
        for (int i = 0; i < kNumCubeFaces; ++i) {
            auto &layer = layers[i];
            if (!layer.pixels) {
                throw std::invalid_argument(str(boost::format("Layer %d of texture '%s' is empty") % i % texture.name()));
            }
            if (compressed) {
                decompressLayer(texture.width(), texture.height(), layer, srcFormat, dstFormat);
                texture.setPixelFormat(dstFormat);
            }
            for (int j = 0; j < rotations[i]; ++j) {
                rotateLayer90(texture.width(), texture.height(), layer, getBitsPerPixel(dstFormat));
            }
        }
    } else {
        throw std::invalid_argument(str(boost::format("Texture '%s' has %d layers, %d expected") % texture.name() % numLayers % kNumCubeFaces));
    }
}

Texture::Properties getTextureProperties(TextureUsage usage) {
    Texture::Properties properties;

    if (usage == TextureUsage::ColorBuffer) {
        properties.minFilter = Texture::Filtering::Linear;
        properties.wrap = Texture::Wrapping::ClampToEdge;

    } else if (usage == TextureUsage::DepthBuffer) {
        properties.minFilter = Texture::Filtering::Nearest;
        properties.magFilter = Texture::Filtering::Nearest;
        properties.wrap = Texture::Wrapping::ClampToBorder;
        properties.borderColor = glm::vec4(1.0f);

    } else if (usage == TextureUsage::EnvironmentMap) {
        properties.wrap = Texture::Wrapping::ClampToEdge;

    } else if (usage == TextureUsage::BumpMap) {
        properties.minFilter = Texture::Filtering::Linear;

    } else if (usage == TextureUsage::GUI || usage == TextureUsage::Movie) {
        properties.minFilter = Texture::Filtering::Linear;
        properties.wrap = Texture::Wrapping::ClampToEdge;

    } else if (usage == TextureUsage::Font) {
        properties.minFilter = Texture::Filtering::Linear;
        properties.wrap = Texture::Wrapping::ClampToBorder;
    }

    return properties;
}

} // namespace graphics

} // namespace reone
