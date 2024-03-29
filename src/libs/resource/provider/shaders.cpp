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
#include "reone/graphics/uniforms.h"
#include "reone/resource/resources.h"
#include "reone/system/logutil.h"
#include "reone/system/stream/memoryinput.h"
#include "reone/system/stringbuilder.h"
#include "reone/system/textreader.h"

using namespace reone::graphics;

namespace reone {

namespace resource {

static const std::string kVertBillboard = "v_billboard";
static const std::string kVertGrass = "v_grass";
static const std::string kVertModel = "v_model";
static const std::string kVertMVP = "v_mvp";
static const std::string kVertMVPNormal = "v_mvpnormal";
static const std::string kVertAABB = "v_aabb";
static const std::string kVertParticles = "v_particles";
static const std::string kVertPassthrough = "v_passthrough";
static const std::string kVertShadows = "v_shadows";
static const std::string kVertText = "v_text";
static const std::string kVertWalkmesh = "v_walkmesh";

static const std::string kGeometryDirLightShadows = "g_dirlightshadow";
static const std::string kGeometryPointLightShadows = "g_ptlightshadow";

static const std::string kFragColor = "f_color";
static const std::string kFragRetroOpaqueModel = "f_rtr_opaqmodel";
static const std::string kFragRetroGrass = "f_rtr_grass";
static const std::string kFragRetroAABB = "f_rtr_aabb";
static const std::string kFragRetroWalkmesh = "f_rtr_walkmesh";
static const std::string kFragRetroCombine = "f_rtr_combine";
static const std::string kFragPBRAABB = "f_pbr_aabb";
static const std::string kFragPBRCombine = "f_pbr_combine";
static const std::string kFragPBRGrass = "f_pbr_grass";
static const std::string kFragPBROpaqueModel = "f_pbr_opaqmodel";
static const std::string kFragPBRSSAO = "f_pbr_ssao";
static const std::string kFragPBRSSR = "f_pbr_ssr";
static const std::string kFragPBRWalkmesh = "f_pbr_walkmesh";
static const std::string kFragNull = "f_null";
static const std::string kFragOITBlend = "f_oit_blend";
static const std::string kFragOITModel = "f_oit_model";
static const std::string kFragOITParticles = "f_oit_particles";
static const std::string kFragPointLightShadows = "f_ptlightshadow";
static const std::string kFragPostBoxBlur4 = "f_pp_boxblur4";
static const std::string kFragPostFXAA = "f_pp_fxaa";
static const std::string kFragPostGaussianBlur13 = "f_pp_gausblur13";
static const std::string kFragPostGaussianBlur9 = "f_pp_gausblur9";
static const std::string kFragPostMedianFilter3 = "f_pp_medianfilt3";
static const std::string kFragPostMedianFilter5 = "f_pp_medianfilt5";
static const std::string kFragPostSharpen = "f_pp_sharpen";
static const std::string kFragText = "f_text";
static const std::string kFragTexture = "f_texture";
static const std::string kFragTextureNoPerspective = "f_texnoper";
static const std::string kFragPBRIrradiance = "f_pbr_irradiance";
static const std::string kFragPBRBRDF = "f_pbr_brdf";
static const std::string kFragPBRPrefilter = "f_pbr_prefilter";
static const std::string kFragProfiler = "f_profiler";

void Shaders::init() {
    if (_inited) {
        return;
    }

    // Shaders
    auto vertBillboard = initShader(ShaderType::Vertex, kVertBillboard);
    auto vertGrass = initShader(ShaderType::Vertex, kVertGrass);
    auto vertModel = initShader(ShaderType::Vertex, kVertModel);
    auto vertMVP = initShader(ShaderType::Vertex, kVertMVP);
    auto vertMVPNormal = initShader(ShaderType::Vertex, kVertMVPNormal);
    auto vertAABB = initShader(ShaderType::Vertex, kVertAABB);
    auto vertParticles = initShader(ShaderType::Vertex, kVertParticles);
    auto vertPassthrough = initShader(ShaderType::Vertex, kVertPassthrough);
    auto vertShadows = initShader(ShaderType::Vertex, kVertShadows);
    auto vertText = initShader(ShaderType::Vertex, kVertText);
    auto vertWalkmesh = initShader(ShaderType::Vertex, kVertWalkmesh);

    auto geomDirLightShadows = initShader(ShaderType::Geometry, kGeometryDirLightShadows);
    auto geomPointLightShadows = initShader(ShaderType::Geometry, kGeometryPointLightShadows);

    auto fragColor = initShader(ShaderType::Fragment, kFragColor);
    auto fragRetroOpaqueModel = initShader(ShaderType::Fragment, kFragRetroOpaqueModel);
    auto fragRetroGrass = initShader(ShaderType::Fragment, kFragRetroGrass);
    auto fragRetroAABB = initShader(ShaderType::Fragment, kFragRetroAABB);
    auto fragRetroWalkmesh = initShader(ShaderType::Fragment, kFragRetroWalkmesh);
    auto fragPBRAABB = initShader(ShaderType::Fragment, kFragPBRAABB);
    auto fragPBRCombine = initShader(ShaderType::Fragment, kFragPBRCombine);
    auto fragPBRGrass = initShader(ShaderType::Fragment, kFragPBRGrass);
    auto fragPBROpaqueModel = initShader(ShaderType::Fragment, kFragPBROpaqueModel);
    auto fragPBRSSAO = initShader(ShaderType::Fragment, kFragPBRSSAO);
    auto fragPBRSSR = initShader(ShaderType::Fragment, kFragPBRSSR);
    auto fragPBRWalkmesh = initShader(ShaderType::Fragment, kFragPBRWalkmesh);
    auto fragNull = initShader(ShaderType::Fragment, kFragNull);
    auto fragOITBlend = initShader(ShaderType::Fragment, kFragOITBlend);
    auto fragOITModel = initShader(ShaderType::Fragment, kFragOITModel);
    auto fragOITParticles = initShader(ShaderType::Fragment, kFragOITParticles);
    auto fragPointLightShadows = initShader(ShaderType::Fragment, kFragPointLightShadows);
    auto fragPostBoxBlur4 = initShader(ShaderType::Fragment, kFragPostBoxBlur4);
    auto fragPostFXAA = initShader(ShaderType::Fragment, kFragPostFXAA);
    auto fragPostGaussianBlur13 = initShader(ShaderType::Fragment, kFragPostGaussianBlur13);
    auto fragPostGaussianBlur9 = initShader(ShaderType::Fragment, kFragPostGaussianBlur9);
    auto fragPostMedianFilter3 = initShader(ShaderType::Fragment, kFragPostMedianFilter3);
    auto fragPostMedianFilter5 = initShader(ShaderType::Fragment, kFragPostMedianFilter5);
    auto fragPostSharpen = initShader(ShaderType::Fragment, kFragPostSharpen);
    auto fragText = initShader(ShaderType::Fragment, kFragText);
    auto fragTexture = initShader(ShaderType::Fragment, kFragTexture);
    auto fragTextureNoPerspective = initShader(ShaderType::Fragment, kFragTextureNoPerspective);
    auto fragIrradiance = initShader(ShaderType::Fragment, kFragPBRIrradiance);
    auto fragPBRBRDF = initShader(ShaderType::Fragment, kFragPBRBRDF);
    auto fragPBRPrefilter = initShader(ShaderType::Fragment, kFragPBRPrefilter);
    auto fragProfiler = initShader(ShaderType::Fragment, kFragProfiler);

    // Shader Programs
    _shaderRegistry.add(ShaderProgramId::billboard, initShaderProgram({vertBillboard, fragTexture}));
    _shaderRegistry.add(ShaderProgramId::retroOpaqueModel, initShaderProgram({vertModel, fragRetroOpaqueModel}));
    _shaderRegistry.add(ShaderProgramId::retroGrass, initShaderProgram({vertGrass, fragRetroGrass}));
    _shaderRegistry.add(ShaderProgramId::retroAABB, initShaderProgram({vertAABB, fragRetroAABB}));
    _shaderRegistry.add(ShaderProgramId::retroWalkmesh, initShaderProgram({vertWalkmesh, fragRetroWalkmesh}));
    _shaderRegistry.add(ShaderProgramId::pbrAABB, initShaderProgram({vertAABB, fragPBRAABB}));
    _shaderRegistry.add(ShaderProgramId::pbrCombine, initShaderProgram({vertPassthrough, fragPBRCombine}));
    _shaderRegistry.add(ShaderProgramId::pbrGrass, initShaderProgram({vertGrass, fragPBRGrass}));
    _shaderRegistry.add(ShaderProgramId::pbrOpaqueModel, initShaderProgram({vertModel, fragPBROpaqueModel}));
    _shaderRegistry.add(ShaderProgramId::pbrSSAO, initShaderProgram({vertPassthrough, fragPBRSSAO}));
    _shaderRegistry.add(ShaderProgramId::pbrSSR, initShaderProgram({vertPassthrough, fragPBRSSR}));
    _shaderRegistry.add(ShaderProgramId::pbrWalkmesh, initShaderProgram({vertWalkmesh, fragPBRWalkmesh}));
    _shaderRegistry.add(ShaderProgramId::dirLightShadows, initShaderProgram({vertShadows, geomDirLightShadows, fragNull}));
    _shaderRegistry.add(ShaderProgramId::mvpColor, initShaderProgram({vertMVP, fragColor}));
    _shaderRegistry.add(ShaderProgramId::mvpTexture, initShaderProgram({vertMVP, fragTexture}));
    _shaderRegistry.add(ShaderProgramId::ndcTexture, initShaderProgram({vertPassthrough, fragTextureNoPerspective}));
    _shaderRegistry.add(ShaderProgramId::oitBlend, initShaderProgram({vertPassthrough, fragOITBlend}));
    _shaderRegistry.add(ShaderProgramId::oitModel, initShaderProgram({vertModel, fragOITModel}));
    _shaderRegistry.add(ShaderProgramId::oitParticles, initShaderProgram({vertParticles, fragOITParticles}));
    _shaderRegistry.add(ShaderProgramId::pointLightShadows, initShaderProgram({vertShadows, geomPointLightShadows, fragPointLightShadows}));
    _shaderRegistry.add(ShaderProgramId::postBoxBlur4, initShaderProgram({vertPassthrough, fragPostBoxBlur4}));
    _shaderRegistry.add(ShaderProgramId::postFXAA, initShaderProgram({vertPassthrough, fragPostFXAA}));
    _shaderRegistry.add(ShaderProgramId::postGaussianBlur13, initShaderProgram({vertPassthrough, fragPostGaussianBlur13}));
    _shaderRegistry.add(ShaderProgramId::postGaussianBlur9, initShaderProgram({vertPassthrough, fragPostGaussianBlur9}));
    _shaderRegistry.add(ShaderProgramId::postMedianFilter3, initShaderProgram({vertPassthrough, fragPostMedianFilter3}));
    _shaderRegistry.add(ShaderProgramId::postMedianFilter5, initShaderProgram({vertPassthrough, fragPostMedianFilter5}));
    _shaderRegistry.add(ShaderProgramId::postSharpen, initShaderProgram({vertPassthrough, fragPostSharpen}));
    _shaderRegistry.add(ShaderProgramId::text, initShaderProgram({vertText, fragText}));
    _shaderRegistry.add(ShaderProgramId::pbrIrradiance, initShaderProgram({vertMVP, fragIrradiance}));
    _shaderRegistry.add(ShaderProgramId::brdfLUT, initShaderProgram({vertMVP, fragPBRBRDF}));
    _shaderRegistry.add(ShaderProgramId::pbrPrefilter, initShaderProgram({vertMVP, fragPBRPrefilter}));
    _shaderRegistry.add(ShaderProgramId::profiler, initShaderProgram({vertMVP, fragProfiler}));

    _inited = true;
}

void Shaders::deinit() {
    if (!_inited) {
        return;
    }
    _inited = false;
}

std::shared_ptr<Shader> Shaders::initShader(ShaderType type, std::string resRef) {
    debug(
        str(boost::format("Initializing shader: type=%d resRef='%s'") % static_cast<int>(type) % resRef),
        LogChannel::Graphics);

    std::list<std::string> sources;

    std::list<std::string> resRefs;
    resRefs.push_back(resRef);
    while (!resRefs.empty()) {
        auto rr = resRefs.back();
        resRefs.pop_back();
        if (_sourceResRefToData.count(rr) == 0) {
            auto res = _resources.get(ResourceId(rr, ResType::Glsl));
            _sourceResRefToData[rr] = std::move(res.data);
        }
        auto source = StringBuilder();
        auto stream = MemoryInputStream(_sourceResRefToData.at(rr));
        auto reader = TextReader(stream);
        while (auto line = reader.readLine()) {
            std::smatch match;
            if (std::regex_search(*line, match, std::regex("^#include \"([\\d\\w_]+)\\.glsl\"$"))) {
                resRefs.push_back(match[1].str());
                continue;
            }
            source.append(*line);
            source.append("\n");
        }
        sources.push_front(source.string());
    }

    // Prepend preprocessor directives
    auto defines = StringBuilder();
    if (_graphicsOpt.ssr) {
        defines.append("#define R_SSR\n");
    }
    if (_graphicsOpt.ssao) {
        defines.append("#define R_SSAO\n");
    }
    if (!defines.empty()) {
        defines.append("\n");
        sources.push_front(defines.string());
    }
    sources.push_front("#version 400 core\n\n");

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
    program->setUniform("sEnvMap", TextureUnits::envMap);
    program->setUniform("sNormalMap", TextureUnits::normalMap);
    program->setUniform("sGBufSelfIllum", TextureUnits::gBufSelfIllum);
    program->setUniform("sGBufDepth", TextureUnits::gBufDepth);
    program->setUniform("sGBufEyeNormal", TextureUnits::gBufEyeNormal);
    program->setUniform("sSSAO", TextureUnits::ssao);
    program->setUniform("sSSR", TextureUnits::ssr);
    program->setUniform("sHilights", TextureUnits::hilights);
    program->setUniform("sOITAccum", TextureUnits::oitAccum);
    program->setUniform("sOITRevealage", TextureUnits::oitRevealage);
    program->setUniform("sNoise", TextureUnits::noise);
    program->setUniform("sEnvMapCube", TextureUnits::envMapCube);
    program->setUniform("sShadowMapCube", TextureUnits::shadowMapCube);
    program->setUniform("sBumpMapArray", TextureUnits::bumpMapArray);
    program->setUniform("sShadowMap", TextureUnits::shadowMapArray);
    program->setUniform("sBRDFLUT", TextureUnits::brdfLUT);
    program->setUniform("sIrradianceMapArray", TextureUnits::irradianceMapArray);
    program->setUniform("sPrefilteredEnvMapArray", TextureUnits::prefilteredEnvMapArray);

    // Uniform Blocks
    program->bindUniformBlock("Globals", UniformBlockBindingPoints::globals);
    program->bindUniformBlock("Locals", UniformBlockBindingPoints::locals);
    program->bindUniformBlock("Bones", UniformBlockBindingPoints::bones);
    program->bindUniformBlock("Dangly", UniformBlockBindingPoints::dangly);
    program->bindUniformBlock("Particles", UniformBlockBindingPoints::particles);
    program->bindUniformBlock("Grass", UniformBlockBindingPoints::grass);
    program->bindUniformBlock("Walkmesh", UniformBlockBindingPoints::walkmesh);
    program->bindUniformBlock("Text", UniformBlockBindingPoints::text);
    program->bindUniformBlock("ScreenEffect", UniformBlockBindingPoints::screenEffect);

    return program;
}

} // namespace resource

} // namespace reone
