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

#include "../pipeline.h"

namespace reone {

namespace scene {

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
            shaderRegistry,
            textureRegistry,
            uniforms),
        _pbrTextures(pbrTextures) {
    }

    void init() override;

    graphics::Texture &render() override;

private:
    struct RenderTargets {
        std::shared_ptr<graphics::Texture> cbGBufDiffuse;
        std::shared_ptr<graphics::Texture> cbGBufLightmap;
        std::shared_ptr<graphics::Texture> cbGBufSelfIllum;
        std::shared_ptr<graphics::Texture> cbGBufEyeNormal;
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
        std::shared_ptr<graphics::Texture> dbGBuffer;
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

    graphics::PBRTextures &_pbrTextures;

    RenderTargets _targets;

    void initRenderTargets();
    void initSSAOSamples();

    void renderSSAO(float sampleRadius, float bias);
    void renderSSR(float bias, float pixelStride, float maxSteps);

    // Render Passes

    void combineOpaqueGeometry();
    void blendTransparentGeometry();

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

} // namespace scene

} // namespace reone
