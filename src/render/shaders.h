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

#include "glm/glm.hpp"

#include "../common/singleton.h"

#include "types.h"

namespace reone {

namespace render {

static constexpr int kMaxBoneCount = 128;

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
    BillboardBillboard
};

struct TextureUnits {
    static constexpr int diffuse { 0 };
    static constexpr int lightmap { 1 };
    static constexpr int envmap { 2 };
    static constexpr int bumpmap { 3 };
    static constexpr int bloom { 5 };
    static constexpr int irradianceMap { 6 };
    static constexpr int prefilterMap { 7 };
    static constexpr int brdfLookup { 8 };
    static constexpr int shadowMap { 9 };
};

struct GlobalUniforms {
    glm::mat4 projection { 1.0f };
    glm::mat4 view { 1.0f };
    glm::vec3 cameraPosition { 0.0f };
    float farPlane { 1.0f };
    bool shadowLightPresent { false };
    glm::vec3 shadowLightPosition { 0.0f };
    glm::mat4 shadowMatrices[kNumCubeFaces];
};

struct GeneralUniforms {
    int diffuseEnabled { false };
    int lightmapEnabled { false };
    int envmapEnabled { false };
    int pbrIblEnabled { false };
    int bumpmapEnabled { false };
    int skeletalEnabled { false };
    int lightingEnabled { false };
    int selfIllumEnabled { false };
    int blurEnabled { false };
    int bloomEnabled { false };
    int discardEnabled { false };
    int shadowsEnabled { false };
    int billboardEnabled { false };
    char padding[12];

    glm::mat4 model { 1.0f };
    glm::vec4 color { 1.0f };
    float alpha { 1.0f };
    char padding2[12];
    glm::vec4 selfIllumColor { 1.0f };
    glm::vec4 discardColor { 0.0f };
    glm::vec2 blurResolution { 0.0f };
    glm::vec2 blurDirection { 0.0f };
    glm::vec2 uvOffset { 0.0f };
    int water { 0 };
    float waterAlpha { 1.0f };
    float roughness { 1.0f };
    char padding3[12];
};

struct SkeletalUniforms {
    glm::mat4 absTransform { 1.0f };
    glm::mat4 absTransformInv { 1.0f };
    glm::mat4 bones[kMaxBoneCount];
};

struct ShaderLight {
    glm::vec4 position { 0.0f }; /**< w = 0.0 indicates a directional light, while w = 1.0 indicates a point light */
    glm::vec4 color { 1.0f };
    float radius { 1.0f };
    float multiplier { 1.0f };
    char padding[8];
};

struct LightingUniforms {
    glm::vec4 ambientLightColor { 1.0f };
    glm::vec4 materialAmbient { 1.0f };
    glm::vec4 materialDiffuse { 0.0f };
    float materialSpecular { 0.0f };
    float materialShininess { 0.0f };
    float materialMetallic { 0.0f };
    float materialRoughness { 1.0f };
    int lightCount { 0 };
    char padding[12];
    ShaderLight lights[kMaxLightCount];
};

struct BillboardUniforms {
    glm::vec2 gridSize { 0.0f };
    glm::vec2 size { 0.0f };
    glm::vec4 particleCenter { 0.0f };
    int frame { 0 };
    int render { 0 };
    char padding[8];
};

struct BumpmapUniforms {
    int grayscale { 0 };
    float scaling { 0.0f };
    glm::vec2 gridSize { 1.0f };
    int frame { 0 };
    int swizzled { 0 };
    char padding[8];
};

struct LocalUniforms {
    GeneralUniforms general;
    BillboardUniforms billboard;
    BumpmapUniforms bumpmap;
    std::shared_ptr<SkeletalUniforms> skeletal;
    std::shared_ptr<LightingUniforms> lighting;
};

class Shaders : public Singleton {
public:
    static Shaders &instance();

    void init();
    void deinit();
    void activate(ShaderProgram program, const LocalUniforms &uniforms);
    void deactivate();

    std::shared_ptr<LightingUniforms> lightingUniforms() const { return _lightingUniforms; }
    std::shared_ptr<SkeletalUniforms> skeletalUniforms() const { return _skeletalUniforms; }

    void setGlobalUniforms(const GlobalUniforms &globals);

private:
    enum class ShaderName {
        VertexSimple,
        VertexModel,
        VertexBillboard,
        GeometryDepth,
        FragmentColor,
        FragmentDepth,
        FragmentGUI,
        FragmentBlinnPhong,
        FragmentPBR,
        FragmentBillboard,
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
    std::shared_ptr<LightingUniforms> _lightingUniforms;
    std::shared_ptr<SkeletalUniforms> _skeletalUniforms;

    // Uniform buffer objects

    uint32_t _generalUbo { 0 };
    uint32_t _lightingUbo { 0 };
    uint32_t _skeletalUbo { 0 };
    uint32_t _billboardUbo { 0 };
    uint32_t _bumpmapUbo { 0 };

    // END Uniform buffer objects

    Shaders();
    ~Shaders();

    void initShader(ShaderName name, unsigned int type, std::vector<char *> sources);
    void initProgram(ShaderProgram program, std::vector<ShaderName> shaders);
    unsigned int getOrdinal(ShaderProgram program) const;
    void setLocalUniforms(const LocalUniforms &locals);
    void setUniform(const std::string &name, int value);
    void setUniform(const std::string &name, const std::function<void(int)> &setter);
    void setUniform(const std::string &name, float value);
    void setUniform(const std::string &name, const glm::vec2 &v);
    void setUniform(const std::string &name, const glm::vec3 &v);
    void setUniform(const std::string &name, const glm::mat4 &m);
    void setUniform(const std::string &name, const std::vector<glm::mat4> &arr);
};

} // namespace render

} // namespace reone
