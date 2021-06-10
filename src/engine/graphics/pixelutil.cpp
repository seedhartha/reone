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

#include "pixelutil.h"

using namespace std;

namespace reone {

namespace graphics {

uint32_t getInternalPixelFormatGL(PixelFormat format) {
    switch (format) {
        case PixelFormat::Grayscale:
            return GL_RED;
        case PixelFormat::RGB:
        case PixelFormat::BGR:
            return GL_RGB8;
        case PixelFormat::RGBA:
        case PixelFormat::BGRA:
            return GL_RGBA8;
        case PixelFormat::DXT1:
            return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
        case PixelFormat::DXT5:
            return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        case PixelFormat::Depth:
            return GL_DEPTH_COMPONENT;
        default:
            throw logic_error("Unsupported pixel format: " + to_string(static_cast<int>(format)));
    }
}

} // namespace graphics

} // namespace reone
