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

#pragma once

#include "texture.h"

namespace reone {

namespace graphics {

void convertGridTextureToArray(Texture &texture, int numX, int numY);

Texture::Properties getTextureProperties(TextureUsage usage);

inline bool isCompressed(PixelFormat format) {
    return format == PixelFormat::DXT1 || format == PixelFormat::DXT5;
}

inline bool hasAlphaChannel(PixelFormat format) {
    switch (format) {
    case PixelFormat::RGBA8:
    case PixelFormat::RGBA16F:
    case PixelFormat::BGRA8:
    case PixelFormat::DXT5:
        return true;
    default:
        return false;
    }
}

} // namespace graphics

} // namespace reone
