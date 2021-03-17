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
    SimpleDebugCubeMap,
    ModelColor,
    ModelBlinnPhong,
    ModelPBR,
    ParticleParticle,
    TextText
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
};

struct GeneralUniforms {
    glm::mat4 projection { 1.0f };
    glm::mat4 view { 1.0f };
    glm::mat4 model { 1.0f };
    glm::vec4 cameraPosition { 0.0f };
    glm::vec4 color { 1.0f };
    glm::vec4 ambientColor { 1.0f };
    glm::vec4 selfIllumColor { 1.0f };
    glm::vec4 discardColor { 0.0f };
    glm::vec2 uvOffset { 0.0f };
    float alpha { 1.0f };
    float waterAlpha { 1.0f };
    int featureMask { 0 }; /**< any combination of UniformFeaturesFlags */
    float roughness { 0.0f };
    float exposure { 1.0f };
    char padding[4];
};

struct MaterialUniforms {
    glm::vec4 ambient { 1.0f };
    glm::vec4 diffuse { 0.0f };
    float specular { 0.0f };
    float shininess { 0.0f };
    float metallic { 0.0f };
    float roughness { 1.0f };
};

struct ShadowUniforms {
    glm::mat4 matrices[kNumCubeFaces];
    glm::vec4 lightPosition { 0.0f };
    int lightPresent { false };
    float strength { 1.0f };
    char padding[8];
};

struct BumpmapsUniforms {
    int grayscale { 0 };
    float scaling { 0.0f };
    glm::vec2 gridSize { 1.0f };
    int frame { 0 };
    int swizzled { 0 };
    char padding[8];
};

struct BlurUniforms {
    glm::vec2 resolution { 0.0f };
    glm::vec2 direction { 0.0f };
};

struct ShaderLight {
    glm::vec4 position { 0.0f };
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

struct ShaderCharacter {
    glm::mat4 transform { 0.0f };
    glm::vec4 uv { 0.0f };
};

struct TextUniforms {
    ShaderCharacter chars[kMaxCharacters];
};

struct ShaderUniforms {
    GeneralUniforms general;
    MaterialUniforms material;
    ShadowUniforms shadows;
    BumpmapsUniforms bumpmaps;
    BlurUniforms blur;
    LightingUniforms lighting;
    SkeletalUniforms skeletal;
    ParticlesUniforms particles;
    TextUniforms text;
};

class Shaders : boost::noncopyable {
public:
    static Shaders &instance();

    void init();
    void deinit();

    void activate(ShaderProgram program, const ShaderUniforms &uniforms);
    void deactivate();

private:
    enum class ShaderName {
        VertexSimple,
        VertexModel,
        VertexParticle,
        VertexText,
        GeometryDepth,
        FragmentColor,
        FragmentDepth,
        FragmentGUI,
        FragmentText,
        FragmentBlinnPhong,
        FragmentPBR,
        FragmentParticle,
        FragmentIrradiance,
        FragmentPrefilter,
        FragmentBRDF,
        FragmentBlur,
        FragmentPresentWorld,
        FragmentDebugCubeMap
    };

    std::unordered_map<ShaderName, uint32_t> _shaders;
    std::unordered_map<ShaderProgram, uint32_t> _programs;
    ShaderProgram _activeProgram { ShaderProgram::None };
    uint32_t _activeOrdinal { 0 };
    uint32_t _ubo { 0 };

    ~Shaders();

    void initShader(ShaderName name, unsigned int type, std::vector<const char *> sources);
    void initProgram(ShaderProgram program, std::vector<ShaderName> shaders);
    void initUBO();
    void initTextureUniforms();

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
