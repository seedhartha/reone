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

#include "world.h"

#include <stdexcept>

#include "glm/ext.hpp"

#include "GL/glew.h"

#include "../../render/mesh/quad.h"
#include "../../render/pbribl.h"
#include "../../render/shaders.h"
#include "../../render/stateutil.h"
#include "../../render/textures.h"
#include "../../render/textureutil.h"

using namespace std;

using namespace reone::render;

namespace reone {

namespace scene {

static constexpr int kShadowResolution = 2048;
static constexpr float kShadowFarPlane = 10000.0f;

static bool g_wireframesEnabled = false;
static bool g_debugCubeMap = false;

WorldRenderPipeline::WorldRenderPipeline(SceneGraph *scene, const GraphicsOptions &opts) :
    _scene(scene), _opts(opts) {
}

void WorldRenderPipeline::init() {
    // Geometry framebuffer

    _geometryColor1 = make_unique<Texture>("geometry_color1", getTextureProperties(TextureUsage::ColorBuffer));
    _geometryColor1->init();
    _geometryColor1->bind();
    _geometryColor1->clearPixels(_opts.width, _opts.height, Texture::PixelFormat::RGBA);
    _geometryColor1->unbind();

    _geometryColor2 = make_unique<Texture>("geometry_color2", getTextureProperties(TextureUsage::ColorBuffer));
    _geometryColor2->init();
    _geometryColor2->bind();
    _geometryColor2->clearPixels(_opts.width, _opts.height, Texture::PixelFormat::RGBA);
    _geometryColor2->unbind();

    _geometryDepth = make_unique<Texture>("geometry_depth", getTextureProperties(TextureUsage::DepthBuffer));
    _geometryDepth->init();
    _geometryDepth->bind();
    _geometryDepth->clearPixels(_opts.width, _opts.height, Texture::PixelFormat::Depth);
    _geometryDepth->unbind();

    _geometry.init();
    _geometry.bind();
    _geometry.attachColor(*_geometryColor1, 0);
    _geometry.attachColor(*_geometryColor2, 1);
    _geometry.attachDepth(*_geometryDepth);
    _geometry.checkCompleteness();
    _geometry.unbind();


    // Vertical blur framebuffer

    _verticalBlurColor = make_unique<Texture>("verticalblur_color2", getTextureProperties(TextureUsage::ColorBuffer));
    _verticalBlurColor->init();
    _verticalBlurColor->bind();
    _verticalBlurColor->clearPixels(_opts.width, _opts.height, Texture::PixelFormat::RGBA);
    _verticalBlurColor->unbind();

    _verticalBlurDepth = make_unique<Texture>("verticalblur_depth", getTextureProperties(TextureUsage::DepthBuffer));
    _verticalBlurDepth->init();
    _verticalBlurDepth->bind();
    _verticalBlurDepth->clearPixels(_opts.width, _opts.height, Texture::PixelFormat::Depth);
    _verticalBlurDepth->unbind();

    _verticalBlur.init();
    _verticalBlur.bind();
    _verticalBlur.attachColor(*_verticalBlurColor);
    _verticalBlur.attachDepth(*_verticalBlurDepth);
    _verticalBlur.checkCompleteness();
    _verticalBlur.unbind();


    // Horizontal blur framebuffer

    _horizontalBlurColor = make_unique<Texture>("horizontalblur_color2", getTextureProperties(TextureUsage::ColorBuffer));
    _horizontalBlurColor->init();
    _horizontalBlurColor->bind();
    _horizontalBlurColor->clearPixels(_opts.width, _opts.height, Texture::PixelFormat::RGBA);
    _horizontalBlurColor->unbind();

    _horizontalBlurDepth = make_unique<Texture>("horizontalblur_depth", getTextureProperties(TextureUsage::DepthBuffer));
    _horizontalBlurDepth->init();
    _horizontalBlurDepth->bind();
    _horizontalBlurDepth->clearPixels(_opts.width, _opts.height, Texture::PixelFormat::Depth);
    _horizontalBlurDepth->unbind();

    _horizontalBlur.init();
    _horizontalBlur.bind();
    _horizontalBlur.attachColor(*_horizontalBlurColor);
    _horizontalBlur.attachDepth(*_horizontalBlurDepth);
    _horizontalBlur.checkCompleteness();
    _horizontalBlur.unbind();


    // Shadows framebuffer

    _shadowsDepth = make_unique<Texture>("shadows_depth", getTextureProperties(TextureUsage::CubeMapDepthBuffer));
    _shadowsDepth->init();
    _shadowsDepth->bind();
    _shadowsDepth->clearPixels(kShadowResolution, kShadowResolution, Texture::PixelFormat::Depth);
    _shadowsDepth->unbind();

    _shadows.init();
    _shadows.bind();
    _shadows.attachDepth(*_shadowsDepth);
    _shadows.checkCompleteness();
    _shadows.unbind();
}

void WorldRenderPipeline::render() const {
    drawShadows();
    drawGeometry();

    float w = static_cast<float>(_opts.width);
    float h = static_cast<float>(_opts.height);

    GlobalUniforms globals;
    globals.projection = glm::ortho(0.0f, w, h, 0.0f);

    Shaders::instance().setGlobalUniforms(globals);

    applyHorizontalBlur();
    applyVerticalBlur();
    drawResult();
}

void WorldRenderPipeline::drawShadows() const {
    if (!_scene->isShadowLightPresent()) return;

    withViewport(glm::ivec4(0, 0, kShadowResolution, kShadowResolution), [this]() {
        _shadows.bind();

        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        glm::mat4 projection(glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, kShadowFarPlane));
        glm::vec3 lightPosition(_scene->shadowLightPosition());

        GlobalUniforms globals;
        globals.shadowLightPresent = true;
        globals.shadowLightPosition = lightPosition;

        for (int i = 0; i < kNumCubeFaces; ++i) {
            auto side = static_cast<CubeMapFace>(i);
            globals.shadowMatrices[i] = projection * getShadowView(lightPosition, side);
        }

        Shaders::instance().setGlobalUniforms(globals);

        glClear(GL_DEPTH_BUFFER_BIT);
        withDepthTest([this]() { _scene->renderNoGlobalUniforms(true); });

        _shadows.unbind();
    });
}

glm::mat4 WorldRenderPipeline::getShadowView(const glm::vec3 &lightPos, CubeMapFace side) const {
    switch (side) {
        case CubeMapFace::PositiveX:
            return glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
        case CubeMapFace::NegativeX:
            return glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
        case CubeMapFace::PositiveY:
            return glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
        case CubeMapFace::NegativeY:
            return glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0));
        case CubeMapFace::PositiveZ:
            return glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));
        case CubeMapFace::NegativeZ:
            return glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0));
        default:
            throw invalid_argument("side is invalid");
    }
}

void WorldRenderPipeline::drawGeometry() const {
    _geometry.bind();

    static constexpr GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, buffers);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (_scene->isShadowLightPresent()) {
        setActiveTextureUnit(TextureUnits::shadowMap);
        _shadowsDepth->bind();
    }

    if (g_wireframesEnabled) {
        withWireframes([this]() {
            withDepthTest([this]() { _scene->render(); });
        });
    } else {
        withDepthTest([this]() { _scene->render(); });
    }

    _geometry.unbind();
}

void WorldRenderPipeline::applyHorizontalBlur() const {
    float w = static_cast<float>(_opts.width);
    float h = static_cast<float>(_opts.height);

    _horizontalBlur.bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 transform(1.0f);
    transform = glm::scale(transform, glm::vec3(w, h, 1.0f));

    LocalUniforms locals;
    locals.general.blurEnabled = true;
    locals.general.model = move(transform);
    locals.general.blurResolution = glm::vec2(w, h);
    locals.general.blurDirection = glm::vec2(1.0f, 0.0f);

    Shaders::instance().activate(ShaderProgram::GUIBlur, locals);

    setActiveTextureUnit(TextureUnits::diffuse);
    _geometryColor2->bind();

    withDepthTest([]() {
        Quad::getDefault().renderTriangles();
    });

    _horizontalBlur.unbind();
}

void WorldRenderPipeline::applyVerticalBlur() const {
    float w = static_cast<float>(_opts.width);
    float h = static_cast<float>(_opts.height);

    _verticalBlur.bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 transform(1.0f);
    transform = glm::scale(transform, glm::vec3(w, h, 1.0f));

    LocalUniforms locals;
    locals.general.blurEnabled = true;
    locals.general.model = move(transform);
    locals.general.blurResolution = glm::vec2(_opts.width, _opts.height);
    locals.general.blurDirection = glm::vec2(0.0f, 1.0f);

    Shaders::instance().activate(ShaderProgram::GUIBlur, locals);

    setActiveTextureUnit(TextureUnits::diffuse);
    _horizontalBlurColor->bind();

    withDepthTest([]() {
        Quad::getDefault().renderTriangles();
    });

    _verticalBlur.unbind();
}

void WorldRenderPipeline::drawResult() const {
    float w = static_cast<float>(_opts.width);
    float h = static_cast<float>(_opts.height);

    glm::mat4 transform(1.0f);
    transform = glm::scale(transform, glm::vec3(w, h, 1.0f));

    if (g_debugCubeMap) {
        LocalUniforms locals;
        locals.general.model = move(transform);
        Shaders::instance().activate(ShaderProgram::GUIDebugCubeMap, locals);

        setActiveTextureUnit(TextureUnits::diffuse);

        /*
        auto envmap = Textures::instance().get("cm_endar", TextureUsage::EnvironmentMap);
        PBRIBL::Derived derived;
        if (PBRIBL::instance().getDerived(envmap.get(), derived)) {
            derived.brdfLookup->bind();
        } else {
            envmap->bind();
        }
        */

        _shadowsDepth->bind();

        Quad::getDefault().renderTriangles();

    } else {
        LocalUniforms locals;
        locals.general.bloomEnabled = true;
        locals.general.model = move(transform);

        Shaders::instance().activate(ShaderProgram::GUIBloom, locals);

        setActiveTextureUnit(TextureUnits::diffuse);
        _geometryColor1->bind();

        setActiveTextureUnit(TextureUnits::bloom);
        _verticalBlurColor->bind();

        Quad::getDefault().renderTriangles();
    }
}

} // namespace scene

} // namespace reone
