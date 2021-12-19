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

#include "types.h"

namespace reone {

namespace graphics {

struct UniformsFeatureFlags {
    static constexpr int diffuse = 1;
    static constexpr int lightmap = 2;
    static constexpr int envmap = 4;
    static constexpr int normalmap = 8;
    static constexpr int heightmap = 0x10;
    static constexpr int skeletal = 0x20;
    static constexpr int lighting = 0x40;
    static constexpr int selfIllum = 0x80;
    static constexpr int discard = 0x100;
    static constexpr int shadows = 0x200;
    static constexpr int particles = 0x400;
    static constexpr int water = 0x800;
    static constexpr int blur = 0x1000;
    static constexpr int text = 0x2000;
    static constexpr int grass = 0x4000;
    static constexpr int fog = 0x8000;
    static constexpr int danglymesh = 0x10000;
};

struct GeneralUniforms {
    glm::mat4 projection {1.0f};
    glm::mat4 view {1.0f};
    glm::mat4 model {1.0f};
    glm::mat3x4 uv {1.0f};
    glm::vec4 cameraPosition {0.0f};
    glm::vec4 color {1.0f};
    glm::vec4 worldAmbientColor {1.0f};
    glm::vec4 ambientColor {1.0f};
    glm::vec4 diffuseColor {0.0f};
    glm::vec4 selfIllumColor {1.0f};
    glm::vec4 discardColor {0.0f};
    glm::vec4 fogColor {0.0f};
    glm::vec4 heightMapFrameBounds {0.0f};
    glm::vec4 shadowLightPosition {0.0f}; /**< W = 0 if light is directional */
    glm::vec2 blurResolution {0.0f};
    glm::vec2 blurDirection {0.0f};
    float alpha {1.0f};
    float waterAlpha {1.0f};
    float fogNear {0.0f};
    float fogFar {0.0f};
    float heightMapScaling {1.0f};
    float shadowStrength {0.0f};
    int featureMask {0}; /**< any combination of UniformFeaturesFlags */
    char padding[4];
    glm::mat4 shadowLightSpaceMatrices[kNumCubeFaces] {glm::mat4(1.0f)};

    void resetGlobals() {
        projection = glm::mat4(1.0f);
        view = glm::mat4(1.0f);
        cameraPosition = glm::vec4(0.0f);
        worldAmbientColor = glm::vec4(1.0f);
        fogColor = glm::vec4(0.0f);
        shadowLightPosition = glm::vec4(0.0f);
        fogNear = 0.0f;
        fogFar = 0.0f;
        shadowStrength = 1.0f;
        for (int i = 0; i < kNumCubeFaces; ++i) {
            shadowLightSpaceMatrices[i] = glm::mat4(1.0f);
        }
    }

    void resetLocals() {
        model = glm::mat4(1.0f);
        uv = glm::mat3x4(1.0f);
        color = glm::vec4(1.0f);
        ambientColor = glm::vec4(1.0f);
        diffuseColor = glm::vec4(0.0f);
        selfIllumColor = glm::vec4(1.0f);
        discardColor = glm::vec4(0.0f);
        heightMapFrameBounds = glm::vec4(0.0f);
        blurResolution = glm::vec2(0.0f);
        blurDirection = glm::vec2(0.0f);
        alpha = 1.0f;
        waterAlpha = 1.0f;
        heightMapScaling = 1.0f;
        featureMask = 0;
    }
};

struct LightUniforms {
    glm::vec4 position {0.0f}; /**< W = 0 if light is directional */
    glm::vec4 color {1.0f};
    float multiplier {1.0f};
    float radius {1.0f};
    int ambientOnly {0};
    char padding[4];
};

struct LightingUniforms {
    int lightCount {0};
    char padding[12];
    LightUniforms lights[kMaxLights];
};

struct SkeletalUniforms {
    glm::mat4 bones[kMaxBones] {glm::mat4(1.0f)};
};

struct ParticleUniforms {
    glm::mat4 transform {1.0f};
    glm::vec4 dir {0.0f};
    glm::vec4 color {1.0f};
    glm::vec2 size {0.0f};
    int frame {0};
    char padding[4];
};

struct ParticlesUniforms {
    glm::ivec2 gridSize {0};
    int render {0};
    char padding[4];
    ParticleUniforms particles[kMaxParticles];
};

struct GrassClusterUniforms {
    glm::vec4 positionVariant {0.0f}; /**< fourth component is a variant (0-3) */
    glm::vec2 lightmapUV {0.0f};
    char padding[8];
};

struct GrassUniforms {
    glm::vec2 quadSize {0.0f};
    char padding[8];
    GrassClusterUniforms clusters[kMaxGrassClusters];
};

struct TextCharacterUniforms {
    glm::vec4 posScale {0.0f};
    glm::vec4 uv {0.0f};
};

struct TextUniforms {
    TextCharacterUniforms chars[kMaxTextChars];
};

struct DanglymeshUniforms {
    glm::vec4 stride {0.0f};
    float displacement {0.0f};
    char padding[12];
    glm::vec4 constraints[kMaxDanglymeshConstraints] {glm::vec4(0.0f)};
};

struct Uniforms {
    GeneralUniforms general;
    TextUniforms text;
    LightingUniforms lighting;
    SkeletalUniforms skeletal;
    ParticlesUniforms particles;
    GrassUniforms grass;
    DanglymeshUniforms danglymesh;
};

} // namespace graphics

} // namespace reone
