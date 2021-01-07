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

class Texture {
public:
    struct MipMap {
        int width { 0 };
        int height { 0 };
        ByteArray data;
    };

    struct Layer {
        std::vector<MipMap> mipMaps;
    };

    Texture(std::string name, TextureType type, int w, int h);
    ~Texture();

    /**
     * Generates and configures an OpenGL texture.
     */
    void init();

    /**
     * Deletes the OpenGL texture.
     */
    void deinit();

    void clearPixels(PixelFormat format);

    void bind();
    void unbind();

    bool isAdditive() const;

    const std::string &name() const;
    uint32_t textureId() const;
    int width() const;
    int height() const;
    PixelFormat pixelFormat() const;
    const TextureFeatures &features() const;

    void setPixels(std::vector<Layer> layers, PixelFormat format);
    void setFeatures(TextureFeatures features);

private:
    std::string _name;
    TextureType _type;
    int _width;
    int _height;

    bool _inited { false };
    uint32_t _textureId { 0 };

    std::vector<Layer> _layers;
    PixelFormat _pixelFormat { PixelFormat::BGR };
    TextureFeatures _features;

    Texture(const Texture &) = delete;
    Texture &operator=(const Texture &) = delete;

    void configure2D();
    void configureCubeMap();

    void refresh();
    void refresh2D();
    void refreshCubeMap();

    void fillTarget(uint32_t target, int level, int width, int height, const void *pixels = nullptr, int size = 0);

    /**
    * @return true if texture of this type is a cube map, false otherwise
    */
    bool isCubeMap() const;

    /**
     * @return true if texture of this type should have mip maps, false otherwise
     */
    bool hasMipMaps() const;

    int getInternalPixelFormat() const;
    uint32_t getPixelFormat() const;
};

} // namespace render

} // namespace reone
