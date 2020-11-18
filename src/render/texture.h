/*
 * Copyright (c) 2020 The reone project contributors
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

#include <cstdint>

#include "../common/types.h"

#include "types.h"

namespace reone {

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

class CurFile;
class TgaFile;
class TpcFile;

class Texture {
public:
    Texture(const std::string &name, TextureType type);
    ~Texture();

    void initGL();
    void deinitGL();
    void bind(int unit);

    bool isAdditive() const;

    const std::string &name() const;
    int width() const;
    int height() const;
    PixelFormat pixelFormat() const;
    const TextureFeatures &features() const;

private:
    struct MipMap {
        int width { 0 };
        int height { 0 };
        ByteArray data;
    };

    struct Layer {
        std::vector<MipMap> mipMaps;
    };

    bool _glInited { false };
    std::string _name;
    TextureType _type { TextureType::Diffuse };
    PixelFormat _pixelFormat { PixelFormat::BGR };
    int _width { 0 };
    int _height { 0 };
    std::vector<Layer> _layers;
    TextureFeatures _features;
    uint32_t _textureId { 0 };

    Texture(const Texture &) = delete;

    Texture &operator=(const Texture &) = delete;

    inline bool isCubeMap() const;
    void fillTextureTarget(uint32_t target, int level, int width, int height, const ByteArray &data);
    int glInternalPixelFormat() const;
    uint32_t glPixelFormat() const;

    friend class CurFile;
    friend class TgaFile;
    friend class TpcFile;
};

} // namespace render

} // namespace reone
