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
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/noncopyable.hpp>

#include "glm/glm.hpp"

#include "types.h"

namespace reone {

namespace render {

enum class ShaderProgram {
    None,
    SimpleColor,
    SimpleDepth,
    SimpleGUI,
    SimpleIrradiance,
    SimplePrefilter,
    SimpleBRDF,
    SimpleBlur,
    SimplePresentWorld,
    ModelColor,
    ModelBlinnPhong,
    ModelPBR,
    ParticleParticle,
    GrassGrass,
    TextText,
    BillboardGUI
};

struct UniformFeatureFlags {
    static constexpr int diffuse = 1;
    static constexpr int lightmap = 2;
    static constexpr int envmap = 4;
    static constexpr int pbrIbl = 8;
    static constexpr int bumpmaps = 0x10;
    static constexpr int skeletal = 0x20;
    static constexpr int lighting = 0x40;
    static constexpr int selfIllum = 0x80;
    static constexpr int discard = 0x100;
    static constexpr int shadows = 0x200;
    static constexpr int particles = 0x400;
    static constexpr int water = 0x800;
    static constexpr int hdr = 0x1000;
    static constexpr int customMat = 0x2000;
    static constexpr int blur = 0x4000;
    static constexpr int text = 0x8000;
    static constexpr int grass = 0x10000;
    static constexpr int fog = 0x20000;
    static constexpr int danglymesh = 0x40000;
};

struct ShaderGeneral {
    glm::mat4 projection { 1.0f };
    glm::mat4 view { 1.0f };
    glm::mat4 model { 1.0f };
    glm::vec4 cameraPosition { 0.0f };
    glm::vec4 color { 1.0f };
    glm::vec4 ambientColor { 1.0f };
    glm::vec4 selfIllumColor { 1.0f };
    glm::vec4 discardColor { 0.0f };
    glm::vec4 fogColor { 0.0f };
    glm::vec2 uvOffset { 0.0f };
    float alpha { 1.0f };
    float waterAlpha { 1.0f };
    float roughness { 0.0f };
    float exposure { 1.0f };
    float fogNear { 0.0f };
    float fogFar { 0.0f };
};

struct ShaderMaterial {
    glm::vec4 ambient { 1.0f };
    glm::vec4 diffuse { 0.0f };
    float specular { 0.0f };
    float shininess { 0.0f };
    float metallic { 0.0f };
    float roughness { 1.0f };
};

struct ShaderShadows {
    glm::mat4 lightSpaceMatrices[kNumCubeFaces];
    glm::vec4 lightPosition { 0.0f }; /**< W = 0 if light is directional */
    int lightPresent { false };
    float strength { 1.0f };
    char padding[8];
};

struct ShaderBumpmaps {
    int grayscale { 0 };
    float scaling { 0.0f };
    glm::vec2 gridSize { 1.0f };
    int frame { 0 };
    int swizzled { 0 };
    char padding[8];
};

struct ShaderBlur {
    glm::vec2 resolution { 0.0f };
    glm::vec2 direction { 0.0f };
};

struct CombinedUniforms {
    int featureMask { 0 }; /**< any combination of UniformFeaturesFlags */
    char padding[12];

    ShaderGeneral general;
    ShaderMaterial material;
    ShaderShadows shadows;
    ShaderBumpmaps bumpmaps;
    ShaderBlur blur;
};

struct ShaderLight {
    glm::vec4 position { 0.0f }; /**< W = 0 if light is directional */
    glm::vec4 color { 1.0f };
    float multiplier { 1.0f };
    float radius { 1.0f };
    char padding[8];
};

struct LightingUniforms {
    int lightCount { 0 };
    char padding[12];
    ShaderLight lights[kMaxLights];
};

struct SkeletalUniforms {
    glm::mat4 bones[kMaxBones];
};

struct ShaderParticle {
    glm::mat4 transform { 1.0f };
    glm::vec4 color { 1.0f };
    glm::vec2 size { 0.0f };
    float alpha { 1.0f };
    int frame { 0 };
};

struct ParticlesUniforms {
    glm::vec2 gridSize { 0.0f };
    int render { 0 };
    char padding[4];
    ShaderParticle particles[kMaxParticles];
};

struct ShaderGrassCluster {
    glm::vec4 positionVariant { 0.0f }; /**< fourth component is a variant (0-3) */
    glm::vec2 lightmapUV { 0.0f };
    char padding[8];
};

struct GrassUniforms {
    glm::vec2 quadSize { 0.0f };
    char padding[8];
    ShaderGrassCluster clusters[kMaxGrassClusters];
};

struct ShaderCharacter {
    glm::vec4 posScale { 0.0f };
    glm::vec4 uv { 0.0f };
};

struct TextUniforms {
    ShaderCharacter chars[kMaxCharacters];
};

struct DanglymeshUniforms {
    glm::vec4 stride { 0.0f };
    float displacement { 0.0f };
    char padding[12];
    glm::vec4 constraints[kMaxDanglymeshConstraints];
};

struct ShaderUniforms {
    CombinedUniforms combined;

    std::shared_ptr<TextUniforms> text;
    std::shared_ptr<LightingUniforms> lighting;
    std::shared_ptr<SkeletalUniforms> skeletal;
    std::shared_ptr<ParticlesUniforms> particles;
    std::shared_ptr<GrassUniforms> grass;
    std::shared_ptr<DanglymeshUniforms> danglymesh;
};

class Shaders : boost::noncopyable {
public:
    static Shaders &instance();

    void init();
    void deinit();

    void activate(ShaderProgram program, const ShaderUniforms &uniforms);
    void deactivate();

    const ShaderUniforms &defaultUniforms() const { return _defaultUniforms; }

private:
    enum class ShaderName {
        VertexSimple,
        VertexModel,
        VertexParticle,
        VertexGrass,
        VertexText,
        VertexBillboard,
        GeometryDepth,
        FragmentColor,
        FragmentDepth,
        FragmentGUI,
        FragmentText,
        FragmentBlinnPhong,
        FragmentPBR,
        FragmentParticle,
        FragmentGrass,
        FragmentIrradiance,
        FragmentPrefilter,
        FragmentBRDF,
        FragmentBlur,
        FragmentPresentWorld
    };

    bool _inited { false };
    std::unordered_map<ShaderName, uint32_t> _shaders;
    std::unordered_map<ShaderProgram, uint32_t> _programs;
    ShaderProgram _activeProgram { ShaderProgram::None };
    uint32_t _activeOrdinal { 0 };
    ShaderUniforms _defaultUniforms;

    // UBO

    uint32_t _uboCombined { 0 };
    uint32_t _uboText { 0 };
    uint32_t _uboLighting { 0 };
    uint32_t _uboSkeletal { 0 };
    uint32_t _uboParticles { 0 };
    uint32_t _uboGrass { 0 };
    uint32_t _uboDanglymesh { 0 };

    // END UBO

    ~Shaders();

    void initShader(ShaderName name, unsigned int type, std::vector<const char *> sources);
    void initProgram(ShaderProgram program, std::vector<ShaderName> shaders);
    void initUBOs();
    void initTextureUniforms();

    template <class T>
    void initUBO(const std::string &block, int bindingPoint, uint32_t ubo, const T &defaults, size_t size = sizeof(T));

    void setUniforms(const ShaderUniforms &locals);
    void setUniform(const std::string &name, int value);
    void setUniform(const std::string &name, const std::function<void(int)> &setter);
    void setUniform(const std::string &name, float value);
    void setUniform(const std::string &name, const glm::vec2 &v);
    void setUniform(const std::string &name, const glm::vec3 &v);
    void setUniform(const std::string &name, const glm::mat4 &m);
    void setUniform(const std::string &name, const std::vector<glm::mat4> &arr);

    unsigned int getOrdinal(ShaderProgram program) const;
};

} // namespace render

} // namespace reone
