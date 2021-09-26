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

#include "../../common/types.h"

#include "../types.h"

namespace reone {

namespace graphics {

/**
 * Abstraction over the OpenGL texture object.
 */
class Texture : boost::noncopyable {
public:
    enum class Filtering {
        Nearest,
        Linear,
        NearestMipmapNearest,
        LinearMipmapNearest,
        NearestMipmapLinear,
        LinearMipmapLinear
    };

    enum class Wrapping {
        Repeat,
        ClampToEdge,
        ClampToBorder
    };

    enum class Blending {
        None,
        Additive
    };

    enum class ProcedureType {
        Invalid,
        Cycle
    };

    struct Properties {
        Filtering minFilter {Filtering::LinearMipmapLinear};
        Filtering maxFilter {Filtering::Linear};
        Wrapping wrap {Wrapping::Repeat};
        glm::vec3 borderColor {1.0f};
        bool cubemap {false};  /**< is this a cube map texture? */
        bool headless {false}; /**< must an OpenGL texture be created? */
    };

    /**
     * Container for properties found in TXI files.
     */
    struct Features {
        std::string envmapTexture;
        std::string bumpyShinyTexture;
        std::string bumpmapTexture;
        float bumpMapScaling {0.0f};
        Blending blending {Blending::None};
        int numChars {0};
        float fontHeight {0.0f};
        std::vector<glm::vec3> upperLeftCoords;
        std::vector<glm::vec3> lowerRightCoords;
        float waterAlpha {-1.0f};
        ProcedureType procedureType {ProcedureType::Invalid};
        int numX {0};
        int numY {0};
        int fps {0};
    };

    struct MipMap {
        int width {0};
        int height {0};
        std::shared_ptr<ByteArray> pixels;
    };

    struct Layer {
        std::vector<MipMap> mipMaps;
    };

    Texture(std::string name, Properties properties);

    ~Texture();

    void init();
    void deinit();

    void bind() const;
    void unbind() const;

    void flushGPUToCPU();

    /**
     * Clears this texture pixels. Texture must be bound, unless it is headless.
     */
    void clearPixels(int w, int h, PixelFormat format);

    /**
     * @return true if this is a cube map texture, false otherwise
     */
    bool isCubeMap() const;

    bool isAdditive() const;
    bool isGrayscale() const;

    const std::string &name() const { return _name; }
    int width() const { return _width; }
    int height() const { return _height; }
    const std::vector<Layer> &layers() const { return _layers; }
    const Features &features() const { return _features; }
    uint32_t textureId() const { return _textureId; }
    PixelFormat pixelFormat() const { return _pixelFormat; }

    /**
     * Sets this texture pixels from a single image. Texture must be bound, unless it is headless.
     */
    void setPixels(int w, int h, PixelFormat format, std::shared_ptr<ByteArray> pixels);

    /**
     * Sets this texture pixels from multiple images. Texture must be bound, unless it is headless.
     */
    void setPixels(int w, int h, PixelFormat format, std::vector<Layer> layers);

    void setFeatures(Features features);

private:
    std::string _name;
    Properties _properties;

    bool _inited {false};
    uint32_t _textureId {0};

    int _width {0};
    int _height {0};
    PixelFormat _pixelFormat {PixelFormat::BGR};
    std::vector<Layer> _layers; /**< either one for 2D textures, or six for cube maps */
    Features _features;

    void configure2D();
    void configureCubeMap();

    void refresh();
    void refresh2D();
    void refreshCubeMap();

    void fillTarget(uint32_t target, int level, int width, int height, const void *pixels = nullptr, int size = 0);

    bool isMipmapFilter(Filtering filter) const;

    uint32_t getFilterGL(Filtering filter) const;
    uint32_t getPixelFormatGL() const;
    uint32_t getPixelTypeGL() const;
};

} // namespace graphics

} // namespace reone
