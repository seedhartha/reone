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
#include "reone/system/checkutil.h"

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

static int getBytesPerPixel(PixelFormat format) {
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

void convertGridTextureToArray(Texture &texture, int numX, int numY) {
    checkEqual("layers size", static_cast<int>(texture.layers().size()), 1);
    if (isCompressed(texture.pixelFormat())) {
        PixelFormat newFormat;
        decompressLayer(
            texture.width(),
            texture.height(),
            texture.layers().front(),
            texture.pixelFormat(),
            newFormat);
        texture.setPixelFormat(newFormat);
    }
    auto gridPixels = *texture.layers().front().pixels;
    glm::ivec2 frameSize {texture.width() / numX, texture.height() / numY};
    std::vector<Texture::Layer> frameLayers;
    int bytesPerPixel = getBytesPerPixel(texture.pixelFormat());
    size_t framePixelsSize = frameSize.x * frameSize.y * bytesPerPixel;
    for (int i = 0; i < numX * numY; ++i) {
        auto framePixels = std::make_shared<ByteBuffer>();
        framePixels->resize(framePixelsSize);
        for (int x = 0; x < frameSize.x; ++x) {
            for (int y = 0; y < frameSize.y; ++y) {
                int srcRowsToSkip = (i / numX) * frameSize.y + y;
                int srcColsToSkip = (i % numX) * frameSize.x + x;
                int srcPixelIdx = srcRowsToSkip * texture.width() + srcColsToSkip;
                auto srcPixel = &gridPixels[srcPixelIdx * bytesPerPixel];
                int dstPixelIdx = (y * frameSize.x + x);
                auto dstPixel = &(*framePixels)[dstPixelIdx * bytesPerPixel];
                std::memcpy(dstPixel, srcPixel, bytesPerPixel);
            }
        }
        frameLayers.push_back(Texture::Layer {std::move(framePixels)});
    }
    texture.setType(TextureType::TwoDimArray);
    texture.setPixels(
        frameSize.x, frameSize.y,
        texture.pixelFormat(),
        std::move(frameLayers));
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
