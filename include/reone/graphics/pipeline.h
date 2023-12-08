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

#include "framebuffer.h"
#include "options.h"
#include "renderbuffer.h"
#include "types.h"

template <>
struct std::hash<glm::ivec2> {
    size_t operator()(const glm::ivec2 &dim) const {
        size_t hash = 0;
        boost::hash_combine(hash, dim.x);
        boost::hash_combine(hash, dim.y);
        return hash;
    }
};

namespace reone {

namespace graphics {

enum class RenderPassName {
    None,
    DirLightShadowsPass,
    PointLightShadows,
    OpaqueGeometry,
    TransparentGeometry,
    PostProcessing
};

class GraphicsContext;
class LocalsUniforms;
class Material;
class Mesh;
class MeshRegistry;
class ShaderRegistry;
class TextureRegistry;
class Uniforms;

struct GrassInstance {
    int variant {0};
    glm::vec3 position {0.0f};
    glm::vec2 lightmapUV {0.0f};
};

class IRenderPass {
public:
    virtual ~IRenderPass() = default;

    virtual void draw(Mesh &mesh,
                      Material &material,
                      const glm::mat4 &transform,
                      const glm::mat4 &transformInv) = 0;

    virtual void drawSkinned(Mesh &mesh,
                             Material &material,
                             const glm::mat4 &transform,
                             const glm::mat4 &transformInv,
                             const std::vector<glm::mat4> &bones) = 0;

    virtual void drawBillboard(Texture &texture,
                               const glm::vec4 &color,
                               const glm::mat4 &transform,
                               const glm::mat4 &transformInv,
                               std::optional<float> size) = 0;

    virtual void drawGrass(float radius,
                           float quadSize,
                           Texture &texture,
                           std::optional<std::reference_wrapper<Texture>> &lightmap,
                           const std::vector<GrassInstance> &instances) = 0;
};

class IPipeline {
public:
    virtual ~IPipeline() = default;

    virtual void setTargetSize(glm::ivec2 size) = 0;
    virtual void inPass(RenderPassName name, std::function<void(IRenderPass &)> block) = 0;

    virtual Texture &output() = 0;
};

class RenderPass : public IRenderPass, boost::noncopyable {
public:
    RenderPass(GraphicsContext &context,
               ShaderRegistry &shaderRegistry,
               MeshRegistry &meshRegistry,
               TextureRegistry &textureRegistry,
               Uniforms &uniforms) :
        _context(context),
        _shaderRegistry(shaderRegistry),
        _meshRegistry(meshRegistry),
        _textureRegistry(textureRegistry),
        _uniforms(uniforms) {
    }

    void draw(Mesh &mesh,
              Material &material,
              const glm::mat4 &transform,
              const glm::mat4 &transformInv) override;

    void drawSkinned(Mesh &mesh,
                     Material &material,
                     const glm::mat4 &transform,
                     const glm::mat4 &transformInv,
                     const std::vector<glm::mat4> &bones) override;

    void drawBillboard(Texture &texture,
                       const glm::vec4 &color,
                       const glm::mat4 &transform,
                       const glm::mat4 &transformInv,
                       std::optional<float> size) override;

    void drawGrass(float radius,
                   float quadSize,
                   Texture &texture,
                   std::optional<std::reference_wrapper<Texture>> &lightmap,
                   const std::vector<GrassInstance> &instances) override;

private:
    GraphicsContext &_context;
    ShaderRegistry &_shaderRegistry;
    MeshRegistry &_meshRegistry;
    TextureRegistry &_textureRegistry;
    Uniforms &_uniforms;

    void applyMaterialToLocals(const Material &material, LocalsUniforms &locals);

    int materialFeatureMask(const Material &material) const;

    void withMaterialAppliedToContext(const Material &material, std::function<void()> block);
};

class Pipeline : public IPipeline, boost::noncopyable {
public:
    Pipeline(GraphicsOptions &options,
             GraphicsContext &context,
             MeshRegistry &meshRegistry,
             ShaderRegistry &shaderRegistry,
             TextureRegistry &textureRegistry,
             Uniforms &uniforms) :
        _options(options),
        _context(context),
        _meshRegistry(meshRegistry),
        _shaderRegistry(shaderRegistry),
        _textureRegistry(textureRegistry),
        _uniforms(uniforms) {
    }

    void init();

    void setTargetSize(glm::ivec2 size) override;
    void inPass(RenderPassName name, std::function<void(IRenderPass &)> block) override;

    Texture &output() override;

private:
    struct BlitFlags {
        static constexpr int color = 1;
        static constexpr int depth = 2;

        static constexpr int colorDepth = color | depth;
    };

    struct RenderTargets {
        std::shared_ptr<Texture> cbGBufferDiffuse;
        std::shared_ptr<Texture> cbGBufferLightmap;
        std::shared_ptr<Texture> cbGBufferEnvMap;
        std::shared_ptr<Texture> cbGBufferSelfIllum;
        std::shared_ptr<Texture> cbGBufferFeatures;
        std::shared_ptr<Texture> cbGBufferEyePos;
        std::shared_ptr<Texture> cbGBufferEyeNormal;
        std::shared_ptr<Texture> cbDeferredOpaque1;
        std::shared_ptr<Texture> cbDeferredOpaque2;
        std::shared_ptr<Texture> cbTransparentGeometry1;
        std::shared_ptr<Texture> cbTransparentGeometry2;
        std::shared_ptr<Texture> cbSSAO;
        std::shared_ptr<Texture> cbSSR;
        std::shared_ptr<Texture> cbPing;
        std::shared_ptr<Texture> cbPingHalf;
        std::shared_ptr<Texture> cbPong;
        std::shared_ptr<Texture> cbPongHalf;
        std::shared_ptr<Texture> cbOutput;

        std::shared_ptr<Renderbuffer> dbCommon;
        std::shared_ptr<Renderbuffer> dbCommonHalf;
        std::shared_ptr<Renderbuffer> dbGBuffer;
        std::shared_ptr<Texture> dbDirectionalLightShadows;
        std::shared_ptr<Texture> dbPointLightShadows;
        std::shared_ptr<Texture> dbOutput;

        std::shared_ptr<Framebuffer> fbPointLightShadows;
        std::shared_ptr<Framebuffer> fbDirLightShadows;
        std::shared_ptr<Framebuffer> fbOpaqueGeometry;
        std::shared_ptr<Framebuffer> fbDeferredCombine;
        std::shared_ptr<Framebuffer> fbTransparentGeometry;
        std::shared_ptr<Framebuffer> fbSSAO;
        std::shared_ptr<Framebuffer> fbSSR;
        std::shared_ptr<Framebuffer> fbPing;
        std::shared_ptr<Framebuffer> fbPingHalf;
        std::shared_ptr<Framebuffer> fbPong;
        std::shared_ptr<Framebuffer> fbPongHalf;
        std::shared_ptr<Framebuffer> fbOutput;
    };

    GraphicsOptions &_options;

    glm::mat4 _shadowLightSpace[kNumShadowLightSpace] {glm::mat4(1.0f)};
    glm::vec4 _shadowCascadeFarPlanes {glm::vec4(0.0f)};

    glm::ivec2 _targetSize {0};
    std::unordered_map<glm::ivec2, RenderTargets> _sizeToTargets;
    RenderPassName _passName {RenderPassName::None};

    // Services

    GraphicsContext &_context;
    MeshRegistry &_meshRegistry;
    ShaderRegistry &_shaderRegistry;
    TextureRegistry &_textureRegistry;
    Uniforms &_uniforms;

    // END Services

    void drawSSAO(const glm::ivec2 &dim, RenderTargets &targets, float sampleRadius, float bias);
    void drawSSR(const glm::ivec2 &dim, RenderTargets &targets, float bias, float pixelStride, float maxSteps);
    void drawOITBlend(RenderTargets &targets, Framebuffer &dst);

    void drawBoxBlur(const glm::ivec2 &dim, Texture &srcTexture, Framebuffer &dst);
    void drawGaussianBlur(const glm::ivec2 &dim, Texture &srcTexture, Framebuffer &dst, bool vertical, bool strong = false);
    void drawMedianFilter(const glm::ivec2 &dim, Texture &srcTexture, Framebuffer &dst, bool strong = false);
    void drawFXAA(const glm::ivec2 &dim, Texture &srcTexture, Framebuffer &dst);
    void drawSharpen(const glm::ivec2 &dim, Texture &srcTexture, Framebuffer &dst, float amount);

    void blitFramebuffer(const glm::ivec2 &dim, Framebuffer &src, int srcColorIdx, Framebuffer &dst, int dstColorIdx, int flags = BlitFlags::color);

    // Render Targets

    void initTargets(glm::ivec2 size);

    RenderTargets &targetsForSize(const glm::ivec2 &size);

    // END Render Targets

    // Render Passes

    void beginDirLightShadowsPass();
    void beginPointLightShadowsPass();
    void beginOpaqueGeometryPass();
    void beginTransparentGeometryPass();
    void beginPostProcessingPass();

    void endDirLightShadowsPass();
    void endPointLightShadowsPass();
    void endOpaqueGeometryPass();
    void endTransparentGeometryPass();
    void endPostProcessingPass();

    // END Render Passes
};

} // namespace graphics

} // namespace reone
