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

static const std::string kResRefBRDF = "brdf";
static const std::string kResRefEnvMap = "envmap";
static const std::string kResRefFog = "fog";
static const std::string kResRefHash = "hash";
static const std::string kResRefHashedAlphaTest = "hashedalphatest";
static const std::string kResRefLighting = "lighting";
static const std::string kResRefLuma = "luma";
static const std::string kResRefMath = "math";
static const std::string kResRefNormalMap = "normalmap";
static const std::string kResRefOIT = "oit";
static const std::string kResRefShadowMap = "shadowmap";

static const std::string kResRefUniformsGlobals = "u_globals";
static const std::string kResRefUniformsLocals = "u_locals";
static const std::string kResRefUniformsGrass = "u_grass";
static const std::string kResRefUniformsLighting = "u_lighting";
static const std::string kResRefUniformsParticle = "u_particle";
static const std::string kResRefUniformsPoints = "u_points";
static const std::string kResRefUniformsSkeletal = "u_skeletal";
static const std::string kResRefUniformsSSAO = "u_ssao";
static const std::string kResRefUniformsText = "u_text";
static const std::string kResRefUniformsWalkmesh = "u_walkmesh";

static const std::string kResRefVertexBillboard = "v_billboard";
static const std::string kResRefVertexClipSpace = "v_clipspace";
static const std::string kResRefVertexGrass = "v_grass";
static const std::string kResRefVertexModel = "v_model";
static const std::string kResRefVertexObjectSpace = "v_objectspace";
static const std::string kResRefVertexParticle = "v_particle";
static const std::string kResRefVertexPoints = "v_points";
static const std::string kResRefVertexShadows = "v_shadows";
static const std::string kResRefVertexText = "v_text";
static const std::string kResRefVertexWalkmesh = "v_walkmesh";

static const std::string kResRefGeometryDirLightShadows = "g_dirlightshadow";
static const std::string kResRefGeometryPointLightShadows = "g_ptlightshadow";

static const std::string kResRefFragmentAABB = "f_aabb";
static const std::string kResRefFragmentBillboard = "f_billboard";
static const std::string kResRefFragmentBoxBlur4 = "f_boxblur4";
static const std::string kResRefFragmentColor = "f_color";
static const std::string kResRefFragmentCombineGeometry = "f_combinegeom";
static const std::string kResRefFragmentCombineOpaque = "f_combineopaque";
static const std::string kResRefFragmentDirLightShadows = "f_dirlightshadow";
static const std::string kResRefFragmentFXAA = "f_fxaa";
static const std::string kResRefFragmentGaussianBlur13 = "f_gaussianblur13";
static const std::string kResRefFragmentGaussianBlur9 = "f_gaussianblur9";
static const std::string kResRefFragmentGrass = "f_grass";
static const std::string kResRefFragmentGUI = "f_gui";
static const std::string kResRefFragmentMedianFilter3 = "f_medianfilter3";
static const std::string kResRefFragmentMedianFilter5 = "f_medianfilter5";
static const std::string kResRefFragmentModelOpaque = "f_modelopaque";
static const std::string kResRefFragmentModelTransparent = "f_modeltransp";
static const std::string kResRefFragmentParticle = "f_particle";
static const std::string kResRefFragmentPointLightShadows = "f_ptlightshadow";
static const std::string kResRefFragmentSharpen = "f_sharpen";
static const std::string kResRefFragmentSSAO = "f_ssao";
static const std::string kResRefFragmentSSR = "f_ssr";
static const std::string kResRefFragmentText = "f_text";
static const std::string kResRefFragmentTexture = "f_texture";
static const std::string kResRefFragmentWalkmesh = "f_walkmesh";

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
    auto vsObjectSpace = initShader(ShaderType::Vertex, {kResRefVertexObjectSpace});
    auto vsClipSpace = initShader(ShaderType::Vertex, {kResRefUniformsGlobals, kResRefUniformsLocals, kResRefVertexClipSpace});
    auto vsShadows = initShader(ShaderType::Vertex, {kResRefUniformsGlobals, kResRefUniformsLocals, kResRefVertexShadows});
    auto vsModel = initShader(ShaderType::Vertex, {kResRefUniformsGlobals, kResRefUniformsLocals, kResRefUniformsSkeletal, kResRefVertexModel});
    auto vsWalkmesh = initShader(ShaderType::Vertex, {kResRefUniformsGlobals, kResRefUniformsLocals, kResRefUniformsWalkmesh, kResRefVertexWalkmesh});
    auto vsBillboard = initShader(ShaderType::Vertex, {kResRefUniformsGlobals, kResRefUniformsLocals, kResRefVertexBillboard});
    auto vsParticle = initShader(ShaderType::Vertex, {kResRefUniformsGlobals, kResRefUniformsLocals, kResRefUniformsParticle, kResRefVertexParticle});
    auto vsGrass = initShader(ShaderType::Vertex, {kResRefUniformsGlobals, kResRefUniformsLocals, kResRefUniformsGrass, kResRefVertexGrass});
    auto vsText = initShader(ShaderType::Vertex, {kResRefUniformsGlobals, kResRefUniformsLocals, kResRefUniformsText, kResRefVertexText});
    auto vsPoints = initShader(ShaderType::Vertex, {kResRefUniformsGlobals, kResRefUniformsLocals, kResRefUniformsPoints, kResRefVertexPoints});
    auto gsPointLightShadows = initShader(ShaderType::Geometry, {kResRefUniformsGlobals, kResRefUniformsLocals, kResRefGeometryPointLightShadows});
    auto gsDirectionalLightShadows = initShader(ShaderType::Geometry, {kResRefUniformsGlobals, kResRefUniformsLocals, kResRefGeometryDirLightShadows});
    auto fsColor = initShader(ShaderType::Fragment, {kResRefUniformsGlobals, kResRefUniformsLocals, kResRefFragmentColor});
    auto fsTexture = initShader(ShaderType::Fragment, {kResRefUniformsGlobals, kResRefUniformsLocals, kResRefFragmentTexture});
    auto fsGUI = initShader(ShaderType::Fragment, {kResRefUniformsGlobals, kResRefUniformsLocals, kResRefFragmentGUI});
    auto fsText = initShader(ShaderType::Fragment, {kResRefUniformsGlobals, kResRefUniformsLocals, kResRefUniformsText, kResRefFragmentText});
    auto fsPointLightShadows = initShader(ShaderType::Fragment, {kResRefUniformsGlobals, kResRefUniformsLocals, kResRefFragmentPointLightShadows});
    auto fsDirectionalLightShadows = initShader(ShaderType::Fragment, {kResRefFragmentDirLightShadows});
    auto fsModelOpaque = initShader(ShaderType::Fragment, {kResRefUniformsGlobals, kResRefUniformsLocals, kResRefMath, kResRefHash, kResRefHashedAlphaTest, kResRefEnvMap, kResRefNormalMap, kResRefFragmentModelOpaque});
    auto fsModelTransparent = initShader(ShaderType::Fragment, {kResRefUniformsGlobals, kResRefUniformsLocals, kResRefMath, kResRefEnvMap, kResRefNormalMap, kResRefOIT, kResRefLuma, kResRefFragmentModelTransparent});
    auto fsAABB = initShader(ShaderType::Fragment, {kResRefUniformsGlobals, kResRefUniformsLocals, kResRefFragmentAABB});
    auto fsWalkmesh = initShader(ShaderType::Fragment, {kResRefUniformsGlobals, kResRefUniformsLocals, kResRefUniformsWalkmesh, kResRefFragmentWalkmesh});
    auto fsBillboard = initShader(ShaderType::Fragment, {kResRefUniformsGlobals, kResRefUniformsLocals, kResRefFragmentBillboard});
    auto fsParticle = initShader(ShaderType::Fragment, {kResRefUniformsGlobals, kResRefUniformsLocals, kResRefUniformsParticle, kResRefOIT, kResRefLuma, kResRefFragmentParticle});
    auto fsGrass = initShader(ShaderType::Fragment, {kResRefUniformsGlobals, kResRefUniformsLocals, kResRefUniformsGrass, kResRefHash, kResRefHashedAlphaTest, kResRefFragmentGrass});
    auto fsSSAO = initShader(ShaderType::Fragment, {kResRefUniformsGlobals, kResRefUniformsLocals, kResRefUniformsSSAO, kResRefFragmentSSAO});
    auto fsSSR = initShader(ShaderType::Fragment, {kResRefUniformsGlobals, kResRefUniformsLocals, kResRefFragmentSSR});
    auto fsCombineOpaque = initShader(ShaderType::Fragment, {kResRefUniformsGlobals, kResRefUniformsLocals, kResRefUniformsLighting, kResRefMath, kResRefBRDF, kResRefLighting, kResRefLuma, kResRefShadowMap, kResRefFog, kResRefFragmentCombineOpaque});
    auto fsCombineGeometry = initShader(ShaderType::Fragment, {kResRefFragmentCombineGeometry});
    auto fsBoxBlur4 = initShader(ShaderType::Fragment, {kResRefUniformsGlobals, kResRefUniformsLocals, kResRefFragmentBoxBlur4});
    auto fsGaussianBlur9 = initShader(ShaderType::Fragment, {kResRefUniformsGlobals, kResRefUniformsLocals, kResRefFragmentGaussianBlur9});
    auto fsGaussianBlur13 = initShader(ShaderType::Fragment, {kResRefUniformsGlobals, kResRefUniformsLocals, kResRefFragmentGaussianBlur13});
    auto fsMedianFilter3 = initShader(ShaderType::Fragment, {kResRefUniformsGlobals, kResRefUniformsLocals, kResRefFragmentMedianFilter3});
    auto fsMedianFilter5 = initShader(ShaderType::Fragment, {kResRefUniformsGlobals, kResRefUniformsLocals, kResRefFragmentMedianFilter5});
    auto fsFXAA = initShader(ShaderType::Fragment, {kResRefUniformsGlobals, kResRefUniformsLocals, kResRefLuma, kResRefFragmentFXAA});
    auto fsSharpen = initShader(ShaderType::Fragment, {kResRefUniformsGlobals, kResRefUniformsLocals, kResRefFragmentSharpen});

    // Shader Programs
    _shaderRegistry.add(ShaderProgramId::simpleColor, initShaderProgram({vsClipSpace, fsColor}));
    _shaderRegistry.add(ShaderProgramId::simpleTexture, initShaderProgram({vsClipSpace, fsTexture}));
    _shaderRegistry.add(ShaderProgramId::gui, initShaderProgram({vsClipSpace, fsGUI}));
    _shaderRegistry.add(ShaderProgramId::text, initShaderProgram({vsText, fsText}));
    _shaderRegistry.add(ShaderProgramId::points, initShaderProgram({vsPoints, fsColor}));
    _shaderRegistry.add(ShaderProgramId::pointLightShadows, initShaderProgram({vsShadows, gsPointLightShadows, fsPointLightShadows}));
    _shaderRegistry.add(ShaderProgramId::directionalLightShadows, initShaderProgram({vsShadows, gsDirectionalLightShadows, fsDirectionalLightShadows}));
    _shaderRegistry.add(ShaderProgramId::modelOpaque, initShaderProgram({vsModel, fsModelOpaque}));
    _shaderRegistry.add(ShaderProgramId::modelTransparent, initShaderProgram({vsModel, fsModelTransparent}));
    _shaderRegistry.add(ShaderProgramId::aabb, initShaderProgram({vsClipSpace, fsAABB}));
    _shaderRegistry.add(ShaderProgramId::walkmesh, initShaderProgram({vsWalkmesh, fsWalkmesh}));
    _shaderRegistry.add(ShaderProgramId::billboard, initShaderProgram({vsBillboard, fsBillboard}));
    _shaderRegistry.add(ShaderProgramId::particle, initShaderProgram({vsParticle, fsParticle}));
    _shaderRegistry.add(ShaderProgramId::grass, initShaderProgram({vsGrass, fsGrass}));
    _shaderRegistry.add(ShaderProgramId::ssao, initShaderProgram({vsObjectSpace, fsSSAO}));
    _shaderRegistry.add(ShaderProgramId::ssr, initShaderProgram({vsObjectSpace, fsSSR}));
    _shaderRegistry.add(ShaderProgramId::combineOpaque, initShaderProgram({vsObjectSpace, fsCombineOpaque}));
    _shaderRegistry.add(ShaderProgramId::combineGeometry, initShaderProgram({vsObjectSpace, fsCombineGeometry}));
    _shaderRegistry.add(ShaderProgramId::boxBlur4, initShaderProgram({vsObjectSpace, fsBoxBlur4}));
    _shaderRegistry.add(ShaderProgramId::gaussianBlur9, initShaderProgram({vsObjectSpace, fsGaussianBlur9}));
    _shaderRegistry.add(ShaderProgramId::gaussianBlur13, initShaderProgram({vsObjectSpace, fsGaussianBlur13}));
    _shaderRegistry.add(ShaderProgramId::medianFilter3, initShaderProgram({vsObjectSpace, fsMedianFilter3}));
    _shaderRegistry.add(ShaderProgramId::medianFilter5, initShaderProgram({vsObjectSpace, fsMedianFilter5}));
    _shaderRegistry.add(ShaderProgramId::fxaa, initShaderProgram({vsObjectSpace, fsFXAA}));
    _shaderRegistry.add(ShaderProgramId::sharpen, initShaderProgram({vsObjectSpace, fsSharpen}));

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
    program->bindUniformBlock("SSAO", UniformBlockBindingPoints::ssao);
    program->bindUniformBlock("Walkmesh", UniformBlockBindingPoints::walkmesh);
    program->bindUniformBlock("Points", UniformBlockBindingPoints::points);

    return program;
}

} // namespace resource

} // namespace reone
