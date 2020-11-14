/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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
#include <string>
#include <unordered_map>
#include <vector>

#include "glm/glm.hpp"

namespace reone {

namespace render {

const int kMaxLightCount = 8;

enum class ShaderProgram {
    None,
    GUIGUI,
    GUIBlur,
    GUIBloom,
    GUIWhite,
    ModelWhite,
    ModelModel
};

struct GlobalUniforms {
    glm::mat4 projection { 1.0f };
    glm::mat4 view { 1.0f };
    glm::vec3 cameraPosition { 0.0f };
};

struct FeatureUniforms {
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

    bool operator==(const FeatureUniforms &other) {
        return
            lightmapEnabled == other.lightmapEnabled &&
            envmapEnabled == other.envmapEnabled &&
            bumpyShinyEnabled == other.bumpyShinyEnabled &&
            bumpmapEnabled == other.bumpmapEnabled &&
            skeletalEnabled == other.skeletalEnabled &&
            lightingEnabled == other.lightingEnabled &&
            selfIllumEnabled == other.selfIllumEnabled &&
            blurEnabled == other.blurEnabled &&
            bloomEnabled == other.bloomEnabled &&
            discardEnabled == other.discardEnabled;
    }

    bool operator!=(const FeatureUniforms &other) {
        return !operator==(other);
    }
};

struct TextureUniforms {
    static constexpr int envmap { 1 };
    static constexpr int lightmap { 2 };
    static constexpr int bumpyShiny { 3 };
    static constexpr int bumpmap { 4 };
    static constexpr int bloom { 5 };
};

struct SkeletalUniforms {
    glm::mat4 absTransform { 1.0f };
    glm::mat4 absTransformInv { 1.0f };
    std::vector<glm::mat4> bones;
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

struct GaussianBlurUniforms {
    glm::vec2 resolution { 0.0f };
    glm::vec2 direction { 0.0f };
};

struct LocalUniforms {
    FeatureUniforms features;
    SkeletalUniforms skeletal;
    LightingUniforms lighting;
    GaussianBlurUniforms blur;

    glm::mat4 model { 1.0f };
    glm::vec3 color { 1.0f };
    float alpha { 1.0f };
    glm::vec3 selfIllumColor { 1.0f };
    glm::vec3 discardColor { 0.0f };
};

class Shaders {
public:
    static Shaders &instance();

    void initGL();
    void deinitGL();
    void activate(ShaderProgram program, const LocalUniforms &uniforms);
    void deactivate();

    void setGlobalUniforms(const GlobalUniforms &globals);

private:
    enum class ShaderName {
        VertexGUI,
        VertexModel,
        FragmentWhite,
        FragmentGUI,
        FragmentModel,
        FragmentBlur,
        FragmentBloom
    };

    std::unordered_map<ShaderName, uint32_t> _shaders;
    std::unordered_map<ShaderProgram, uint32_t> _programs;
    ShaderProgram _activeProgram { ShaderProgram::None };
    uint32_t _activeOrdinal { 0 };
    uint32_t _featuresUbo { 0 };
    uint32_t _lightingUbo { 0 };

    Shaders() = default;
    Shaders(const Shaders &) = delete;
    ~Shaders();

    Shaders &operator=(const Shaders &) = delete;

    void initShader(ShaderName name, unsigned int type, const char *source);
    void initProgram(ShaderProgram program, ShaderName vertexShader, ShaderName fragmentShader);
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
