/*
 * Copyright (c) 2020-2023 The reone project contributors
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
#include "uniformbuffer.h"

namespace reone {

namespace graphics {

struct UniformsFeatureFlags {
    static constexpr int lightmap = 1;
    static constexpr int envmap = 2;
    static constexpr int normalmap = 4;
    static constexpr int heightmap = 8;
    static constexpr int skeletal = 16;
    static constexpr int shadows = 32;
    static constexpr int water = 64;
    static constexpr int fog = 128;
    static constexpr int fixedsize = 256;
    static constexpr int hashedalphatest = 512;
    static constexpr int premulalpha = 1024;
    static constexpr int envmapcube = 2048;
};

struct GlobalsUniforms {
    glm::mat4 projection {1.0f};
    glm::mat4 view {1.0f};
    glm::mat4 viewInv {1.0f};
    glm::mat4 shadowLightSpace[kNumShadowLightSpace] {glm::mat4(1.0f)};
    glm::vec4 cameraPosition {0.0f};
    glm::vec4 worldAmbientColor {1.0f};
    glm::vec4 fogColor {0.0f};
    glm::vec4 shadowLightPosition {0.0f}; /**< W = 0 if light is directional */
    glm::vec4 shadowCascadeFarPlanes {0.0f};
    float clipNear {kDefaultClipPlaneNear};
    float clipFar {kDefaultClipPlaneFar};
    float fogNear {0.0f};
    float fogFar {0.0f};
    float shadowStrength {0.0f};
    float shadowRadius {0.0f};
    float padding[2];

    void reset() {
        projection = glm::mat4(1.0f);
        view = glm::mat4(1.0f);
        viewInv = glm::mat4(1.0f);
        for (int i = 0; i < kNumShadowLightSpace; ++i) {
            shadowLightSpace[i] = glm::mat4(1.0f);
        }
        cameraPosition = glm::vec4(0.0f);
        worldAmbientColor = glm::vec4(1.0f);
        fogColor = glm::vec4(0.0f);
        shadowLightPosition = glm::vec4(0.0f);
        shadowCascadeFarPlanes = glm::vec4(0.0f);
        clipNear = kDefaultClipPlaneNear;
        clipFar = kDefaultClipPlaneFar;
        fogNear = 0.0f;
        fogFar = 0.0f;
        shadowStrength = 1.0f;
        shadowRadius = 0.0f;
    }
};

struct LocalsUniforms {
    glm::mat4 model {1.0f};
    glm::mat4 modelInv {1.0f};
    glm::mat3x4 uv {1.0f};
    glm::vec4 color {1.0f};
    glm::vec4 selfIllumColor {1.0f};
    glm::vec4 heightMapFrameBounds {0.0f};
    glm::ivec2 gridSize {0};
    int featureMask {0}; /**< any combination of UniformFeaturesFlags */
    float waterAlpha {1.0f};
    float heightMapScaling {1.0f};
    float billboardSize {1.0f};
    float padding[2];

    void reset() {
        model = glm::mat4(1.0f);
        modelInv = glm::mat4(1.0f);
        uv = glm::mat3x4(1.0f);
        color = glm::vec4(1.0f);
        selfIllumColor = glm::vec4(1.0f);
        heightMapFrameBounds = glm::vec4(0.0f);
        gridSize = glm::ivec2(0);
        featureMask = 0;
        waterAlpha = 1.0f;
        heightMapScaling = 1.0f;
        billboardSize = 1.0f;
    }
};

struct LightUniforms {
    glm::vec4 position {0.0f}; /**< W = 0 if light is directional */
    glm::vec4 color {1.0f};
    float multiplier {1.0f};
    float radius {1.0f};
    int ambientOnly {0};
    int dynamicType {0};
};

struct LightsUniforms {
    int numLights {0};
    float padding[3];
    LightUniforms lights[kMaxLights];
};

struct SkeletalUniforms {
    glm::mat4 bones[kMaxBones] {glm::mat4(1.0f)};
};

struct ParticleUniforms {
    glm::vec4 positionFrame {1.0f};
    glm::vec4 right {0.0f};
    glm::vec4 up {0.0f};
    glm::vec4 color {1.0f};
    glm::vec2 size {0.0f};
    float padding[2];
};

struct ParticlesUniforms {
    ParticleUniforms particles[kMaxParticles];
};

struct GrassClusterUniforms {
    glm::vec4 positionVariant {0.0f}; /**< fourth component is a variant (0-3) */
    glm::vec2 lightmapUV {0.0f};
    float padding[2];
};

struct GrassUniforms {
    glm::vec2 quadSize {0.0f};
    float radius {0.0f};
    float padding;
    GrassClusterUniforms clusters[kMaxGrassClusters];
};

struct TextCharacterUniforms {
    glm::vec4 posScale {0.0f};
    glm::vec4 uv {0.0f};
};

struct TextUniforms {
    TextCharacterUniforms chars[kMaxTextChars];
};

struct WalkmeshUniforms {
    glm::vec4 materials[kMaxWalkmeshMaterials] {glm::vec4(1.0f)};
};

struct PointsUniforms {
    glm::vec4 points[kMaxPoints] {glm::vec4(0.0f)};
};

struct ScreenEffectUniforms {
    glm::mat4 screenProjection {1.0f};
    glm::vec4 ssaoSamples[kNumSSAOSamples] {glm::vec4(0.0f)};
    glm::vec2 screenResolution {0.0f};
    glm::vec2 screenResolutionRcp {0.0f};
    glm::vec2 blurDirection {0.0f};
    float ssaoSampleRadius {0.5f};
    float ssaoBias {0.1f};
    float ssrBias {0.5f};
    float ssrPixelStride {4.0f};
    float ssrMaxSteps {32.0f};
    float sharpenAmount {0.25f};
};

class GraphicsContext;

class IUniforms {
public:
    virtual ~IUniforms() = default;

    virtual void setGlobals(const std::function<void(GlobalsUniforms &)> &block) = 0;
    virtual void setLocals(const std::function<void(LocalsUniforms &)> &block) = 0;
    virtual void setText(const std::function<void(TextUniforms &)> &block) = 0;
    virtual void setLights(const std::function<void(LightsUniforms &)> &block) = 0;
    virtual void setSkeletal(const std::function<void(SkeletalUniforms &)> &block) = 0;
    virtual void setParticles(const std::function<void(ParticlesUniforms &)> &block) = 0;
    virtual void setGrass(const std::function<void(GrassUniforms &)> &block) = 0;
    virtual void setWalkmesh(const std::function<void(WalkmeshUniforms &)> &block) = 0;
    virtual void setPoints(const std::function<void(PointsUniforms &)> &block) = 0;
    virtual void setScreenEffect(const std::function<void(ScreenEffectUniforms &)> &block) = 0;
};

class Uniforms : public IUniforms, boost::noncopyable {
public:
    Uniforms(GraphicsContext &context) :
        _context(context) {
    }

    ~Uniforms() { deinit(); }

    void init();
    void deinit();

    void setGlobals(const std::function<void(GlobalsUniforms &)> &block) override;
    void setLocals(const std::function<void(LocalsUniforms &)> &block) override;
    void setText(const std::function<void(TextUniforms &)> &block) override;
    void setLights(const std::function<void(LightsUniforms &)> &block) override;
    void setSkeletal(const std::function<void(SkeletalUniforms &)> &block) override;
    void setParticles(const std::function<void(ParticlesUniforms &)> &block) override;
    void setGrass(const std::function<void(GrassUniforms &)> &block) override;
    void setWalkmesh(const std::function<void(WalkmeshUniforms &)> &block) override;
    void setPoints(const std::function<void(PointsUniforms &)> &block) override;
    void setScreenEffect(const std::function<void(ScreenEffectUniforms &)> &block) override;

private:
    bool _inited {false};

    GraphicsContext &_context;

    // Uniforms

    GlobalsUniforms _globals;
    LocalsUniforms _locals;
    TextUniforms _text;
    LightsUniforms _lights;
    SkeletalUniforms _skeletal;
    ParticlesUniforms _particles;
    GrassUniforms _grass;
    WalkmeshUniforms _walkmesh;
    PointsUniforms _points;
    ScreenEffectUniforms _screenEffect;

    // END Uniforms

    // Uniform Buffers

    std::shared_ptr<UniformBuffer> _ubGlobals;
    std::shared_ptr<UniformBuffer> _ubLocals;
    std::shared_ptr<UniformBuffer> _ubText;
    std::shared_ptr<UniformBuffer> _ubLights;
    std::shared_ptr<UniformBuffer> _ubSkeletal;
    std::shared_ptr<UniformBuffer> _ubParticles;
    std::shared_ptr<UniformBuffer> _ubGrass;
    std::shared_ptr<UniformBuffer> _ubWalkmesh;
    std::shared_ptr<UniformBuffer> _ubPoints;
    std::shared_ptr<UniformBuffer> _ubScreenEffect;

    // END Uniform Buffers

    std::unique_ptr<UniformBuffer> initBuffer(const void *data, ptrdiff_t size);
};

} // namespace graphics

} // namespace reone
