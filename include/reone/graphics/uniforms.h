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

struct UniformBlockBindingPoints {
    static constexpr int globals = 0;
    static constexpr int locals = 1;
    static constexpr int bones = 2;
    static constexpr int dangly = 3;
    static constexpr int saber = 4;
    static constexpr int particles = 5;
    static constexpr int grass = 6;
    static constexpr int walkmesh = 7;
    static constexpr int text = 8;
    static constexpr int screenEffect = 9;
};

struct UniformsFeatureFlags {
    static constexpr int lightmap = 1 << 0;
    static constexpr int envmap = 1 << 1;
    static constexpr int normalmap = 1 << 2;
    static constexpr int bumpmap = 1 << 3;
    static constexpr int skin = 1 << 4;
    static constexpr int dangly = 1 << 5;
    static constexpr int saber = 1 << 6;
    static constexpr int shadows = 1 << 7;
    static constexpr int water = 1 << 8;
    static constexpr int fog = 1 << 9;
    static constexpr int fixedsize = 1 << 10;
    static constexpr int hashedalphatest = 1 << 11;
    static constexpr int premulalpha = 1 << 12;
    static constexpr int envmapcube = 1 << 13;
    static constexpr int staticobj = 1 << 14;
};

struct alignas(16) GlobalUniformsLight {
    glm::vec4 position {0.0f}; /**< W = 0 if light is directional */
    glm::vec4 color {1.0f};
    float multiplier {1.0f};
    float radius {1.0f};
    int ambientOnly {0};
    int dynamicType {0};
};

struct GlobalUniforms {
    glm::mat4 projection {1.0f};
    glm::mat4 projectionInv {1.0f};
    glm::mat4 view {1.0f};
    glm::mat4 viewInv {1.0f};
    glm::vec4 cameraPosition {0.0f};
    glm::vec4 worldAmbientColor {1.0f};
    GlobalUniformsLight lights[kMaxLights];
    glm::vec4 shadowLightPosition {0.0f}; /**< W = 0 if light is directional */
    glm::vec4 shadowCascadeFarPlanes {0.0f};
    glm::mat4 shadowLightSpace[kNumShadowLightSpace] {glm::mat4(1.0f)};
    glm::vec4 fogColor {0.0f};
    float clipNear {kDefaultClipPlaneNear};
    float clipFar {kDefaultClipPlaneFar};
    int numLights {0};
    float shadowStrength {0.0f};
    float shadowRadius {0.0f};
    float fogNear {0.0f};
    float fogFar {0.0f};

    void reset() {
        projection = glm::mat4(1.0f);
        projectionInv = glm::mat4(1.0f);
        view = glm::mat4(1.0f);
        viewInv = glm::mat4(1.0f);
        cameraPosition = glm::vec4(0.0f);
        worldAmbientColor = glm::vec4(1.0f);
        shadowLightPosition = glm::vec4(0.0f);
        shadowCascadeFarPlanes = glm::vec4(0.0f);
        fogColor = glm::vec4(0.0f);
        clipNear = kDefaultClipPlaneNear;
        clipFar = kDefaultClipPlaneFar;
        numLights = 0;
        shadowStrength = 1.0f;
        shadowRadius = 0.0f;
        fogNear = 0.0f;
        fogFar = 0.0f;
    }
};

struct LocalUniforms {
    glm::mat4 model;
    glm::mat4 modelInv;
    glm::mat3x4 uv;
    glm::vec4 color;
    glm::vec4 ambientColor;
    glm::vec4 diffuseColor;
    glm::vec4 selfIllumColor;
    int featureMask;
    int bumpMapFrame;
    float bumpMapScale;
    float waterAlpha;
    float billboardSize;

    LocalUniforms() {
        reset();
    }

    void reset() {
        model = glm::mat4(1.0f);
        modelInv = glm::mat4(1.0f);
        uv = glm::mat3x4(1.0f);
        color = glm::vec4(1.0f);
        ambientColor = glm::vec4(1.0f);
        diffuseColor = glm::vec4(1.0f);
        selfIllumColor = glm::vec4(0.0f);
        featureMask = 0;
        bumpMapFrame = 0;
        bumpMapScale = 1.0f;
        waterAlpha = 0.0f;
        billboardSize = 1.0f;
    }
};

struct BoneUniforms {
    glm::mat4 bones[kMaxBones] {glm::mat4(1.0f)};
};

struct DanglyUniforms {
    glm::vec4 positions[kMaxDanglyVertices] {glm::vec4(0.0f)};
};

struct alignas(16) ParticleUniformsParticle {
    glm::vec4 positionFrame {0.0f};
    glm::vec4 right {1.0f, 0.0f, 0.0f, 0.0f};
    glm::vec4 up {0.0f, 0.0f, 1.0f, 0.0f};
    glm::vec4 color {1.0f};
    glm::vec2 size {1.0f};
};

struct ParticleUniforms {
    glm::ivec2 gridSize {0};
    ParticleUniformsParticle particles[kMaxParticles];
};

struct alignas(16) GrassUniformsCluster {
    glm::vec4 positionVariant {0.0f}; /**< fourth component is a variant (0-3) */
    glm::vec2 lightmapUV {0.0f};
};

struct GrassUniforms {
    glm::vec2 quadSize {0.0f};
    float radius {0.0f};
    GrassUniformsCluster clusters[kMaxGrassClusters];
};

struct alignas(16) TextUniformsCharacter {
    glm::vec4 posScale {0.0f};
    glm::vec4 uv {0.0f};
};

struct TextUniforms {
    TextUniformsCharacter chars[kMaxTextChars];
};

struct WalkmeshUniforms {
    glm::vec4 materials[kMaxWalkmeshMaterials] {glm::vec4(1.0f)};
};

struct ScreenEffectUniforms {
    glm::mat4 projection {1.0f};
    glm::mat4 projectionInv {1.0f};
    glm::mat4 screenProjection {1.0f};
    glm::vec4 ssaoSamples[kNumSSAOSamples] {glm::vec4(0.0f)};
    glm::vec2 screenResolution {0.0f};
    glm::vec2 screenResolutionRcp {0.0f};
    glm::vec2 blurDirection {0.0f};
    float clipNear {kDefaultClipPlaneNear};
    float clipFar {kDefaultClipPlaneFar};
    float ssaoSampleRadius {0.5f};
    float ssaoBias {0.1f};
    float ssrBias {0.5f};
    float ssrPixelStride {4.0f};
    float ssrMaxSteps {32.0f};
    float sharpenAmount {0.25f};
};

class Context;

class IUniforms {
public:
    virtual ~IUniforms() = default;

    virtual void setGlobals(const std::function<void(GlobalUniforms &)> &block) = 0;
    virtual void setLocals(const std::function<void(LocalUniforms &)> &block) = 0;
    virtual void setBones(const std::function<void(BoneUniforms &)> &block) = 0;
    virtual void setDangly(const std::function<void(DanglyUniforms &)> &block) = 0;
    virtual void setParticles(const std::function<void(ParticleUniforms &)> &block) = 0;
    virtual void setGrass(const std::function<void(GrassUniforms &)> &block) = 0;
    virtual void setWalkmesh(const std::function<void(WalkmeshUniforms &)> &block) = 0;
    virtual void setText(const std::function<void(TextUniforms &)> &block) = 0;
    virtual void setScreenEffect(const std::function<void(ScreenEffectUniforms &)> &block) = 0;
};

class Uniforms : public IUniforms, boost::noncopyable {
public:
    Uniforms(Context &context) :
        _context(context) {
    }

    ~Uniforms() { deinit(); }

    void init();
    void deinit();

    void setGlobals(const std::function<void(GlobalUniforms &)> &block) override;
    void setLocals(const std::function<void(LocalUniforms &)> &block) override;
    void setBones(const std::function<void(BoneUniforms &)> &block) override;
    void setDangly(const std::function<void(DanglyUniforms &)> &block) override;
    void setParticles(const std::function<void(ParticleUniforms &)> &block) override;
    void setGrass(const std::function<void(GrassUniforms &)> &block) override;
    void setWalkmesh(const std::function<void(WalkmeshUniforms &)> &block) override;
    void setText(const std::function<void(TextUniforms &)> &block) override;
    void setScreenEffect(const std::function<void(ScreenEffectUniforms &)> &block) override;

private:
    bool _inited {false};

    Context &_context;

    // Uniforms

    GlobalUniforms _globals;
    LocalUniforms _locals;
    BoneUniforms _bones;
    DanglyUniforms _dangly;
    ParticleUniforms _particles;
    GrassUniforms _grass;
    WalkmeshUniforms _walkmesh;
    TextUniforms _text;
    ScreenEffectUniforms _screenEffect;

    // END Uniforms

    // Uniform Buffers

    std::shared_ptr<UniformBuffer> _ubGlobals;
    std::shared_ptr<UniformBuffer> _ubLocals;
    std::shared_ptr<UniformBuffer> _ubBones;
    std::shared_ptr<UniformBuffer> _ubDangly;
    std::shared_ptr<UniformBuffer> _ubParticles;
    std::shared_ptr<UniformBuffer> _ubGrass;
    std::shared_ptr<UniformBuffer> _ubWalkmesh;
    std::shared_ptr<UniformBuffer> _ubText;
    std::shared_ptr<UniformBuffer> _ubScreenEffect;

    // END Uniform Buffers

    std::unique_ptr<UniformBuffer> initBuffer(const void *data, ptrdiff_t size);
};

} // namespace graphics

} // namespace reone
