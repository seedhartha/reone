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

#include "reone/graphics/dxtutil.h"

namespace reone {

namespace graphics {

static uint32_t packRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return ((r << 24) | (g << 16) | (b << 8) | a);
}

static void decompressDXT15Block(uint32_t x,
                                 uint32_t y,
                                 uint32_t width,
                                 const uint8_t *blockStorage,
                                 bool hasAlpha,
                                 uint32_t *outImage) {
    uint8_t alphas[2];
    uint64_t alphaCodes;
    uint16_t colors[2];
    uint32_t colorCodes;
    if (hasAlpha) {
        alphas[0] = *(blockStorage + 0);
        alphas[1] = *(blockStorage + 1);
        alphaCodes = *reinterpret_cast<const uint64_t *>(blockStorage + 2);
        colors[0] = *reinterpret_cast<const uint16_t *>(blockStorage + 8);
        colors[1] = *reinterpret_cast<const uint16_t *>(blockStorage + 10);
        colorCodes = *reinterpret_cast<const uint32_t *>(blockStorage + 12);
    } else {
        colors[0] = *reinterpret_cast<const uint16_t *>(blockStorage + 0);
        colors[1] = *reinterpret_cast<const uint16_t *>(blockStorage + 2);
        colorCodes = *reinterpret_cast<const uint32_t *>(blockStorage + 4);
    }

    uint8_t r[2], g[2], b[2];
    for (int i = 0; i < 2; ++i) {
        uint32_t temp = (colors[i] >> 11) * 255 + 16;
        r[i] = static_cast<uint8_t>((temp / 32 + temp) / 32);
        temp = ((colors[i] & 0x07e0) >> 5) * 255 + 32;
        g[i] = static_cast<uint8_t>((temp / 64 + temp) / 64);
        temp = (colors[i] & 0x001f) * 255 + 16;
        b[i] = static_cast<uint8_t>((temp / 32 + temp) / 32);
    }

    for (int j = 0; j < 4; j++) {
        for (int i = 0; i < 4; i++) {
            uint8_t alpha;
            if (hasAlpha) {
                int alphaCodeIdx = 3 * (4 * j + i);
                int alphaCode = (alphaCodes >> alphaCodeIdx) & 0x07;
                if (alphaCode == 0) {
                    alpha = alphas[0];
                } else if (alphaCode == 1) {
                    alpha = alphas[1];
                } else {
                    if (alphas[0] > alphas[1]) {
                        alpha = ((8 - alphaCode) * alphas[0] + (alphaCode - 1) * alphas[1]) / 7;
                    } else {
                        if (alphaCode == 6)
                            alpha = 0;
                        else if (alphaCode == 7)
                            alpha = 255;
                        else
                            alpha = ((6 - alphaCode) * alphas[0] + (alphaCode - 1) * alphas[1]) / 5;
                    }
                }
            } else {
                alpha = 255;
            }

            int colorCodeIdx = 2 * (4 * j + i);
            uint8_t colorCode = (colorCodes >> colorCodeIdx) & 0x03;
            uint32_t color = 0;
            if (hasAlpha || colors[0] > colors[1]) {
                switch (colorCode) {
                case 0:
                    color = packRGBA(r[0], g[0], b[0], alpha);
                    break;
                case 1:
                    color = packRGBA(r[1], g[1], b[1], alpha);
                    break;
                case 2:
                    color = packRGBA((2 * r[0] + r[1]) / 3, (2 * g[0] + g[1]) / 3, (2 * b[0] + b[1]) / 3, alpha);
                    break;
                case 3:
                    color = packRGBA((r[0] + 2 * r[1]) / 3, (g[0] + 2 * g[1]) / 3, (b[0] + 2 * b[1]) / 3, alpha);
                    break;
                }
            } else {
                switch (colorCode) {
                case 0:
                    color = packRGBA(r[0], g[0], b[0], alpha);
                    break;
                case 1:
                    color = packRGBA(r[1], g[1], b[1], alpha);
                    break;
                case 2:
                    color = packRGBA((r[0] + r[1]) / 2, (g[0] + g[1]) / 2, (b[0] + b[1]) / 2, alpha);
                    break;
                case 3:
                    color = packRGBA(0, 0, 0, alpha);
                    break;
                }
            }

            if (x + i < width) {
                outImage[(y + j) * width + (x + i)] = color;
            }
        }
    }
}

void decompressDXT1(uint32_t width,
                    uint32_t height,
                    const uint8_t *blockStorage,
                    uint32_t *outImage) {
    uint32_t blockCountX = (width + 3) / 4;
    uint32_t blockCountY = (height + 3) / 4;
    for (uint32_t j = 0; j < blockCountY; j++) {
        for (uint32_t i = 0; i < blockCountX; i++) {
            decompressDXT15Block(i * 4, j * 4, width, blockStorage + i * 8, false, outImage);
        }
        blockStorage += blockCountX * 8;
    }
}

void decompressDXT5(uint32_t width,
                    uint32_t height,
                    const uint8_t *blockStorage,
                    uint32_t *outImage) {
    uint32_t blockCountX = (width + 3) / 4;
    uint32_t blockCountY = (height + 3) / 4;
    for (uint32_t j = 0; j < blockCountY; j++) {
        for (uint32_t i = 0; i < blockCountX; i++) {
            decompressDXT15Block(i * 4, j * 4, width, blockStorage + i * 16, true, outImage);
        }
        blockStorage += blockCountX * 16;
    }
}

} // namespace graphics

} // namespace reone
