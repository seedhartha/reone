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

#include "reone/graphics/shaders.h"

#include "reone/graphics/options.h"
#include "reone/system/stringbuilder.h"

using namespace reone::resource;

namespace reone {

namespace graphics {

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

static const std::string kResRefUniformsGeneral = "u_general";
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
    _sourceProvider = std::make_unique<ErfResourceProvider>(shaderpackPath);
    _sourceProvider->init();

    // Shaders
    auto vsObjectSpace = initShader(ShaderType::Vertex, {kResRefVertexObjectSpace});
    auto vsClipSpace = initShader(ShaderType::Vertex, {kResRefUniformsGeneral, kResRefVertexClipSpace});
    auto vsShadows = initShader(ShaderType::Vertex, {kResRefUniformsGeneral, kResRefVertexShadows});
    auto vsModel = initShader(ShaderType::Vertex, {kResRefUniformsGeneral, kResRefUniformsSkeletal, kResRefVertexModel});
    auto vsWalkmesh = initShader(ShaderType::Vertex, {kResRefUniformsGeneral, kResRefUniformsWalkmesh, kResRefVertexWalkmesh});
    auto vsBillboard = initShader(ShaderType::Vertex, {kResRefUniformsGeneral, kResRefVertexBillboard});
    auto vsParticle = initShader(ShaderType::Vertex, {kResRefUniformsGeneral, kResRefUniformsParticle, kResRefVertexParticle});
    auto vsGrass = initShader(ShaderType::Vertex, {kResRefUniformsGeneral, kResRefUniformsGrass, kResRefVertexGrass});
    auto vsText = initShader(ShaderType::Vertex, {kResRefUniformsGeneral, kResRefUniformsText, kResRefVertexText});
    auto vsPoints = initShader(ShaderType::Vertex, {kResRefUniformsGeneral, kResRefUniformsPoints, kResRefVertexPoints});
    auto gsPointLightShadows = initShader(ShaderType::Geometry, {kResRefUniformsGeneral, kResRefGeometryPointLightShadows});
    auto gsDirectionalLightShadows = initShader(ShaderType::Geometry, {kResRefUniformsGeneral, kResRefGeometryDirLightShadows});
    auto fsColor = initShader(ShaderType::Fragment, {kResRefUniformsGeneral, kResRefFragmentColor});
    auto fsTexture = initShader(ShaderType::Fragment, {kResRefUniformsGeneral, kResRefFragmentTexture});
    auto fsGUI = initShader(ShaderType::Fragment, {kResRefUniformsGeneral, kResRefFragmentGUI});
    auto fsText = initShader(ShaderType::Fragment, {kResRefUniformsGeneral, kResRefUniformsText, kResRefFragmentText});
    auto fsPointLightShadows = initShader(ShaderType::Fragment, {kResRefUniformsGeneral, kResRefFragmentPointLightShadows});
    auto fsDirectionalLightShadows = initShader(ShaderType::Fragment, {kResRefFragmentDirLightShadows});
    auto fsModelOpaque = initShader(ShaderType::Fragment, {kResRefUniformsGeneral, kResRefMath, kResRefHash, kResRefHashedAlphaTest, kResRefEnvMap, kResRefNormalMap, kResRefFragmentModelOpaque});
    auto fsModelTransparent = initShader(ShaderType::Fragment, {kResRefUniformsGeneral, kResRefMath, kResRefEnvMap, kResRefNormalMap, kResRefOIT, kResRefLuma, kResRefFragmentModelTransparent});
    auto fsAABB = initShader(ShaderType::Fragment, {kResRefUniformsGeneral, kResRefFragmentAABB});
    auto fsWalkmesh = initShader(ShaderType::Fragment, {kResRefUniformsGeneral, kResRefUniformsWalkmesh, kResRefFragmentWalkmesh});
    auto fsBillboard = initShader(ShaderType::Fragment, {kResRefUniformsGeneral, kResRefFragmentBillboard});
    auto fsParticle = initShader(ShaderType::Fragment, {kResRefUniformsGeneral, kResRefUniformsParticle, kResRefOIT, kResRefLuma, kResRefFragmentParticle});
    auto fsGrass = initShader(ShaderType::Fragment, {kResRefUniformsGeneral, kResRefUniformsGrass, kResRefHash, kResRefHashedAlphaTest, kResRefFragmentGrass});
    auto fsSSAO = initShader(ShaderType::Fragment, {kResRefUniformsGeneral, kResRefUniformsSSAO, kResRefFragmentSSAO});
    auto fsSSR = initShader(ShaderType::Fragment, {kResRefUniformsGeneral, kResRefFragmentSSR});
    auto fsCombineOpaque = initShader(ShaderType::Fragment, {kResRefUniformsGeneral, kResRefUniformsLighting, kResRefMath, kResRefBRDF, kResRefLighting, kResRefLuma, kResRefShadowMap, kResRefFog, kResRefFragmentCombineOpaque});
    auto fsCombineGeometry = initShader(ShaderType::Fragment, {kResRefFragmentCombineGeometry});
    auto fsBoxBlur4 = initShader(ShaderType::Fragment, {kResRefUniformsGeneral, kResRefFragmentBoxBlur4});
    auto fsGaussianBlur9 = initShader(ShaderType::Fragment, {kResRefUniformsGeneral, kResRefFragmentGaussianBlur9});
    auto fsGaussianBlur13 = initShader(ShaderType::Fragment, {kResRefUniformsGeneral, kResRefFragmentGaussianBlur13});
    auto fsMedianFilter3 = initShader(ShaderType::Fragment, {kResRefUniformsGeneral, kResRefFragmentMedianFilter3});
    auto fsMedianFilter5 = initShader(ShaderType::Fragment, {kResRefUniformsGeneral, kResRefFragmentMedianFilter5});
    auto fsFXAA = initShader(ShaderType::Fragment, {kResRefUniformsGeneral, kResRefLuma, kResRefFragmentFXAA});
    auto fsSharpen = initShader(ShaderType::Fragment, {kResRefUniformsGeneral, kResRefFragmentSharpen});

    // Shader Programs
    _spSimpleColor = initShaderProgram({vsClipSpace, fsColor});
    _spSimpleTexture = initShaderProgram({vsClipSpace, fsTexture});
    _spGUI = initShaderProgram({vsClipSpace, fsGUI});
    _spText = initShaderProgram({vsText, fsText});
    _spPoints = initShaderProgram({vsPoints, fsColor});
    _spPointLightShadows = initShaderProgram({vsShadows, gsPointLightShadows, fsPointLightShadows});
    _spDirectionalLightShadows = initShaderProgram({vsShadows, gsDirectionalLightShadows, fsDirectionalLightShadows});
    _spModelOpaque = initShaderProgram({vsModel, fsModelOpaque});
    _spModelTransparent = initShaderProgram({vsModel, fsModelTransparent});
    _spAABB = initShaderProgram({vsClipSpace, fsAABB});
    _spWalkmesh = initShaderProgram({vsWalkmesh, fsWalkmesh});
    _spBillboard = initShaderProgram({vsBillboard, fsBillboard});
    _spParticle = initShaderProgram({vsParticle, fsParticle});
    _spGrass = initShaderProgram({vsGrass, fsGrass});
    _spSSAO = initShaderProgram({vsObjectSpace, fsSSAO});
    _spSSR = initShaderProgram({vsObjectSpace, fsSSR});
    _spCombineOpaque = initShaderProgram({vsObjectSpace, fsCombineOpaque});
    _spCombineGeometry = initShaderProgram({vsObjectSpace, fsCombineGeometry});
    _spBoxBlur4 = initShaderProgram({vsObjectSpace, fsBoxBlur4});
    _spGaussianBlur9 = initShaderProgram({vsObjectSpace, fsGaussianBlur9});
    _spGaussianBlur13 = initShaderProgram({vsObjectSpace, fsGaussianBlur13});
    _spMedianFilter3 = initShaderProgram({vsObjectSpace, fsMedianFilter3});
    _spMedianFilter5 = initShaderProgram({vsObjectSpace, fsMedianFilter5});
    _spFXAA = initShaderProgram({vsObjectSpace, fsFXAA});
    _spSharpen = initShaderProgram({vsObjectSpace, fsSharpen});

    _inited = true;
}

void Shaders::deinit() {
    if (!_inited) {
        return;
    }

    _spSimpleColor.reset();
    _spSimpleTexture.reset();
    _spGUI.reset();
    _spText.reset();
    _spPoints.reset();
    _spPointLightShadows.reset();
    _spDirectionalLightShadows.reset();
    _spModelOpaque.reset();
    _spModelTransparent.reset();
    _spAABB.reset();
    _spWalkmesh.reset();
    _spBillboard.reset();
    _spParticle.reset();
    _spGrass.reset();
    _spSSAO.reset();
    _spSSR.reset();
    _spCombineOpaque.reset();
    _spCombineGeometry.reset();
    _spBoxBlur4.reset();
    _spGaussianBlur9.reset();
    _spGaussianBlur13.reset();
    _spMedianFilter3.reset();
    _spMedianFilter5.reset();
    _spFXAA.reset();
    _spSharpen.reset();

    _inited = false;
}

void Shaders::use(ShaderProgramId programId) {
    if (_usedProgram == programId) {
        return;
    }
    if (programId != ShaderProgramId::None) {
        getProgram(programId).use();
    }
    _usedProgram = programId;
}

std::shared_ptr<Shader> Shaders::initShader(ShaderType type, std::vector<std::string> sourceResRefs) {
    std::list<std::string> sources;
    sources.push_back("#version 330 core\n\n");
    auto defines = StringBuilder();
    if (_options.ssr) {
        defines.append("#define R_SSR\n");
    }
    if (_options.ssao) {
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
    program->bindUniformBlock("General", UniformBlockBindingPoints::general);
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

ShaderProgram &Shaders::getProgram(ShaderProgramId id) {
    switch (id) {
    case ShaderProgramId::SimpleColor:
        return *_spSimpleColor;
    case ShaderProgramId::SimpleTexture:
        return *_spSimpleTexture;
    case ShaderProgramId::GUI:
        return *_spGUI;
    case ShaderProgramId::Text:
        return *_spText;
    case ShaderProgramId::Points:
        return *_spPoints;
    case ShaderProgramId::PointLightShadows:
        return *_spPointLightShadows;
    case ShaderProgramId::DirectionalLightShadows:
        return *_spDirectionalLightShadows;
    case ShaderProgramId::ModelOpaque:
        return *_spModelOpaque;
    case ShaderProgramId::ModelTransparent:
        return *_spModelTransparent;
    case ShaderProgramId::AABB:
        return *_spAABB;
    case ShaderProgramId::Walkmesh:
        return *_spWalkmesh;
    case ShaderProgramId::Particle:
        return *_spParticle;
    case ShaderProgramId::Grass:
        return *_spGrass;
    case ShaderProgramId::Billboard:
        return *_spBillboard;
    case ShaderProgramId::SSAO:
        return *_spSSAO;
    case ShaderProgramId::SSR:
        return *_spSSR;
    case ShaderProgramId::CombineOpaque:
        return *_spCombineOpaque;
    case ShaderProgramId::CombineGeometry:
        return *_spCombineGeometry;
    case ShaderProgramId::BoxBlur4:
        return *_spBoxBlur4;
    case ShaderProgramId::GaussianBlur9:
        return *_spGaussianBlur9;
    case ShaderProgramId::GaussianBlur13:
        return *_spGaussianBlur13;
    case ShaderProgramId::MedianFilter3:
        return *_spMedianFilter3;
    case ShaderProgramId::MedianFilter5:
        return *_spMedianFilter5;
    case ShaderProgramId::FXAA:
        return *_spFXAA;
    case ShaderProgramId::Sharpen:
        return *_spSharpen;
    default:
        throw std::invalid_argument("Invalid shader program id: " + std::to_string(static_cast<int>(id)));
    }
}

} // namespace graphics

} // namespace reone
