/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "../core/types.h"

#include "types.h"

namespace reone {

namespace resources {

class TgaFile;
class TpcFile;

}

namespace render {

enum class PixelFormat {
    Grayscale,
    RGB,
    RGBA,
    BGR,
    BGRA,
    DXT1,
    DXT5
};

class Texture {
public:
    Texture(const std::string &name, TextureType type);
    ~Texture();

    void initGL();
    void deinitGL();
    void bind();
    void unbind();

    bool isAdditive() const;

    const std::string &name() const;
    int width() const;
    int height() const;
    PixelFormat pixelFormat() const;
    const TextureFeatures &features() const;

private:
    bool _glInited { false };
    std::string _name;
    TextureType _type { TextureType::Diffuse };
    int _width { 0 };
    int _height { 0 };
    PixelFormat _pixelFormat { PixelFormat::BGR };
    std::vector<ByteArray> _images;
    bool _cubeMap { false };
    TextureFeatures _features;
    unsigned int _textureId { 0 };

    Texture(const Texture &) = delete;

    Texture &operator=(const Texture &) = delete;

    void fillTextureTarget(unsigned int target, const ByteArray &data);
    int glInternalPixelFormat() const;
    unsigned int glPixelFormat() const;

    friend class resources::TgaFile;
    friend class resources::TpcFile;
};

} // namespace render

} // namespace reone
