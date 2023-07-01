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
#include "reone/system/stream/fileinput.h"
#include "reone/system/stringbuilder.h"

namespace reone {

namespace graphics {

static const std::string kGlslHeader = "common/header.glsl";
static const std::string kGlslGeneralUniforms = "common/generaluniforms.glsl";
static const std::string kGlslSkeletalUniforms = "common/skeletaluniforms.glsl";
static const std::string kGlslWalkmeshUniforms = "common/walkmeshuniforms.glsl";
static const std::string kGlslParticleUniforms = "common/particleuniforms.glsl";
static const std::string kGlslGrassUniforms = "common/grassuniforms.glsl";
static const std::string kGlslTextUniforms = "common/textuniforms.glsl";
static const std::string kGlslPointsUniforms = "common/pointsuniforms.glsl";
static const std::string kGlslSSAOUniforms = "common/ssaouniforms.glsl";
static const std::string kGlslLightingUniforms = "common/lightinguniforms.glsl";
static const std::string kGlslMath = "common/math.glsl";
static const std::string kGlslHash = "common/hash.glsl";
static const std::string kGlslHashedAlphaTest = "common/hashedalphatest.glsl";
static const std::string kGlslEnvironmentMapping = "common/environmentmapping.glsl";
static const std::string kGlslNormalMapping = "common/normalmapping.glsl";
static const std::string kGlslOIT = "common/oit.glsl";
static const std::string kGlslLuma = "common/luma.glsl";
static const std::string kGlslBRDF = "common/brdf.glsl";
static const std::string kGlslLighting = "common/lighting.glsl";
static const std::string kGlslShadowMapping = "common/shadowmapping.glsl";
static const std::string kGlslFog = "common/fog.glsl";

static const std::string kVsObjectSpace = "vertex/objectspace.glsl";
static const std::string kVsClipSpace = "vertex/clipspace.glsl";
static const std::string kVsShadows = "vertex/shadows.glsl";
static const std::string kVsModel = "vertex/model.glsl";
static const std::string kVsWalkmesh = "vertex/walkmesh.glsl";
static const std::string kVsBillboard = "vertex/billboard.glsl";
static const std::string kVsParticle = "vertex/particle.glsl";
static const std::string kVsGrass = "vertex/grass.glsl";
static const std::string kVsText = "vertex/text.glsl";
static const std::string kVsPoints = "vertex/points.glsl";

static const std::string kGsPointLightShadows = "geometry/pointlightshadows.glsl";
static const std::string kGsDirectionalLightShadows = "geometry/directionallightshadows.glsl";

static const std::string kFsColor = "fragment/color.glsl";
static const std::string kFsTexture = "fragment/texture.glsl";
static const std::string kFsGUI = "fragment/gui.glsl";
static const std::string kFsText = "fragment/text.glsl";
static const std::string kFsPointLightShadows = "fragment/pointlightshadows.glsl";
static const std::string kFsDirectionalLightShadows = "fragment/directionallightshadows.glsl";
static const std::string kFsModelOpaque = "fragment/modelopaque.glsl";
static const std::string kFsModelTransparent = "fragment/modeltransparent.glsl";
static const std::string kFsAABB = "fragment/aabb.glsl";
static const std::string kFsWalkmesh = "fragment/walkmesh.glsl";
static const std::string kFsBillboard = "fragment/billboard.glsl";
static const std::string kFsParticle = "fragment/particle.glsl";
static const std::string kFsGrass = "fragment/grass.glsl";
static const std::string kFsSSAO = "fragment/ssao.glsl";
static const std::string kFsSSR = "fragment/ssr.glsl";
static const std::string kFsCombineOpaque = "fragment/combineopaque.glsl";
static const std::string kFsCombineGeometry = "fragment/combinegeometry.glsl";
static const std::string kFsBoxBlur4 = "fragment/boxblur4.glsl";
static const std::string kFsGaussianBlur9 = "fragment/gaussianblur9.glsl";
static const std::string kFsGaussianBlur13 = "fragment/gaussianblur13.glsl";
static const std::string kFsMedianFilter3 = "fragment/medianfilter3.glsl";
static const std::string kFsMedianFilter5 = "fragment/medianfilter5.glsl";
static const std::string kFsFXAA = "fragment/fxaa.glsl";
static const std::string kFsSharpen = "fragment/sharpen.glsl";

void Shaders::init() {
    if (_inited) {
        return;
    }

    // Shaders
    auto vsObjectSpace = initShader(ShaderType::Vertex, {kVsObjectSpace});
    auto vsClipSpace = initShader(ShaderType::Vertex, {kGlslGeneralUniforms, kVsClipSpace});
    auto vsShadows = initShader(ShaderType::Vertex, {kGlslGeneralUniforms, kVsShadows});
    auto vsModel = initShader(ShaderType::Vertex, {kGlslGeneralUniforms, kGlslSkeletalUniforms, kVsModel});
    auto vsWalkmesh = initShader(ShaderType::Vertex, {kGlslGeneralUniforms, kGlslWalkmeshUniforms, kVsWalkmesh});
    auto vsBillboard = initShader(ShaderType::Vertex, {kGlslGeneralUniforms, kVsBillboard});
    auto vsParticle = initShader(ShaderType::Vertex, {kGlslGeneralUniforms, kGlslParticleUniforms, kVsParticle});
    auto vsGrass = initShader(ShaderType::Vertex, {kGlslGeneralUniforms, kGlslGrassUniforms, kVsGrass});
    auto vsText = initShader(ShaderType::Vertex, {kGlslGeneralUniforms, kGlslTextUniforms, kVsText});
    auto vsPoints = initShader(ShaderType::Vertex, {kGlslGeneralUniforms, kGlslPointsUniforms, kVsPoints});
    auto gsPointLightShadows = initShader(ShaderType::Geometry, {kGlslGeneralUniforms, kGsPointLightShadows});
    auto gsDirectionalLightShadows = initShader(ShaderType::Geometry, {kGlslGeneralUniforms, kGsDirectionalLightShadows});
    auto fsColor = initShader(ShaderType::Fragment, {kGlslGeneralUniforms, kFsColor});
    auto fsTexture = initShader(ShaderType::Fragment, {kGlslGeneralUniforms, kFsTexture});
    auto fsGUI = initShader(ShaderType::Fragment, {kGlslGeneralUniforms, kFsGUI});
    auto fsText = initShader(ShaderType::Fragment, {kGlslGeneralUniforms, kGlslTextUniforms, kFsText});
    auto fsPointLightShadows = initShader(ShaderType::Fragment, {kGlslGeneralUniforms, kFsPointLightShadows});
    auto fsDirectionalLightShadows = initShader(ShaderType::Fragment, {kFsDirectionalLightShadows});
    auto fsModelOpaque = initShader(ShaderType::Fragment, {kGlslGeneralUniforms, kGlslMath, kGlslHash, kGlslHashedAlphaTest, kGlslEnvironmentMapping, kGlslNormalMapping, kFsModelOpaque});
    auto fsModelTransparent = initShader(ShaderType::Fragment, {kGlslGeneralUniforms, kGlslMath, kGlslEnvironmentMapping, kGlslNormalMapping, kGlslOIT, kGlslLuma, kFsModelTransparent});
    auto fsAABB = initShader(ShaderType::Fragment, {kGlslGeneralUniforms, kFsAABB});
    auto fsWalkmesh = initShader(ShaderType::Fragment, {kGlslGeneralUniforms, kGlslWalkmeshUniforms, kFsWalkmesh});
    auto fsBillboard = initShader(ShaderType::Fragment, {kGlslGeneralUniforms, kFsBillboard});
    auto fsParticle = initShader(ShaderType::Fragment, {kGlslGeneralUniforms, kGlslParticleUniforms, kGlslOIT, kGlslLuma, kFsParticle});
    auto fsGrass = initShader(ShaderType::Fragment, {kGlslGeneralUniforms, kGlslGrassUniforms, kGlslHash, kGlslHashedAlphaTest, kFsGrass});
    auto fsSSAO = initShader(ShaderType::Fragment, {kGlslGeneralUniforms, kGlslSSAOUniforms, kFsSSAO});
    auto fsSSR = initShader(ShaderType::Fragment, {kGlslGeneralUniforms, kFsSSR});
    auto fsCombineOpaque = initShader(ShaderType::Fragment, {kGlslGeneralUniforms, kGlslLightingUniforms, kGlslMath, kGlslBRDF, kGlslLighting, kGlslLuma, kGlslShadowMapping, kGlslFog, kFsCombineOpaque});
    auto fsCombineGeometry = initShader(ShaderType::Fragment, {kFsCombineGeometry});
    auto fsBoxBlur4 = initShader(ShaderType::Fragment, {kGlslGeneralUniforms, kFsBoxBlur4});
    auto fsGaussianBlur9 = initShader(ShaderType::Fragment, {kGlslGeneralUniforms, kFsGaussianBlur9});
    auto fsGaussianBlur13 = initShader(ShaderType::Fragment, {kGlslGeneralUniforms, kFsGaussianBlur13});
    auto fsMedianFilter3 = initShader(ShaderType::Fragment, {kGlslGeneralUniforms, kFsMedianFilter3});
    auto fsMedianFilter5 = initShader(ShaderType::Fragment, {kGlslGeneralUniforms, kFsMedianFilter5});
    auto fsFXAA = initShader(ShaderType::Fragment, {kGlslGeneralUniforms, kGlslLuma, kFsFXAA});
    auto fsSharpen = initShader(ShaderType::Fragment, {kGlslGeneralUniforms, kFsSharpen});

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

std::shared_ptr<Shader> Shaders::initShader(ShaderType type, std::vector<std::string> sourceFiles) {
    std::list<std::string> sources;
    sources.push_back("#version 330 core");
    auto defines = StringBuilder();
    defines.append("\n");
    if (_options.ssr) {
        defines.append("#define R_SSR\n");
    }
    if (_options.ssao) {
        defines.append("#define R_SSAO\n");
    }
    defines.append("\n");
    sources.push_back(defines.build());
    for (auto &file : sourceFiles) {
        std::string source;
        if (_sourceFiles.count(file) > 0) {
            source = _sourceFiles.at(file);
        } else {
            auto path = boost::filesystem::current_path();
            path.append("glsl");
            path.append(file);
            if (!boost::filesystem::exists(path)) {
                throw std::runtime_error("GLSL file not found: " + path.string());
            }
            auto stream = FileInputStream(path);
            stream.seek(0, SeekOrigin::End);
            auto filesize = stream.position();
            stream.seek(0, SeekOrigin::Begin);
            source.resize(filesize);
            stream.read(&source[0], filesize);
            _sourceFiles[file] = source;
        }
        sources.push_back(std::move(source));
    }

    auto shader = std::make_unique<Shader>(type, std::move(sources));
    shader->init();
    return std::move(shader);
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

    return std::move(program);
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
