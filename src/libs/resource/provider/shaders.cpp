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

#include "reone/resource/provider/shaders.h"

#include "reone/graphics/options.h"
#include "reone/graphics/shaderregistry.h"
#include "reone/system/stringbuilder.h"

using namespace reone::graphics;

namespace reone {

namespace resource {

static const std::string kIncludeBRDF = "i_brdf";
static const std::string kIncludeEnvMap = "i_envmap";
static const std::string kIncludeFog = "i_fog";
static const std::string kIncludeHash = "i_hash";
static const std::string kIncludeHashedAlpha = "i_hashedalpha";
static const std::string kIncludeLighting = "i_lighting";
static const std::string kIncludeLuma = "i_luma";
static const std::string kIncludeMath = "i_math";
static const std::string kIncludeNormalMap = "i_normalmap";
static const std::string kIncludeOIT = "i_oit";
static const std::string kIncludeShadowMap = "i_shadowmap";

static const std::string kUniformsGlobals = "u_globals";
static const std::string kUniformsLocals = "u_locals";
static const std::string kUniformsGrass = "u_grass";
static const std::string kUniformsLighting = "u_lighting";
static const std::string kUniformsParticle = "u_particle";
static const std::string kUniformsPoints = "u_points";
static const std::string kUniformsSkeletal = "u_skeletal";
static const std::string kUniformsText = "u_text";
static const std::string kUniformsWalkmesh = "u_walkmesh";
static const std::string kUniformsScreenEffect = "u_screeneffect";

static const std::string kVertexBillboard = "v_billboard";
static const std::string kVertexGrass = "v_grass";
static const std::string kVertexModel = "v_model";
static const std::string kVertexMVP2D = "v_mvp2d";
static const std::string kVertexMVP3D = "v_mvp3d";
static const std::string kVertexParticle = "v_particle";
static const std::string kVertexPassthrough = "v_passthrough";
static const std::string kVertexPoints = "v_points";
static const std::string kVertexShadows = "v_shadows";
static const std::string kVertexText = "v_text";
static const std::string kVertexWalkmesh = "v_walkmesh";

static const std::string kGeometryDirLightShadows = "g_dirlightshadow";
static const std::string kGeometryPointLightShadows = "g_ptlightshadow";

static const std::string kFragmentAABB = "f_aabb";
static const std::string kFragmentBillboard = "f_billboard";
static const std::string kFragmentBoxBlur4 = "f_boxblur4";
static const std::string kFragmentColor = "f_color";
static const std::string kFragmentCombineGeometry = "f_combinegeom";
static const std::string kFragmentCombineOpaque = "f_combineopaque";
static const std::string kFragmentDirLightShadows = "f_dirlightshadow";
static const std::string kFragmentFXAA = "f_fxaa";
static const std::string kFragmentGaussianBlur13 = "f_gaussianblur13";
static const std::string kFragmentGaussianBlur9 = "f_gaussianblur9";
static const std::string kFragmentGrass = "f_grass";
static const std::string kFragmentGUI = "f_gui";
static const std::string kFragmentMedianFilter3 = "f_medianfilter3";
static const std::string kFragmentMedianFilter5 = "f_medianfilter5";
static const std::string kFragmentModelOpaque = "f_modelopaque";
static const std::string kFragmentModelTransparent = "f_modeltransp";
static const std::string kFragmentParticle = "f_particle";
static const std::string kFragmentPointLightShadows = "f_ptlightshadow";
static const std::string kFragmentSharpen = "f_sharpen";
static const std::string kFragmentSSAO = "f_ssao";
static const std::string kFragmentSSR = "f_ssr";
static const std::string kFragmentText = "f_text";
static const std::string kFragmentTexture = "f_texture";
static const std::string kFragmentWalkmesh = "f_walkmesh";

void Shaders::init() {
    if (_inited) {
        return;
    }
    auto shaderpackPath = std::filesystem::current_path();
    shaderpackPath.append("shaderpack.erf");
    if (!std::filesystem::exists(shaderpackPath)) {
        throw std::runtime_error("File not found: " + shaderpackPath.string());
    }
    _sourceProvider = std::make_unique<ErfResourceContainer>(shaderpackPath);
    _sourceProvider->init();

    // Shaders
    auto vsPassthrough = initShader(ShaderType::Vertex, {kVertexPassthrough});
    auto vsMVP2D = initShader(ShaderType::Vertex, {kUniformsGlobals, kUniformsLocals, kVertexMVP2D});
    auto vsMVP3D = initShader(ShaderType::Vertex, {kUniformsGlobals, kUniformsLocals, kVertexMVP3D});
    auto vsShadows = initShader(ShaderType::Vertex, {kUniformsGlobals, kUniformsLocals, kVertexShadows});
    auto vsModel = initShader(ShaderType::Vertex, {kUniformsGlobals, kUniformsLocals, kUniformsSkeletal, kVertexModel});
    auto vsWalkmesh = initShader(ShaderType::Vertex, {kUniformsGlobals, kUniformsLocals, kUniformsWalkmesh, kVertexWalkmesh});
    auto vsBillboard = initShader(ShaderType::Vertex, {kUniformsGlobals, kUniformsLocals, kVertexBillboard});
    auto vsParticle = initShader(ShaderType::Vertex, {kUniformsGlobals, kUniformsLocals, kUniformsParticle, kVertexParticle});
    auto vsGrass = initShader(ShaderType::Vertex, {kUniformsGlobals, kUniformsLocals, kUniformsGrass, kVertexGrass});
    auto vsText = initShader(ShaderType::Vertex, {kUniformsGlobals, kUniformsLocals, kUniformsText, kVertexText});
    auto vsPoints = initShader(ShaderType::Vertex, {kUniformsGlobals, kUniformsLocals, kUniformsPoints, kVertexPoints});
    auto gsPointLightShadows = initShader(ShaderType::Geometry, {kUniformsGlobals, kUniformsLocals, kGeometryPointLightShadows});
    auto gsDirectionalLightShadows = initShader(ShaderType::Geometry, {kUniformsGlobals, kUniformsLocals, kGeometryDirLightShadows});
    auto fsColor = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kFragmentColor});
    auto fsTexture = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kFragmentTexture});
    auto fsGUI = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kFragmentGUI});
    auto fsText = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kUniformsText, kFragmentText});
    auto fsPointLightShadows = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kFragmentPointLightShadows});
    auto fsDirectionalLightShadows = initShader(ShaderType::Fragment, {kFragmentDirLightShadows});
    auto fsModelOpaque = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kIncludeMath, kIncludeHash, kIncludeHashedAlpha, kIncludeEnvMap, kIncludeNormalMap, kFragmentModelOpaque});
    auto fsModelTransparent = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kIncludeMath, kIncludeEnvMap, kIncludeNormalMap, kIncludeOIT, kIncludeLuma, kFragmentModelTransparent});
    auto fsAABB = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kFragmentAABB});
    auto fsWalkmesh = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kUniformsWalkmesh, kFragmentWalkmesh});
    auto fsBillboard = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kFragmentBillboard});
    auto fsParticle = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kUniformsParticle, kIncludeOIT, kIncludeLuma, kFragmentParticle});
    auto fsGrass = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kUniformsGrass, kIncludeHash, kIncludeHashedAlpha, kFragmentGrass});
    auto fsSSAO = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kUniformsScreenEffect, kFragmentSSAO});
    auto fsSSR = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kUniformsScreenEffect, kFragmentSSR});
    auto fsCombineOpaque = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kUniformsLighting, kIncludeMath, kIncludeBRDF, kIncludeLighting, kIncludeLuma, kIncludeShadowMap, kIncludeFog, kFragmentCombineOpaque});
    auto fsCombineGeometry = initShader(ShaderType::Fragment, {kFragmentCombineGeometry});
    auto fsBoxBlur4 = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kUniformsScreenEffect, kFragmentBoxBlur4});
    auto fsGaussianBlur9 = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kUniformsScreenEffect, kFragmentGaussianBlur9});
    auto fsGaussianBlur13 = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kUniformsScreenEffect, kFragmentGaussianBlur13});
    auto fsMedianFilter3 = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kUniformsScreenEffect, kFragmentMedianFilter3});
    auto fsMedianFilter5 = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kUniformsScreenEffect, kFragmentMedianFilter5});
    auto fsFXAA = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kUniformsScreenEffect, kIncludeLuma, kFragmentFXAA});
    auto fsSharpen = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kUniformsScreenEffect, kFragmentSharpen});

    // Shader Programs
    _shaderRegistry.add(ShaderProgramId::color2d, initShaderProgram({vsMVP2D, fsColor}));
    _shaderRegistry.add(ShaderProgramId::texture2d, initShaderProgram({vsMVP2D, fsTexture}));
    _shaderRegistry.add(ShaderProgramId::gui, initShaderProgram({vsMVP2D, fsGUI}));
    _shaderRegistry.add(ShaderProgramId::text, initShaderProgram({vsText, fsText}));
    _shaderRegistry.add(ShaderProgramId::points, initShaderProgram({vsPoints, fsColor}));
    _shaderRegistry.add(ShaderProgramId::pointLightShadows, initShaderProgram({vsShadows, gsPointLightShadows, fsPointLightShadows}));
    _shaderRegistry.add(ShaderProgramId::directionalLightShadows, initShaderProgram({vsShadows, gsDirectionalLightShadows, fsDirectionalLightShadows}));
    _shaderRegistry.add(ShaderProgramId::modelOpaque, initShaderProgram({vsModel, fsModelOpaque}));
    _shaderRegistry.add(ShaderProgramId::modelTransparent, initShaderProgram({vsModel, fsModelTransparent}));
    _shaderRegistry.add(ShaderProgramId::aabb, initShaderProgram({vsMVP3D, fsAABB}));
    _shaderRegistry.add(ShaderProgramId::walkmesh, initShaderProgram({vsWalkmesh, fsWalkmesh}));
    _shaderRegistry.add(ShaderProgramId::billboard, initShaderProgram({vsBillboard, fsBillboard}));
    _shaderRegistry.add(ShaderProgramId::particle, initShaderProgram({vsParticle, fsParticle}));
    _shaderRegistry.add(ShaderProgramId::grass, initShaderProgram({vsGrass, fsGrass}));
    _shaderRegistry.add(ShaderProgramId::ssao, initShaderProgram({vsPassthrough, fsSSAO}));
    _shaderRegistry.add(ShaderProgramId::ssr, initShaderProgram({vsPassthrough, fsSSR}));
    _shaderRegistry.add(ShaderProgramId::combineOpaque, initShaderProgram({vsPassthrough, fsCombineOpaque}));
    _shaderRegistry.add(ShaderProgramId::combineGeometry, initShaderProgram({vsPassthrough, fsCombineGeometry}));
    _shaderRegistry.add(ShaderProgramId::boxBlur4, initShaderProgram({vsPassthrough, fsBoxBlur4}));
    _shaderRegistry.add(ShaderProgramId::gaussianBlur9, initShaderProgram({vsPassthrough, fsGaussianBlur9}));
    _shaderRegistry.add(ShaderProgramId::gaussianBlur13, initShaderProgram({vsPassthrough, fsGaussianBlur13}));
    _shaderRegistry.add(ShaderProgramId::medianFilter3, initShaderProgram({vsPassthrough, fsMedianFilter3}));
    _shaderRegistry.add(ShaderProgramId::medianFilter5, initShaderProgram({vsPassthrough, fsMedianFilter5}));
    _shaderRegistry.add(ShaderProgramId::fxaa, initShaderProgram({vsPassthrough, fsFXAA}));
    _shaderRegistry.add(ShaderProgramId::sharpen, initShaderProgram({vsPassthrough, fsSharpen}));

    _inited = true;
}

void Shaders::deinit() {
    if (!_inited) {
        return;
    }
    _inited = false;
}

std::shared_ptr<Shader> Shaders::initShader(ShaderType type, std::vector<std::string> sourceResRefs) {
    std::list<std::string> sources;
    sources.push_back("#version 330 core\n\n");
    auto defines = StringBuilder();
    if (_graphicsOpt.ssr) {
        defines.append("#define R_SSR\n");
    }
    if (_graphicsOpt.ssao) {
        defines.append("#define R_SSAO\n");
    }
    if (!defines.empty()) {
        defines.append("\n");
        sources.push_back(defines.string());
    }
    for (auto &resRef : sourceResRefs) {
        std::string source;
        if (_resRefToSource.count(resRef) > 0) {
            source = _resRefToSource.at(resRef);
        } else {
            auto bytes = _sourceProvider->findResourceData(ResourceId(resRef, ResType::Glsl));
            if (!bytes) {
                throw std::runtime_error("Shader source not found: " + resRef);
            }
            source = std::string(bytes->begin(), bytes->end());
            _resRefToSource[resRef] = source;
        }
        sources.push_back(std::move(source));
    }

    auto shader = std::make_unique<Shader>(type, std::move(sources));
    shader->init();
    return shader;
}

std::shared_ptr<ShaderProgram> Shaders::initShaderProgram(std::vector<std::shared_ptr<Shader>> shaders) {
    auto program = std::make_unique<ShaderProgram>(std::move(shaders));
    program->init();
    program->use();

    // Samplers
    program->setUniform("sMainTex", TextureUnits::mainTex);
    program->setUniform("sLightmap", TextureUnits::lightmap);
    program->setUniform("sEnvironmentMap", TextureUnits::environmentMap);
    program->setUniform("sBumpMap", TextureUnits::bumpMap);
    program->setUniform("sEnvmapColor", TextureUnits::envmapColor);
    program->setUniform("sSelfIllumColor", TextureUnits::selfIllumColor);
    program->setUniform("sFeatures", TextureUnits::features);
    program->setUniform("sEyePos", TextureUnits::eyePos);
    program->setUniform("sEyeNormal", TextureUnits::eyeNormal);
    program->setUniform("sSSAO", TextureUnits::ssao);
    program->setUniform("sSSR", TextureUnits::ssr);
    program->setUniform("sHilights", TextureUnits::hilights);
    program->setUniform("sOITAccum", TextureUnits::oitAccum);
    program->setUniform("sOITRevealage", TextureUnits::oitRevealage);
    program->setUniform("sNoise", TextureUnits::noise);
    program->setUniform("sEnvironmentMapCube", TextureUnits::environmentMapCube);
    program->setUniform("sShadowMapCube", TextureUnits::shadowMapCube);
    program->setUniform("sShadowMap", TextureUnits::shadowMapArray);

    // Uniform Blocks
    program->bindUniformBlock("Globals", UniformBlockBindingPoints::globals);
    program->bindUniformBlock("Locals", UniformBlockBindingPoints::locals);
    program->bindUniformBlock("Text", UniformBlockBindingPoints::text);
    program->bindUniformBlock("Lighting", UniformBlockBindingPoints::lighting);
    program->bindUniformBlock("Skeletal", UniformBlockBindingPoints::skeletal);
    program->bindUniformBlock("Particles", UniformBlockBindingPoints::particles);
    program->bindUniformBlock("Grass", UniformBlockBindingPoints::grass);
    program->bindUniformBlock("Walkmesh", UniformBlockBindingPoints::walkmesh);
    program->bindUniformBlock("Points", UniformBlockBindingPoints::points);
    program->bindUniformBlock("ScreenEffect", UniformBlockBindingPoints::screenEffect);

    return program;
}

} // namespace resource

} // namespace reone
