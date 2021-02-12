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

#pragma once

#include "texture.h"

namespace reone {

namespace render {

Texture::Properties getTextureProperties(TextureUsage usage, bool headless = false);

/**
 * KotOR and TSL cube maps require specific transformations before use.
 * Allegedly, this is what the original engine does.
 *
 * @param layers texture layers to process
 * @param srcFormat source pixel format
 * @param destFormat resulting pixel format (output parameter)
 */
void prepareCubeMap(std::vector<Texture::Layer> &layers, Texture::PixelFormat srcFormat, Texture::PixelFormat &destFormat);

/**
 * Decompresses the mip map.
 *
 * @param mipMap mip map to decompress
 * @param srcFormat source pixel format - must be either DXT1 or DXT5
 * @param destFormat pixel format of a decompressed mip map (output parameter)
 */
void decompressMipMap(Texture::MipMap &mipMap, Texture::PixelFormat srcFormat, Texture::PixelFormat &destFormat);

/**
 * Rotates the mip map by 90 degrees.
 *
 * @param mipMap mip map to rotate - must be uncompressed
 * @param bpp number of bytes per pixel
 */
void rotateMipMap90(Texture::MipMap &mipMap, int bpp);

} // namespace render

} // namespace reone
