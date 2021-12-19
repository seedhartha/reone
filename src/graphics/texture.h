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

#include "../common/types.h"

#include "attachment.h"
#include "types.h"

namespace reone {

namespace graphics {

class Texture : public IAttachment, boost::noncopyable {
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
        bool cubemap {false};
        int numSamples {1};
    };

    /**
     * Container for properties found in TXI files.
     */
    struct Features {
        std::string envmapTexture;
        std::string bumpyShinyTexture;
        std::string bumpmapTexture;
        float bumpMapScaling {1.0f};
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

    Texture(std::string name, Properties properties) :
        _name(std::move(name)),
        _properties(std::move(properties)) {
    }

    ~Texture() { deinit(); }

    void init();
    void deinit();

    void bind();
    void unbind();

    void configure();
    void refresh();

    void flushGPUToCPU();

    glm::vec4 sample(float s, float t) const;
    glm::vec4 sample(int x, int y) const;

    bool isCubeMap() const { return _properties.cubemap; }
    bool isMultisample() const { return _properties.numSamples > 1; }
    bool isAdditive() const { return _features.blending == Blending::Additive; }
    bool isGrayscale() const { return _pixelFormat == PixelFormat::Grayscale; }

    bool isTexture() const override { return true; }
    bool isRenderbuffer() const override { return false; }

    const std::string &name() const { return _name; }
    int width() const { return _width; }
    int height() const { return _height; }
    const std::vector<Layer> &layers() const { return _layers; }
    const Features &features() const { return _features; }
    uint32_t nameGL() const { return _nameGL; }
    PixelFormat pixelFormat() const { return _pixelFormat; }

    void setFeatures(Features features) { _features = std::move(features); }

    // Pixels

    void clear(int w, int h, PixelFormat format);

    void setPixels(int w, int h, PixelFormat format, std::shared_ptr<ByteArray> pixels);
    void setPixels(int w, int h, PixelFormat format, std::vector<Layer> layers);

    // END Pixels

private:
    std::string _name;
    Properties _properties;

    bool _inited {false};
    uint32_t _nameGL {0};

    int _width {0};
    int _height {0};
    PixelFormat _pixelFormat {PixelFormat::BGR};
    std::vector<Layer> _layers; /**< either one for 2D textures, or six for cube maps */
    Features _features;

    void configure2D();
    void configureCubeMap();

    void refresh2D();
    void refreshCubeMap();

    void fillTarget(uint32_t target, int level, int width, int height, const void *pixels = nullptr, int size = 0);
};

} // namespace graphics

} // namespace reone
