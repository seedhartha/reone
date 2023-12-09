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

#include "reone/graphics/framebuffer.h"
#include "reone/graphics/renderbuffer.h"
#include "reone/graphics/texture.h"

#include "renderpass.h"

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

struct GraphicsOptions;

class Context;
class MeshRegistry;
class ShaderRegistry;
class TextureRegistry;
class Uniforms;

} // namespace graphics

namespace scene {

class IRenderPipeline {
public:
    virtual ~IRenderPipeline() = default;

    virtual void init() = 0;

    virtual void inRenderPass(RenderPassName name, std::function<void(IRenderPass &)> block) = 0;

    virtual graphics::Texture &output() = 0;
};

class IRenderPipelineFactory {
public:
    virtual ~IRenderPipelineFactory() = default;

    virtual std::unique_ptr<IRenderPipeline> create(glm::ivec2 targetSize) = 0;
};

class RenderPipeline : public IRenderPipeline, boost::noncopyable {
public:
    RenderPipeline(glm::ivec2 targetSize,
                   graphics::GraphicsOptions &options,
                   graphics::Context &context,
                   graphics::MeshRegistry &meshRegistry,
                   graphics::ShaderRegistry &shaderRegistry,
                   graphics::TextureRegistry &textureRegistry,
                   graphics::Uniforms &uniforms) :
        _targetSize(std::move(targetSize)),
        _options(options),
        _context(context),
        _meshRegistry(meshRegistry),
        _shaderRegistry(shaderRegistry),
        _textureRegistry(textureRegistry),
        _uniforms(uniforms) {
    }

    void init() override;

    void inRenderPass(RenderPassName name, std::function<void(IRenderPass &)> block) override;

    graphics::Texture &output() override;

private:
    struct BlitFlags {
        static constexpr int color = 1;
        static constexpr int depth = 2;

        static constexpr int colorDepth = color | depth;
    };

    struct RenderTargets {
        std::shared_ptr<graphics::Texture> cbGBufferDiffuse;
        std::shared_ptr<graphics::Texture> cbGBufferLightmap;
        std::shared_ptr<graphics::Texture> cbGBufferEnvMap;
        std::shared_ptr<graphics::Texture> cbGBufferSelfIllum;
        std::shared_ptr<graphics::Texture> cbGBufferFeatures;
        std::shared_ptr<graphics::Texture> cbGBufferEyePos;
        std::shared_ptr<graphics::Texture> cbGBufferEyeNormal;
        std::shared_ptr<graphics::Texture> cbDeferredOpaque1;
        std::shared_ptr<graphics::Texture> cbDeferredOpaque2;
        std::shared_ptr<graphics::Texture> cbTransparentGeometry1;
        std::shared_ptr<graphics::Texture> cbTransparentGeometry2;
        std::shared_ptr<graphics::Texture> cbSSAO;
        std::shared_ptr<graphics::Texture> cbSSR;
        std::shared_ptr<graphics::Texture> cbPing;
        std::shared_ptr<graphics::Texture> cbPingHalf;
        std::shared_ptr<graphics::Texture> cbPong;
        std::shared_ptr<graphics::Texture> cbPongHalf;
        std::shared_ptr<graphics::Texture> cbOutput;

        std::shared_ptr<graphics::Renderbuffer> dbCommon;
        std::shared_ptr<graphics::Renderbuffer> dbCommonHalf;
        std::shared_ptr<graphics::Renderbuffer> dbGBuffer;
        std::shared_ptr<graphics::Texture> dbDirectionalLightShadows;
        std::shared_ptr<graphics::Texture> dbPointLightShadows;
        std::shared_ptr<graphics::Texture> dbOutput;

        std::shared_ptr<graphics::Framebuffer> fbPointLightShadows;
        std::shared_ptr<graphics::Framebuffer> fbDirLightShadows;
        std::shared_ptr<graphics::Framebuffer> fbOpaqueGeometry;
        std::shared_ptr<graphics::Framebuffer> fbDeferredCombine;
        std::shared_ptr<graphics::Framebuffer> fbTransparentGeometry;
        std::shared_ptr<graphics::Framebuffer> fbSSAO;
        std::shared_ptr<graphics::Framebuffer> fbSSR;
        std::shared_ptr<graphics::Framebuffer> fbPing;
        std::shared_ptr<graphics::Framebuffer> fbPingHalf;
        std::shared_ptr<graphics::Framebuffer> fbPong;
        std::shared_ptr<graphics::Framebuffer> fbPongHalf;
        std::shared_ptr<graphics::Framebuffer> fbOutput;
    };

    glm::ivec2 _targetSize;
    graphics::GraphicsOptions &_options;
    graphics::Context &_context;
    graphics::MeshRegistry &_meshRegistry;
    graphics::ShaderRegistry &_shaderRegistry;
    graphics::TextureRegistry &_textureRegistry;
    graphics::Uniforms &_uniforms;

    bool _inited {false};

    RenderTargets _renderTargets;
    glm::mat4 _shadowLightSpace[graphics::kNumShadowLightSpace] {glm::mat4(1.0f)};
    glm::vec4 _shadowCascadeFarPlanes {glm::vec4(0.0f)};

    RenderPassName _passName {RenderPassName::None};

    void initRenderTargets();
    void initSSAOSamples();

    void drawSSAO(const glm::ivec2 &dim, float sampleRadius, float bias);
    void drawSSR(const glm::ivec2 &dim, float bias, float pixelStride, float maxSteps);
    void drawOITBlend(graphics::Framebuffer &dst);

    void drawBoxBlur(const glm::ivec2 &dim, graphics::Texture &srcTexture, graphics::Framebuffer &dst);
    void drawGaussianBlur(const glm::ivec2 &dim, graphics::Texture &srcTexture, graphics::Framebuffer &dst, bool vertical, bool strong = false);
    void drawMedianFilter(const glm::ivec2 &dim, graphics::Texture &srcTexture, graphics::Framebuffer &dst, bool strong = false);
    void drawFXAA(const glm::ivec2 &dim, graphics::Texture &srcTexture, graphics::Framebuffer &dst);
    void drawSharpen(const glm::ivec2 &dim, graphics::Texture &srcTexture, graphics::Framebuffer &dst, float amount);

    void blitFramebuffer(const glm::ivec2 &dim, graphics::Framebuffer &src, int srcColorIdx, graphics::Framebuffer &dst, int dstColorIdx, int flags = BlitFlags::color);

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

class RenderPipelineFactory : public IRenderPipelineFactory, boost::noncopyable {
public:
    RenderPipelineFactory(graphics::GraphicsOptions &options,
                          graphics::Context &context,
                          graphics::MeshRegistry &meshRegistry,
                          graphics::ShaderRegistry &shaderRegistry,
                          graphics::TextureRegistry &textureRegistry,
                          graphics::Uniforms &uniforms) :
        _options(options),
        _context(context),
        _meshRegistry(meshRegistry),
        _shaderRegistry(shaderRegistry),
        _textureRegistry(textureRegistry),
        _uniforms(uniforms) {
    }

    std::unique_ptr<IRenderPipeline> create(glm::ivec2 targetSize) override {
        return std::make_unique<RenderPipeline>(
            targetSize,
            _options,
            _context,
            _meshRegistry,
            _shaderRegistry,
            _textureRegistry,
            _uniforms);
    }

private:
    graphics::GraphicsOptions &_options;
    graphics::Context &_context;
    graphics::MeshRegistry &_meshRegistry;
    graphics::ShaderRegistry &_shaderRegistry;
    graphics::TextureRegistry &_textureRegistry;
    graphics::Uniforms &_uniforms;
};

} // namespace scene

} // namespace reone
