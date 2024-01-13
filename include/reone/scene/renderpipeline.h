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
#include "reone/graphics/renderpass.h"
#include "reone/graphics/texture.h"

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

class Context;
class MeshRegistry;
class PBRTextures;
class ShaderRegistry;
class TextureRegistry;
class Uniforms;

struct GraphicsOptions;

} // namespace graphics

namespace scene {

enum class RendererType {
    Retro,
    PBR
};

class IRenderPipeline {
public:
    virtual ~IRenderPipeline() = default;

    virtual void init() = 0;

    virtual void reset() = 0;
    virtual void inRenderPass(graphics::RenderPassName name, std::function<void(graphics::IRenderPass &)> block) = 0;

    virtual graphics::Texture &render() = 0;
};

class IRenderPipelineFactory {
public:
    virtual ~IRenderPipelineFactory() = default;

    virtual std::unique_ptr<IRenderPipeline> create(RendererType type, glm::ivec2 targetSize) = 0;
};

class RenderPipelineBase : public IRenderPipeline, boost::noncopyable {
public:
    using RenderPassCallback = std::function<void(graphics::IRenderPass &)>;

    void init() override;

    void reset() override {
        _passCallbacks.clear();
    }

    void inRenderPass(graphics::RenderPassName name, RenderPassCallback callback) override {
        _passCallbacks[name] = std::move(callback);
    }

    graphics::Texture &render() override;

protected:
    struct RenderTargets {
        std::shared_ptr<graphics::Texture> cbGBufDiffuse;
        std::shared_ptr<graphics::Texture> cbGBufLightmap;
        std::shared_ptr<graphics::Texture> cbGBufEnvMap;
        std::shared_ptr<graphics::Texture> cbGBufSelfIllum;
        std::shared_ptr<graphics::Texture> cbGBufFeatures;
        std::shared_ptr<graphics::Texture> cbGBufEyePos;
        std::shared_ptr<graphics::Texture> cbGBufEyeNormal;
        std::shared_ptr<graphics::Texture> cbGBufPBRIrradiance;
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

    struct GaussianBlurParams {
        bool vertical {false};
        bool strong {false};
    };

    glm::ivec2 _targetSize;
    graphics::GraphicsOptions &_options;
    graphics::Context &_context;
    graphics::MeshRegistry &_meshRegistry;
    graphics::PBRTextures &_pbrTextures;
    graphics::ShaderRegistry &_shaderRegistry;
    graphics::TextureRegistry &_textureRegistry;
    graphics::Uniforms &_uniforms;

    bool _inited {false};

    RenderTargets _renderTargets;
    glm::mat4 _shadowLightSpace[graphics::kNumShadowLightSpace] {glm::mat4(1.0f)};
    glm::vec4 _shadowCascadeFarPlanes {glm::vec4(0.0f)};

    graphics::RenderPassName _passName {graphics::RenderPassName::None};
    std::map<graphics::RenderPassName, RenderPassCallback> _passCallbacks;

    RenderPipelineBase(glm::ivec2 targetSize,
                       graphics::GraphicsOptions &options,
                       graphics::Context &context,
                       graphics::MeshRegistry &meshRegistry,
                       graphics::PBRTextures &pbrTextures,
                       graphics::ShaderRegistry &shaderRegistry,
                       graphics::TextureRegistry &textureRegistry,
                       graphics::Uniforms &uniforms) :
        _targetSize(std::move(targetSize)),
        _options(options),
        _context(context),
        _meshRegistry(meshRegistry),
        _pbrTextures(pbrTextures),
        _shaderRegistry(shaderRegistry),
        _textureRegistry(textureRegistry),
        _uniforms(uniforms) {
    }

    void initRenderTargets();
    void initSSAOSamples();

    void combineOpaqueGeometry();
    void blendTransparentGeometry();

    void renderSSAO(float sampleRadius, float bias);
    void renderSSR(float bias, float pixelStride, float maxSteps);

    void applyBoxBlur(graphics::Texture &tex, graphics::Framebuffer &dst, const glm::ivec2 &size);
    void applyGaussianBlur(graphics::Texture &tex, graphics::Framebuffer &dst, const glm::ivec2 &size, const GaussianBlurParams &params);
    void applyMedianFilter(graphics::Texture &tex, graphics::Framebuffer &dst, const glm::ivec2 &size, bool strong = false);
    void applyFXAA(graphics::Texture &tex, graphics::Framebuffer &dst, const glm::ivec2 &size);
    void applySharpen(graphics::Texture &tex, graphics::Framebuffer &dst, const glm::ivec2 &size, float amount);

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

class RetroRenderPipeline : public RenderPipelineBase {
public:
    RetroRenderPipeline(glm::ivec2 targetSize,
                        graphics::GraphicsOptions &options,
                        graphics::Context &context,
                        graphics::MeshRegistry &meshRegistry,
                        graphics::PBRTextures &pbrTextures,
                        graphics::ShaderRegistry &shaderRegistry,
                        graphics::TextureRegistry &textureRegistry,
                        graphics::Uniforms &uniforms) :
        RenderPipelineBase(
            std::move(targetSize),
            options,
            context,
            meshRegistry,
            pbrTextures,
            shaderRegistry,
            textureRegistry,
            uniforms) {
    }
};

class PBRRenderPipeline : public RenderPipelineBase {
public:
    PBRRenderPipeline(glm::ivec2 targetSize,
                      graphics::GraphicsOptions &options,
                      graphics::Context &context,
                      graphics::MeshRegistry &meshRegistry,
                      graphics::PBRTextures &pbrTextures,
                      graphics::ShaderRegistry &shaderRegistry,
                      graphics::TextureRegistry &textureRegistry,
                      graphics::Uniforms &uniforms) :
        RenderPipelineBase(
            std::move(targetSize),
            options,
            context,
            meshRegistry,
            pbrTextures,
            shaderRegistry,
            textureRegistry,
            uniforms) {
    }
};

class RenderPipelineFactory : public IRenderPipelineFactory, boost::noncopyable {
public:
    RenderPipelineFactory(graphics::GraphicsOptions &options,
                          graphics::Context &context,
                          graphics::MeshRegistry &meshRegistry,
                          graphics::PBRTextures &pbrTextures,
                          graphics::ShaderRegistry &shaderRegistry,
                          graphics::TextureRegistry &textureRegistry,
                          graphics::Uniforms &uniforms) :
        _options(options),
        _context(context),
        _meshRegistry(meshRegistry),
        _pbrTextures(pbrTextures),
        _shaderRegistry(shaderRegistry),
        _textureRegistry(textureRegistry),
        _uniforms(uniforms) {
    }

    std::unique_ptr<IRenderPipeline> create(RendererType type, glm::ivec2 targetSize) override {
        switch (type) {
        case RendererType::Retro:
            return std::make_unique<RetroRenderPipeline>(
                std::move(targetSize),
                _options,
                _context,
                _meshRegistry,
                _pbrTextures,
                _shaderRegistry,
                _textureRegistry,
                _uniforms);
        case RendererType::PBR:
            return std::make_unique<PBRRenderPipeline>(
                std::move(targetSize),
                _options,
                _context,
                _meshRegistry,
                _pbrTextures,
                _shaderRegistry,
                _textureRegistry,
                _uniforms);
        default:
            throw std::invalid_argument("Unsupported renderer type: " + std::to_string(static_cast<int>(type)));
        }
    }

private:
    graphics::GraphicsOptions &_options;
    graphics::Context &_context;
    graphics::MeshRegistry &_meshRegistry;
    graphics::PBRTextures &_pbrTextures;
    graphics::ShaderRegistry &_shaderRegistry;
    graphics::TextureRegistry &_textureRegistry;
    graphics::Uniforms &_uniforms;
};

} // namespace scene

} // namespace reone
