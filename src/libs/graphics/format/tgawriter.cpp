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

#include "reone/graphics/format/tgawriter.h"

#include "reone/graphics/dxtutil.h"
#include "reone/graphics/texture.h"
#include "reone/system/exception/validation.h"

namespace reone {

namespace graphics {

static constexpr int kHeaderSize = 18;

TgaWriter::TgaWriter(std::shared_ptr<Texture> texture) :
    _texture(std::move(texture)) {
}

void TgaWriter::save(IOutputStream &out, bool compress) {
    // Image ID, color-mapped images, RLE and image orientation are not supported

    int width = _texture->width();
    int totalHeight = static_cast<int>(_texture->layers().size()) * _texture->height();

    TGADataType dataType;
    int depth;
    std::vector<uint8_t> pixels(getTexturePixels(compress, dataType, depth));

    // Write Header

    uint8_t header[kHeaderSize] {0};
    header[0] = 0; // ID length
    header[1] = 0; // color map type
    header[2] = static_cast<uint8_t>(dataType);
    memset(header + 4, 0, 5);
    header[8] = 0;                    // X origin (lo)
    header[9] = 0;                    // X origin (hi)
    header[10] = 0;                   // Y origin (lo)
    header[11] = 0;                   // Y origin (hi)
    header[12] = width % 256;         // width (lo)
    header[13] = width / 256;         // width (hi)
    header[14] = totalHeight % 256;   // height (lo)
    header[15] = totalHeight / 256;   // height (hi)
    header[16] = depth;               // pixel size
    header[17] = depth == 32 ? 8 : 0; // image descriptor;
    out.write(reinterpret_cast<char *>(header), kHeaderSize);

    // Write Scanlines

    int scanlineSize = width * depth / 8;
    if (depth >= 24 && compress) {
        for (int wrote = 0; wrote < totalHeight; ++wrote) {
            int offset = wrote * scanlineSize;
            writeRLE(&pixels[offset], depth, out);
        }
    } else {
        out.write(reinterpret_cast<char *>(&pixels[0]), static_cast<size_t>(totalHeight) * scanlineSize);
    }
}

std::vector<uint8_t> TgaWriter::getTexturePixels(bool compress, TGADataType &dataType, int &depth) const {
    std::vector<uint8_t> result;

    switch (_texture->pixelFormat()) {
    case PixelFormat::R8:
        dataType = TGADataType::Grayscale;
        depth = 8;
        break;
    case PixelFormat::RGB8:
    case PixelFormat::BGR8:
    case PixelFormat::DXT1:
        dataType = compress ? TGADataType::RGBA_RLE : TGADataType::RGBA;
        depth = 24;
        break;
    case PixelFormat::RGBA8:
    case PixelFormat::BGRA8:
    case PixelFormat::DXT5:
        dataType = compress ? TGADataType::RGBA_RLE : TGADataType::RGBA;
        depth = 32;
        break;
    default:
        throw ValidationException("Unsupported texture pixel format: " + std::to_string(static_cast<int>(_texture->pixelFormat())));
    }

    int numLayers = static_cast<int>(_texture->layers().size());
    int numPixels = _texture->width() * _texture->height();
    int numPixelsTotal = numLayers * numPixels;
    result.resize(static_cast<size_t>(numPixelsTotal) * depth / 8);
    uint8_t *pixels = &result[0];

    for (int i = 0; i < numLayers; ++i) {
        auto &layer = _texture->layers()[i];
        auto layerPixelsPtr = reinterpret_cast<const uint8_t *>(layer.pixels->data());

        switch (_texture->pixelFormat()) {
        case PixelFormat::R8:
            memcpy(pixels, layerPixelsPtr, numPixels);
            break;
        case PixelFormat::RGB8:
            for (int j = 0; j < numPixels; ++j) {
                *(pixels++) = layerPixelsPtr[2];
                *(pixels++) = layerPixelsPtr[1];
                *(pixels++) = layerPixelsPtr[0];
                layerPixelsPtr += 3;
            }
            break;
        case PixelFormat::RGBA8:
            for (int j = 0; j < numPixels; ++j) {
                *(pixels++) = layerPixelsPtr[2];
                *(pixels++) = layerPixelsPtr[1];
                *(pixels++) = layerPixelsPtr[0];
                *(pixels++) = layerPixelsPtr[3];
                layerPixelsPtr += 4;
            }
            break;
        case PixelFormat::BGR8:
            memcpy(pixels, layerPixelsPtr, 3ll * numPixels);
            break;
        case PixelFormat::BGRA8:
            memcpy(pixels, layerPixelsPtr, 4ll * numPixels);
            break;
        case PixelFormat::DXT1: {
            std::vector<uint32_t> decompPixels(numPixels);
            decompressDXT1(_texture->width(), _texture->height(), layerPixelsPtr, &decompPixels[0]);
            uint32_t *decompPtr = &decompPixels[0];
            for (int j = 0; j < numPixels; ++j) {
                uint32_t rgb = *(decompPtr++);
                *(pixels++) = (rgb >> 8) & 0xff;
                *(pixels++) = (rgb >> 16) & 0xff;
                *(pixels++) = (rgb >> 24) & 0xff;
            }
            break;
        }
        case PixelFormat::DXT5: {
            std::vector<uint32_t> decompPixels(numPixels);
            decompressDXT5(_texture->width(), _texture->height(), layerPixelsPtr, &decompPixels[0]);
            uint32_t *decompPtr = &decompPixels[0];
            for (int j = 0; j < numPixels; ++j) {
                uint32_t rgba = *(decompPtr++);
                *(pixels++) = (rgba >> 8) & 0xff;
                *(pixels++) = (rgba >> 16) & 0xff;
                *(pixels++) = (rgba >> 24) & 0xff;
                *(pixels++) = rgba & 0xff;
            }
            break;
        }
        default:
            break;
        }
    }

    return result;
}

void TgaWriter::writeRLE(uint8_t *pixels, int depth, IOutputStream &out) {
    uint8_t *from = pixels;
    uint8_t repeat = 0, direct = 0;
    int bytes = depth / 8;

    for (int x = 1; x < _texture->width(); ++x) {
        if (memcpy(pixels, pixels + bytes, bytes)) {
            if (repeat) {
                out.writeByte(128 + repeat);
                out.write(reinterpret_cast<char *>(from), bytes);
                from = pixels + bytes;
                repeat = 0;
                direct = 0;
            } else {
                ++direct;
            }
        } else {
            if (direct) {
                out.writeByte(direct - 1);
                out.write(reinterpret_cast<char *>(from), bytes * static_cast<size_t>(direct));
                from = pixels;
                direct = 0;
                repeat = 1;
            } else {
                ++repeat;
            }
        }
        if (repeat == 128) {
            out.writeByte(static_cast<char>(255));
            out.write(reinterpret_cast<char *>(from), bytes);
            from = pixels + bytes;
            direct = 0;
            repeat = 0;
        } else if (direct == 128) {
            out.writeByte(127);
            out.write(reinterpret_cast<char *>(from), bytes * static_cast<size_t>(direct));
            from = pixels + bytes;
            direct = 0;
            repeat = 0;
        }
        pixels += bytes;
    }

    if (repeat > 0) {
        out.writeByte(128 + repeat);
        out.write(reinterpret_cast<char *>(from), bytes);
    } else {
        out.writeByte(direct);
        out.write(reinterpret_cast<char *>(from), bytes * static_cast<size_t>(direct + 1));
    }
}

} // namespace graphics

} // namespace reone
