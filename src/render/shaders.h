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

#include "types.h"

namespace reone {

namespace render {

constexpr int kMaxBoneCount = 128;

enum class ShaderProgram {
    None,
    GUIGUI,
    GUIBlur,
    GUIBloom,
    GUIWhite,
    GUIDebugShadows,
    ModelWhite,
    ModelModel,
    BillboardBillboard,
    DepthDepth
};

struct TextureUnits {
    static constexpr int envmap { 1 };
    static constexpr int lightmap { 2 };
    static constexpr int bumpyShiny { 3 };
    static constexpr int bumpmap { 4 };
    static constexpr int bloom { 5 };
    static constexpr int shadowmap { 6 };
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
    int lightmapEnabled { false };
    int envmapEnabled { false };
    int bumpyShinyEnabled { false };
    int bumpmapEnabled { false };
    int skeletalEnabled { false };
    int lightingEnabled { false };
    int selfIllumEnabled { false };
    int blurEnabled { false };
    int bloomEnabled { false };
    int discardEnabled { false };
    int shadowsEnabled { false };
    char padding1[4];

    glm::mat4 model { 1.0f };
    glm::vec4 color { 1.0f };
    float alpha { 1.0f };
    char padding2[12];
    glm::vec4 selfIllumColor { 1.0f };
    glm::vec4 discardColor { 0.0f };
    glm::vec2 blurResolution { 0.0f };
    glm::vec2 blurDirection { 0.0f };
    glm::vec2 billboardGridSize { 0.0f };
    glm::vec2 billboardSize { 0.0f };
    glm::vec4 particleCenter { 0.0f };
    int billboardFrame { 0 };
    int billboardToWorldZ { 0 };
    int grayscaleBumpmap { 0 };
    float bumpmapScaling { 0.0f };
    glm::vec2 uvOffset { 0.0f };
    int water { 0 };
    float waterAlpha { 1.0f };
};

struct SkeletalUniforms {
    glm::mat4 absTransform { 1.0f };
    glm::mat4 absTransformInv { 1.0f };
    glm::mat4 bones[kMaxBoneCount];
};

struct ShaderLight {
    glm::vec4 position { 0.0f };
    glm::vec4 color { 1.0f };
    float radius { 1.0f };
    char padding[12];
};

struct LightingUniforms {
    glm::vec4 ambientLightColor { 1.0f };
    int lightCount { 0 };
    char padding[12];
    ShaderLight lights[kMaxLightCount];
};

struct LocalUniforms {
    GeneralUniforms general;
    std::shared_ptr<SkeletalUniforms> skeletal;
    std::shared_ptr<LightingUniforms> lighting;
};

class Shaders {
public:
    static Shaders &instance();

    void initGL();
    void deinitGL();
    void activate(ShaderProgram program, const LocalUniforms &uniforms);
    void deactivate();

    std::shared_ptr<LightingUniforms> lightingUniforms() const;
    std::shared_ptr<SkeletalUniforms> skeletalUniforms() const;

    void setGlobalUniforms(const GlobalUniforms &globals);

private:
    enum class ShaderName {
        GeometryDepth,
        VertexGUI,
        VertexModel,
        VertexDepth,
        VertexBillboard,
        FragmentWhite,
        FragmentGUI,
        FragmentModel,
        FragmentBlur,
        FragmentBloom,
        FragmentDepth,
        FragmentDebugShadows,
        FragmentBillboard
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

    // END Uniform buffer objects

    Shaders();
    Shaders(const Shaders &) = delete;
    ~Shaders();

    Shaders &operator=(const Shaders &) = delete;

    void initShader(ShaderName name, unsigned int type, const char *source);
    void initProgram(ShaderProgram program, int shaderCount, ...);
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
