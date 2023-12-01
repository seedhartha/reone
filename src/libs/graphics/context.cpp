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

#include "reone/graphics/context.h"
#include "reone/graphics/texture.h"
#include "reone/graphics/textureutil.h"
#include "reone/system/randomutil.h"
#include "reone/system/threadutil.h"

namespace reone {

namespace graphics {

void GraphicsContext::init() {
    if (_inited) {
        return;
    }
    checkMainThread();
    GLenum error = glewInit();
    if (error != GLEW_OK) {
        throw std::runtime_error(str(boost::format("glewInit failed: %s") % glewGetErrorString(error)));
    }
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    setDepthTestMode(DepthTestMode::LessOrEqual);
    _depthTestModes.push(DepthTestMode::LessOrEqual);

    setCullFaceMode(CullFaceMode::None);
    _cullFaceModes.push(CullFaceMode::None);

    setBlendMode(BlendMode::None);
    _blendModes.push(BlendMode::None);

    setPolygonMode(PolygonMode::Fill);
    _polygonModes.push(PolygonMode::Fill);

    glm::ivec4 viewport(0.0f);
    glGetIntegerv(GL_VIEWPORT, &viewport[0]);
    _viewports.push(std::move(viewport));

    _default2DRGB = std::make_shared<Texture>("default_rgb", getTextureProperties(TextureUsage::Default));
    _default2DRGB->clear(1, 1, PixelFormat::RGB8);
    _default2DRGB->init();

    _defaultCubemapRGB = std::make_shared<Texture>("default_cubemap_rgb", getTextureProperties(TextureUsage::Default));
    _defaultCubemapRGB->setCubemap(true);
    _defaultCubemapRGB->clear(1, 1, PixelFormat::RGB8, kNumCubeFaces);
    _defaultCubemapRGB->init();

    _defaultCubemapDepth = std::make_shared<Texture>("default_cubemap_depth", getTextureProperties(TextureUsage::Default));
    _defaultCubemapDepth->setCubemap(true);
    _defaultCubemapDepth->clear(1, 1, PixelFormat::Depth32F, kNumCubeFaces);
    _defaultCubemapDepth->init();

    _defaultArrayDepth = std::make_shared<Texture>("default_array_depth", getTextureProperties(TextureUsage::Default));
    _defaultArrayDepth->clear(1, 1, PixelFormat::Depth32F, kNumShadowCascades);
    _defaultArrayDepth->init();

    auto noisePixels = std::make_shared<ByteBuffer>();
    noisePixels->resize(4 * 4 * 2 * sizeof(float));
    for (int i = 0; i < 4 * 4 * 2; ++i) {
        float *pixel = reinterpret_cast<float *>(&(*noisePixels)[4 * i]);
        *pixel = randomFloat(-1.0f, 1.0f);
    }
    auto noiseLayer = Texture::Layer {std::move(noisePixels)};
    _noiseRG = std::make_shared<Texture>("noise_rg", getTextureProperties(TextureUsage::Noise));
    _noiseRG->setPixels(4, 4, PixelFormat::RG16F, std::move(noiseLayer));
    _noiseRG->init();

    auto ssaoPixels = std::make_shared<ByteBuffer>();
    ssaoPixels->resize(3);
    (*ssaoPixels)[0] = 0xff;
    (*ssaoPixels)[1] = 0xff;
    (*ssaoPixels)[2] = 0xff;
    auto ssaoLayer = Texture::Layer {std::move(ssaoPixels)};
    _ssaoRGB = std::make_shared<Texture>("ssao_rgb", getTextureProperties(TextureUsage::Default));
    _ssaoRGB->setPixels(1, 1, PixelFormat::RGB8, std::move(ssaoLayer));
    _ssaoRGB->init();

    auto ssrPixels = std::make_shared<ByteBuffer>();
    ssrPixels->resize(4);
    (*ssrPixels)[0] = 0;
    (*ssrPixels)[1] = 0;
    (*ssrPixels)[2] = 0;
    (*ssrPixels)[3] = 0;
    auto ssrLayer = Texture::Layer {std::move(ssrPixels)};
    _ssrRGBA = std::make_shared<Texture>("ssr_rgba", getTextureProperties(TextureUsage::Default));
    _ssrRGBA->setPixels(1, 1, PixelFormat::RGBA8, std::move(ssrLayer));
    _ssrRGBA->init();

    _inited = true;
}

void GraphicsContext::clearColor(glm::vec4 color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void GraphicsContext::clearDepth() {
    checkMainThread();
    glClear(GL_DEPTH_BUFFER_BIT);
}

void GraphicsContext::clearColorDepth(glm::vec4 color) {
    checkMainThread();
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GraphicsContext::bind(Texture &texture, int unit) {
    if (_activeTexUnit != unit) {
        glActiveTexture(GL_TEXTURE0 + unit);
        _activeTexUnit = unit;
    }
    texture.bind();
}

void GraphicsContext::bindBuiltInTextures() {
    bind(*_default2DRGB, TextureUnits::mainTex);
    bind(*_default2DRGB, TextureUnits::lightmap);
    bind(*_default2DRGB, TextureUnits::environmentMap);
    bind(*_default2DRGB, TextureUnits::bumpMap);
    bind(*_default2DRGB, TextureUnits::envmapColor);
    bind(*_default2DRGB, TextureUnits::selfIllumColor);
    bind(*_default2DRGB, TextureUnits::features);
    bind(*_default2DRGB, TextureUnits::eyePos);
    bind(*_default2DRGB, TextureUnits::eyeNormal);
    bind(*_default2DRGB, TextureUnits::hilights);
    bind(*_default2DRGB, TextureUnits::oitAccum);
    bind(*_default2DRGB, TextureUnits::oitRevealage);
    bind(*_defaultCubemapRGB, TextureUnits::environmentMapCube);
    bind(*_defaultCubemapDepth, TextureUnits::shadowMapCube);
    bind(*_defaultArrayDepth, TextureUnits::shadowMapArray);

    bind(*_noiseRG, TextureUnits::noise);
    bind(*_ssaoRGB, TextureUnits::ssao);
    bind(*_ssrRGBA, TextureUnits::ssr);
}

void GraphicsContext::withBlending(BlendMode mode, const std::function<void()> &block) {
    if (_blendModes.top() == mode) {
        block();
        return;
    }

    setBlendMode(mode);
    _blendModes.push(mode);

    block();

    _blendModes.pop();
    setBlendMode(_blendModes.top());
}

void GraphicsContext::withDepthTest(DepthTestMode mode, const std::function<void()> &block) {
    if (_depthTestModes.top() == mode) {
        block();
        return;
    }
    setDepthTestMode(mode);
    _depthTestModes.push(mode);

    block();

    _depthTestModes.pop();
    setDepthTestMode(_depthTestModes.top());
}

void GraphicsContext::withFaceCulling(CullFaceMode mode, const std::function<void()> &block) {
    if (_cullFaceModes.top() == mode) {
        block();
        return;
    }
    setCullFaceMode(mode);
    _cullFaceModes.push(mode);

    block();

    _cullFaceModes.pop();
    setCullFaceMode(_cullFaceModes.top());
}

void GraphicsContext::withPolygonMode(PolygonMode mode, const std::function<void()> &block) {
    if (_polygonModes.top() == mode) {
        block();
        return;
    }
    setPolygonMode(mode);
    _polygonModes.push(mode);

    block();

    _polygonModes.pop();
    setPolygonMode(_polygonModes.top());
}

void GraphicsContext::withViewport(glm::ivec4 viewport, const std::function<void()> &block) {
    if (_viewports.top() == viewport) {
        block();
        return;
    }
    setViewport(viewport);
    _viewports.push(viewport);

    block();

    _viewports.pop();
    setViewport(_viewports.top());
}

void GraphicsContext::withScissorTest(const glm::ivec4 &bounds, const std::function<void()> &block) {
    glEnable(GL_SCISSOR_TEST);
    glScissor(bounds[0], bounds[1], bounds[2], bounds[3]);
    glClear(GL_COLOR_BUFFER_BIT);

    block();

    glDisable(GL_SCISSOR_TEST);
}

void GraphicsContext::setDepthTestMode(DepthTestMode mode) {
    if (mode == DepthTestMode::None) {
        glDisable(GL_DEPTH_TEST);
    } else {
        glEnable(GL_DEPTH_TEST);
        switch (mode) {
            break;
        case DepthTestMode::Equal:
            glDepthFunc(GL_EQUAL);
            break;
        case DepthTestMode::LessOrEqual:
            glDepthFunc(GL_LEQUAL);
            break;
        case DepthTestMode::Always:
            glDepthFunc(GL_ALWAYS);
            break;
        case DepthTestMode::Less:
        default:
            glDepthFunc(GL_LESS);
            break;
        }
    }
}

void GraphicsContext::setCullFaceMode(CullFaceMode mode) {
    if (mode == CullFaceMode::None) {
        glDisable(GL_CULL_FACE);
    } else {
        glEnable(GL_CULL_FACE);
        if (mode == CullFaceMode::Front) {
            glCullFace(GL_FRONT);
        } else {
            glCullFace(GL_BACK);
        }
    }
}

void GraphicsContext::setBlendMode(BlendMode mode) {
    if (mode == BlendMode::None) {
        glDisable(GL_BLEND);
    } else {
        glEnable(GL_BLEND);
        switch (mode) {
        case BlendMode::Additive:
            glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ONE);
            break;
        case BlendMode::Lighten:
            glBlendEquationSeparate(GL_MAX, GL_FUNC_ADD);
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
            break;
        case BlendMode::OIT_Transparent:
            glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
            glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case BlendMode::Normal:
        default:
            glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
            break;
        }
    }
}

void GraphicsContext::setPolygonMode(PolygonMode mode) {
    if (mode == PolygonMode::Line) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void GraphicsContext::setViewport(glm::ivec4 viewport) {
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}

} // namespace graphics

} // namespace reone
