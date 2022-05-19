/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "dxtutil.h"

namespace reone {

namespace graphics {

static uint32_t packRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return ((r << 24) | (g << 16) | (b << 8) | a);
}

static void decompressDXT1Block(uint32_t x, uint32_t y, uint32_t width, const uint8_t *blockStorage, uint32_t *image) {
    uint16_t color0 = *reinterpret_cast<const uint16_t *>(blockStorage + 0);
    uint16_t color1 = *reinterpret_cast<const uint16_t *>(blockStorage + 2);
    uint32_t colorCodes = *reinterpret_cast<const uint32_t *>(blockStorage + 4);

    uint32_t temp;

    temp = (color0 >> 11) * 255 + 16;
    uint8_t r0 = static_cast<uint8_t>((temp / 32 + temp) / 32);
    temp = ((color0 & 0x07e0) >> 5) * 255 + 32;
    uint8_t g0 = static_cast<uint8_t>((temp / 64 + temp) / 64);
    temp = (color0 & 0x001f) * 255 + 16;
    uint8_t b0 = static_cast<uint8_t>((temp / 32 + temp) / 32);

    temp = (color1 >> 11) * 255 + 16;
    uint8_t r1 = static_cast<uint8_t>((temp / 32 + temp) / 32);
    temp = ((color1 & 0x07e0) >> 5) * 255 + 32;
    uint8_t g1 = static_cast<uint8_t>((temp / 64 + temp) / 64);
    temp = (color1 & 0x001f) * 255 + 16;
    uint8_t b1 = static_cast<uint8_t>((temp / 32 + temp) / 32);

    for (int j = 0; j < 4; j++) {
        for (int i = 0; i < 4; i++) {
            int colorCodeIdx = 2 * (4 * j + i);
            uint8_t colorCode = (colorCodes >> colorCodeIdx) & 0x03;
            uint32_t finalColor = 0;

            if (color0 > color1) {
                switch (colorCode) {
                case 0:
                    finalColor = packRGBA(r0, g0, b0, 255);
                    break;
                case 1:
                    finalColor = packRGBA(r1, g1, b1, 255);
                    break;
                case 2:
                    finalColor = packRGBA((2 * r0 + r1) / 3, (2 * g0 + g1) / 3, (2 * b0 + b1) / 3, 255);
                    break;
                case 3:
                    finalColor = packRGBA((r0 + 2 * r1) / 3, (g0 + 2 * g1) / 3, (b0 + 2 * b1) / 3, 255);
                    break;
                }
            } else {
                switch (colorCode) {
                case 0:
                    finalColor = packRGBA(r0, g0, b0, 255);
                    break;
                case 1:
                    finalColor = packRGBA(r1, g1, b1, 255);
                    break;
                case 2:
                    finalColor = packRGBA((r0 + r1) / 2, (g0 + g1) / 2, (b0 + b1) / 2, 255);
                    break;
                case 3:
                    finalColor = packRGBA(0, 0, 0, 255);
                    break;
                }
            }

            if (x + i < width) {
                image[(y + j) * width + (x + i)] = finalColor;
            }
        }
    }
}

void decompressDXT1(uint32_t width, uint32_t height, const uint8_t *blockStorage, uint32_t *image) {
    uint32_t blockCountX = (width + 3) / 4;
    uint32_t blockCountY = (height + 3) / 4;
    uint32_t blockWidth = (width < 4) ? width : 4;
    uint32_t blockHeight = (height < 4) ? height : 4;

    for (uint32_t j = 0; j < blockCountY; j++) {
        for (uint32_t i = 0; i < blockCountX; i++) {
            decompressDXT1Block(i * 4, j * 4, width, blockStorage + i * 8, image);
        }
        blockStorage += blockCountX * 8;
    }
}

static void decompressDXT5Block(uint32_t x, uint32_t y, uint32_t width, const uint8_t *blockStorage, uint32_t *image) {
    uint8_t alpha0 = *(blockStorage + 0);
    uint8_t alpha1 = *(blockStorage + 1);
    uint64_t alphaCodes = *reinterpret_cast<const uint64_t *>(blockStorage + 2);

    uint16_t color0 = *reinterpret_cast<const uint16_t *>(blockStorage + 8);
    uint16_t color1 = *reinterpret_cast<const uint16_t *>(blockStorage + 10);
    uint32_t colorCodes = *reinterpret_cast<const uint32_t *>(blockStorage + 12);

    uint32_t temp;

    temp = (color0 >> 11) * 255 + 16;
    uint8_t r0 = static_cast<uint8_t>((temp / 32 + temp) / 32);
    temp = ((color0 & 0x07e0) >> 5) * 255 + 32;
    uint8_t g0 = static_cast<uint8_t>((temp / 64 + temp) / 64);
    temp = (color0 & 0x001f) * 255 + 16;
    uint8_t b0 = static_cast<uint8_t>((temp / 32 + temp) / 32);

    temp = (color1 >> 11) * 255 + 16;
    uint8_t r1 = static_cast<uint8_t>((temp / 32 + temp) / 32);
    temp = ((color1 & 0x07e0) >> 5) * 255 + 32;
    uint8_t g1 = static_cast<uint8_t>((temp / 64 + temp) / 64);
    temp = (color1 & 0x001f) * 255 + 16;
    uint8_t b1 = static_cast<uint8_t>((temp / 32 + temp) / 32);

    for (int j = 0; j < 4; j++) {
        for (int i = 0; i < 4; i++) {
            int alphaCodeIdx = 3 * (4 * j + i);
            int alphaCode = (alphaCodes >> alphaCodeIdx) & 0x07;
            uint8_t finalAlpha;
            if (alphaCode == 0) {
                finalAlpha = alpha0;
            } else if (alphaCode == 1) {
                finalAlpha = alpha1;
            } else {
                if (alpha0 > alpha1) {
                    finalAlpha = ((8 - alphaCode) * alpha0 + (alphaCode - 1) * alpha1) / 7;
                } else {
                    if (alphaCode == 6)
                        finalAlpha = 0;
                    else if (alphaCode == 7)
                        finalAlpha = 255;
                    else
                        finalAlpha = ((6 - alphaCode) * alpha0 + (alphaCode - 1) * alpha1) / 5;
                }
            }

            int colorCodeIdx = 2 * (4 * j + i);
            uint8_t colorCode = (colorCodes >> colorCodeIdx) & 0x03;
            uint32_t finalColor;
            switch (colorCode) {
            case 0:
                finalColor = packRGBA(r0, g0, b0, finalAlpha);
                break;
            case 1:
                finalColor = packRGBA(r1, g1, b1, finalAlpha);
                break;
            case 2:
                finalColor = packRGBA((2 * r0 + r1) / 3, (2 * g0 + g1) / 3, (2 * b0 + b1) / 3, finalAlpha);
                break;
            case 3:
                finalColor = packRGBA((r0 + 2 * r1) / 3, (g0 + 2 * g1) / 3, (b0 + 2 * b1) / 3, finalAlpha);
                break;
            }

            if (x + i < width) {
                image[(y + j) * width + (x + i)] = finalColor;
            }
        }
    }
}

void decompressDXT5(uint32_t width, uint32_t height, const uint8_t *blockStorage, uint32_t *image) {
    uint32_t blockCountX = (width + 3) / 4;
    uint32_t blockCountY = (height + 3) / 4;
    uint32_t blockWidth = (width < 4) ? width : 4;
    uint32_t blockHeight = (height < 4) ? height : 4;

    for (uint32_t j = 0; j < blockCountY; j++) {
        for (uint32_t i = 0; i < blockCountX; i++) {
            decompressDXT5Block(i * 4, j * 4, width, blockStorage + i * 16, image);
        }
        blockStorage += blockCountX * 16;
    }
}

} // namespace graphics

} // namespace reone
