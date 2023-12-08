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
#include "renderbuffer.h"
#include "renderpass.h"
#include "texture.h"

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

class IPipeline {
public:
    virtual ~IPipeline() = default;

    virtual void init() = 0;

    virtual void inPass(RenderPassName name, std::function<void(IRenderPass &)> block) = 0;

    virtual Texture &output() = 0;
};

class IPipelineFactory {
public:
    virtual ~IPipelineFactory() = default;

    virtual std::unique_ptr<IPipeline> create(glm::ivec2 targetSize) = 0;
};

class Pipeline : public IPipeline, boost::noncopyable {
public:
    Pipeline(glm::ivec2 targetSize,
             GraphicsOptions &options,
             Context &context,
             MeshRegistry &meshRegistry,
             ShaderRegistry &shaderRegistry,
             TextureRegistry &textureRegistry,
             Uniforms &uniforms) :
        _targetSize(std::move(targetSize)),
        _options(options),
        _context(context),
        _meshRegistry(meshRegistry),
        _shaderRegistry(shaderRegistry),
        _textureRegistry(textureRegistry),
        _uniforms(uniforms) {
    }

    void init() override;

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

    glm::ivec2 _targetSize;
    GraphicsOptions &_options;
    Context &_context;
    MeshRegistry &_meshRegistry;
    ShaderRegistry &_shaderRegistry;
    TextureRegistry &_textureRegistry;
    Uniforms &_uniforms;

    bool _inited {false};

    RenderTargets _renderTargets;
    glm::mat4 _shadowLightSpace[kNumShadowLightSpace] {glm::mat4(1.0f)};
    glm::vec4 _shadowCascadeFarPlanes {glm::vec4(0.0f)};

    RenderPassName _passName {RenderPassName::None};

    void initRenderTargets();
    void initSSAOSamples();

    void drawSSAO(const glm::ivec2 &dim, float sampleRadius, float bias);
    void drawSSR(const glm::ivec2 &dim, float bias, float pixelStride, float maxSteps);
    void drawOITBlend(Framebuffer &dst);

    void drawBoxBlur(const glm::ivec2 &dim, Texture &srcTexture, Framebuffer &dst);
    void drawGaussianBlur(const glm::ivec2 &dim, Texture &srcTexture, Framebuffer &dst, bool vertical, bool strong = false);
    void drawMedianFilter(const glm::ivec2 &dim, Texture &srcTexture, Framebuffer &dst, bool strong = false);
    void drawFXAA(const glm::ivec2 &dim, Texture &srcTexture, Framebuffer &dst);
    void drawSharpen(const glm::ivec2 &dim, Texture &srcTexture, Framebuffer &dst, float amount);

    void blitFramebuffer(const glm::ivec2 &dim, Framebuffer &src, int srcColorIdx, Framebuffer &dst, int dstColorIdx, int flags = BlitFlags::color);

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

class PipelineFactory : public IPipelineFactory, boost::noncopyable {
public:
    PipelineFactory(GraphicsOptions &options,
                    Context &context,
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

    std::unique_ptr<IPipeline> create(glm::ivec2 targetSize) override {
        return std::make_unique<Pipeline>(
            targetSize,
            _options,
            _context,
            _meshRegistry,
            _shaderRegistry,
            _textureRegistry,
            _uniforms);
    }

private:
    GraphicsOptions &_options;
    Context &_context;
    MeshRegistry &_meshRegistry;
    ShaderRegistry &_shaderRegistry;
    TextureRegistry &_textureRegistry;
    Uniforms &_uniforms;
};

} // namespace graphics

} // namespace reone
