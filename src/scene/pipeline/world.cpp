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

#include "../../graphics/context.h"
#include "../../graphics/format/tgawriter.h"
#include "../../graphics/mesh.h"
#include "../../graphics/meshes.h"
#include "../../graphics/pbribl.h"
#include "../../graphics/renderbuffer.h"
#include "../../graphics/shaders.h"
#include "../../graphics/texture.h"
#include "../../graphics/textures.h"
#include "../../graphics/textureutil.h"
#include "../../graphics/window.h"

#include "../graph.h"
#include "../node/camera.h"
#include "../node/light.h"

using namespace std;

using namespace reone::graphics;

namespace reone {

namespace scene {

static constexpr float kShadowNearPlane = 0.0f;
static constexpr float kShadowFarPlane = 10000.0f;
static constexpr float kOrthographicScale = 10.0f;
static constexpr int kScreenshotResolution = 256;

static bool g_wireframesEnabled = false;

WorldRenderPipeline::WorldRenderPipeline(
    GraphicsOptions options,
    SceneGraph &sceneGraph,
    Context &context,
    Meshes &meshes,
    Shaders &shaders) :
    _options(move(options)),
    _sceneGraph(sceneGraph),
    _context(context),
    _meshes(meshes),
    _shaders(shaders) {

    for (int i = 0; i < kNumCubeFaces; ++i) {
        _lightSpaceMatrices[i] = glm::mat4(1.0f);
    }
}

void WorldRenderPipeline::init() {
    // Reusable depth renderbuffer

    _depthRenderbuffer = make_unique<Renderbuffer>();
    _depthRenderbuffer->init();
    _depthRenderbuffer->bind();
    _depthRenderbuffer->configure(_options.width, _options.height, PixelFormat::Depth);
    _context.unbindRenderbuffer();

    // Geometry framebuffer

    _geometryColor1 = make_unique<Texture>("geometry_color1", getTextureProperties(TextureUsage::ColorBuffer));
    _geometryColor1->init();
    _geometryColor1->bind();
    _geometryColor1->clearPixels(_options.width, _options.height, PixelFormat::RGB);

    _geometryColor2 = make_unique<Texture>("geometry_color2", getTextureProperties(TextureUsage::ColorBuffer));
    _geometryColor2->init();
    _geometryColor2->bind();
    _geometryColor2->clearPixels(_options.width, _options.height, PixelFormat::RGB);

    _geometry.init();
    _geometry.bind();
    _geometry.attachColor(*_geometryColor1, 0);
    _geometry.attachColor(*_geometryColor2, 1);
    _geometry.attachDepth(*_depthRenderbuffer);
    _geometry.checkCompleteness();
    _context.unbindFramebuffer();

    // Vertical blur framebuffer

    _verticalBlurColor = make_unique<Texture>("verticalblur_color", getTextureProperties(TextureUsage::ColorBuffer));
    _verticalBlurColor->init();
    _verticalBlurColor->bind();
    _verticalBlurColor->clearPixels(_options.width, _options.height, PixelFormat::RGB);

    _verticalBlur.init();
    _verticalBlur.bind();
    _verticalBlur.attachColor(*_verticalBlurColor);
    _verticalBlur.attachDepth(*_depthRenderbuffer);
    _verticalBlur.checkCompleteness();
    _context.unbindFramebuffer();

    // Horizontal blur framebuffer

    _horizontalBlurColor = make_unique<Texture>("horizontalblur_color", getTextureProperties(TextureUsage::ColorBuffer));
    _horizontalBlurColor->init();
    _horizontalBlurColor->bind();
    _horizontalBlurColor->clearPixels(_options.width, _options.height, PixelFormat::RGB);

    _horizontalBlur.init();
    _horizontalBlur.bind();
    _horizontalBlur.attachColor(*_horizontalBlurColor);
    _horizontalBlur.attachDepth(*_depthRenderbuffer);
    _horizontalBlur.checkCompleteness();
    _context.unbindFramebuffer();

    // Shadows framebuffer

    _shadowsDepth = make_unique<Texture>("shadows_depth", getTextureProperties(TextureUsage::DepthBuffer));
    _shadowsDepth->init();
    _shadowsDepth->bind();
    _shadowsDepth->clearPixels(1024 * _options.shadowResolution, 1024 * _options.shadowResolution, PixelFormat::Depth);

    _cubeShadowsDepth = make_unique<Texture>("cubeshadows_depth", getTextureProperties(TextureUsage::CubeMapDepthBuffer));
    _cubeShadowsDepth->init();
    _cubeShadowsDepth->bind();
    _cubeShadowsDepth->clearPixels(1024 * _options.shadowResolution, 1024 * _options.shadowResolution, PixelFormat::Depth);

    _shadows.init();

    // Screenshot framebuffer

    _screenshotColor = make_unique<Texture>("result_color", getTextureProperties(TextureUsage::ColorBuffer));
    _screenshotColor->init();
    _screenshotColor->bind();
    _screenshotColor->clearPixels(kScreenshotResolution, kScreenshotResolution, PixelFormat::RGB);
    _screenshotColor->unbind();

    _screenshot.init();
    _screenshot.bind();
    _screenshot.attachColor(*_screenshotColor);
    _screenshot.attachDepth(*_depthRenderbuffer);
    _screenshot.checkCompleteness();
    _context.unbindFramebuffer();
}

void WorldRenderPipeline::render() {
    shared_ptr<CameraSceneNode> camera(_sceneGraph.activeCamera());
    if (!camera)
        return;

    computeLightSpaceMatrices();

    drawShadows();
    drawGeometry();
    applyHorizontalBlur();
    applyVerticalBlur();
    drawResult();
}

static glm::mat4 getPointLightView(const glm::vec3 &lightPos, CubeMapFace face) {
    switch (face) {
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

void WorldRenderPipeline::computeLightSpaceMatrices() {
    static glm::vec3 up(0.0f, 0.0f, 1.0f);

    const LightSceneNode *shadowLight = _sceneGraph.shadowLight();
    if (!shadowLight)
        return;

    glm::vec3 lightPosition(shadowLight->absoluteTransform()[3]);
    glm::vec3 cameraPosition(_sceneGraph.activeCamera()->absoluteTransform()[3]);

    if (shadowLight->isDirectional()) {
        glm::mat4 projection(glm::ortho(-kOrthographicScale, kOrthographicScale, -kOrthographicScale, kOrthographicScale, kShadowNearPlane, kShadowFarPlane));
        glm::mat4 lightView(glm::lookAt(lightPosition, cameraPosition, up));
        _lightSpaceMatrices[0] = projection * lightView;
    } else {
        glm::mat4 projection(glm::perspective(glm::radians(90.0f), 1.0f, kShadowNearPlane, kShadowFarPlane));
        for (int i = 0; i < kNumCubeFaces; ++i) {
            glm::mat4 lightView(getPointLightView(lightPosition, static_cast<CubeMapFace>(i)));
            _lightSpaceMatrices[i] = projection * lightView;
        }
    }
}

void WorldRenderPipeline::drawShadows() {
    if (_options.shadowResolution < 1)
        return;

    // Set uniforms prototype

    const LightSceneNode *shadowLight = _sceneGraph.shadowLight();
    if (!shadowLight)
        return;

    glm::vec4 lightPosition(
        glm::vec3(shadowLight->absoluteTransform()[3]),
        shadowLight->isDirectional() ? 0.0f : 1.0f);

    ShaderUniforms uniforms(_shaders.defaultUniforms());
    uniforms.combined.featureMask |= UniformFeatureFlags::shadows;
    uniforms.combined.shadows.lightPosition = move(lightPosition);
    for (int i = 0; i < kNumCubeFaces; ++i) {
        uniforms.combined.shadows.lightSpaceMatrices[i] = _lightSpaceMatrices[i];
    }
    _sceneGraph.setUniformsPrototype(move(uniforms));

    // Set viewport

    glm::ivec4 oldViewport(_context.viewport());
    _context.setViewport(glm::ivec4(0, 0, 1024 * _options.shadowResolution, 1024 * _options.shadowResolution));

    // Enable depth testing

    bool oldDepthTest = _context.isDepthTestEnabled();
    _context.setDepthTestEnabled(true);

    // Bind shadows framebuffer

    _shadows.bind();
    if (shadowLight->isDirectional()) {
        _shadows.attachDepth(*_shadowsDepth);
    } else {
        _shadows.attachDepth(*_cubeShadowsDepth);
    }
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // Draw the scene

    glClear(GL_DEPTH_BUFFER_BIT);
    _sceneGraph.draw(true);

    // Restore context

    _context.setDepthTestEnabled(oldDepthTest);
    _context.setViewport(move(oldViewport));
}

void WorldRenderPipeline::drawGeometry() {
    // Set uniforms prototype

    shared_ptr<CameraSceneNode> camera(_sceneGraph.activeCamera());
    const LightSceneNode *shadowLight = _sceneGraph.shadowLight();

    ShaderUniforms uniforms(_shaders.defaultUniforms());
    uniforms.combined.general.projection = camera->projection();
    uniforms.combined.general.view = camera->view();
    uniforms.combined.general.cameraPosition = camera->absoluteTransform()[3];
    uniforms.combined.shadows.lightPresent = static_cast<bool>(shadowLight);

    if (shadowLight) {
        glm::vec4 lightPosition(
            glm::vec3(shadowLight->absoluteTransform()[3]),
            shadowLight->isDirectional() ? 0.0f : 1.0f);

        uniforms.combined.shadows.lightPosition = move(lightPosition);
        uniforms.combined.shadows.strength = 1.0f - shadowLight->fadeFactor();

        for (int i = 0; i < kNumCubeFaces; ++i) {
            uniforms.combined.shadows.lightSpaceMatrices[i] = _lightSpaceMatrices[i];
        }
    }

    _sceneGraph.setUniformsPrototype(move(uniforms));

    // Enable wireframe mode

    PolygonMode oldPolygonMode = _context.polygonMode();
    if (g_wireframesEnabled) {
        _context.setPolygonMode(PolygonMode::Line);
    }

    // Enable depth testing

    bool oldDepthTest = _context.isDepthTestEnabled();
    _context.setDepthTestEnabled(true);

    // Bind geometry framebuffer

    _geometry.bind();

    static constexpr GLenum buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, buffers);

    if (shadowLight) {
        if (shadowLight->isDirectional()) {
            _context.setActiveTextureUnit(TextureUnits::shadowMap);
            _shadowsDepth->bind();
        } else {
            _context.setActiveTextureUnit(TextureUnits::shadowMapCube);
            _cubeShadowsDepth->bind();
        }
    }

    // Draw the scene

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _sceneGraph.draw();

    // Restore context

    _context.unbindFramebuffer();
    _context.setPolygonMode(oldPolygonMode);
    _context.setDepthTestEnabled(oldDepthTest);
}

void WorldRenderPipeline::applyHorizontalBlur() {
    // Enable depth testing

    bool oldDepthTest = _context.isDepthTestEnabled();
    _context.setDepthTestEnabled(true);

    // Bind horizontal blur framebuffer

    _horizontalBlur.bind();

    // Bind bright geometry texture

    _context.setActiveTextureUnit(TextureUnits::diffuseMap);
    _geometryColor2->bind();

    // Set shader uniforms

    float w = static_cast<float>(_options.width);
    float h = static_cast<float>(_options.height);

    ShaderUniforms uniforms;
    uniforms.combined.featureMask |= UniformFeatureFlags::blur;
    uniforms.combined.blur.resolution = glm::vec2(w, h);
    uniforms.combined.blur.direction = glm::vec2(1.0f, 0.0f);

    _shaders.activate(ShaderProgram::SimpleBlur, uniforms);

    // Draw a quad

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _meshes.quadNDC().draw();

    // Restore context

    _context.unbindFramebuffer();
    _context.setDepthTestEnabled(oldDepthTest);
}

void WorldRenderPipeline::applyVerticalBlur() {
    // Enable depth testing

    bool oldDepthTest = _context.isDepthTestEnabled();
    _context.setDepthTestEnabled(true);

    // Bind vertical blur framebuffer

    _verticalBlur.bind();

    // Bind diffuse map

    _context.setActiveTextureUnit(TextureUnits::diffuseMap);
    _horizontalBlurColor->bind();

    // Set shader uniforms

    float w = static_cast<float>(_options.width);
    float h = static_cast<float>(_options.height);

    ShaderUniforms uniforms;
    uniforms.combined.featureMask |= UniformFeatureFlags::blur;
    uniforms.combined.blur.resolution = glm::vec2(w, h);
    uniforms.combined.blur.direction = glm::vec2(0.0f, 1.0f);

    _shaders.activate(ShaderProgram::SimpleBlur, uniforms);

    // Draw a quad

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _meshes.quadNDC().draw();

    // Restore context

    _context.unbindFramebuffer();
    _context.setDepthTestEnabled(oldDepthTest);
}

void WorldRenderPipeline::drawResult() {
    // Set viewport

    glm::ivec4 oldViewport;
    if (_takeScreenshot) {
        oldViewport = _context.viewport();
        _context.setViewport(glm::ivec4(0, 0, kScreenshotResolution, kScreenshotResolution));
        _screenshot.bind();
    }

    // Bind geometry texture

    _context.setActiveTextureUnit(TextureUnits::diffuseMap);
    _geometryColor1->bind();

    // Bind blur texture

    _context.setActiveTextureUnit(TextureUnits::bloom);
    _verticalBlurColor->bind();

    // Set shader uniforms

    ShaderUniforms uniforms;
    _shaders.activate(ShaderProgram::SimplePresentWorld, uniforms);

    // Draw a quad

    _meshes.quadNDC().draw();

    // Restore context

    if (_takeScreenshot) {
        saveScreenshot();
        _context.unbindFramebuffer();
        _context.setViewport(move(oldViewport));
        _takeScreenshot = false; // finished taking a screenshot
    }
}

void WorldRenderPipeline::saveScreenshot() {
    _screenshotColor->bind();
    _screenshotColor->flushGPUToCPU();
}

} // namespace scene

} // namespace reone
