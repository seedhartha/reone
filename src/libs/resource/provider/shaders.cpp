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

static const std::string kUniformsGlobals = "u_globals";
static const std::string kUniformsGrass = "u_grass";
static const std::string kUniformsLights = "u_lights";
static const std::string kUniformsLocals = "u_locals";
static const std::string kUniformsParticle = "u_particle";
static const std::string kUniformsPoints = "u_points";
static const std::string kUniformsScreenEffect = "u_screeneffect";
static const std::string kUniformsSkeletal = "u_skeletal";
static const std::string kUniformsText = "u_text";
static const std::string kUniformsWalkmesh = "u_walkmesh";

static const std::string kIncludeBlinnPhong = "i_blinnphong";
static const std::string kIncludeEnvMap = "i_envmap";
static const std::string kIncludeFog = "i_fog";
static const std::string kIncludeHash = "i_hash";
static const std::string kIncludeHashedAlpha = "i_hashedalpha";
static const std::string kIncludeLighting = "i_lighting";
static const std::string kIncludeLuma = "i_luma";
static const std::string kIncludeMath = "i_math";
static const std::string kIncludeNormalMap = "i_normalmap";
static const std::string kIncludeOIT = "i_oit";
static const std::string kIncludePBR = "i_pbr";
static const std::string kIncludeShadowMap = "i_shadowmap";

static const std::string kVertBillboard = "v_billboard";
static const std::string kVertGrass = "v_grass";
static const std::string kVertModel = "v_model";
static const std::string kVertMVP2D = "v_mvp2d";
static const std::string kVertMVP3D = "v_mvp3d";
static const std::string kVertParticle = "v_particle";
static const std::string kVertPassthrough = "v_passthrough";
static const std::string kVertPoints = "v_points";
static const std::string kVertShadows = "v_shadows";
static const std::string kVertText = "v_text";
static const std::string kVertWalkmesh = "v_walkmesh";

static const std::string kGeometryDirLightShadows = "g_dirlightshadow";
static const std::string kGeometryPointLightShadows = "g_ptlightshadow";

static const std::string kFragColor = "f_color";
static const std::string kFragDeferredAABB = "f_df_aabb";
static const std::string kFragDeferredCombine = "f_df_combine";
static const std::string kFragDeferredGrass = "f_df_grass";
static const std::string kFragDeferredOpaqueModel = "f_df_opaquemodel";
static const std::string kFragDeferredSSAO = "f_df_ssao";
static const std::string kFragDeferredSSR = "f_df_ssr";
static const std::string kFragDeferredWalkmesh = "f_df_walkmesh";
static const std::string kFragNull = "f_null";
static const std::string kFragOITBlend = "f_oit_blend";
static const std::string kFragOITModel = "f_oit_model";
static const std::string kFragOITParticle = "f_oit_particle";
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
    auto vertBillboard = initShader(ShaderType::Vertex, {kUniformsGlobals, kUniformsLocals, kVertBillboard});
    auto vertGrass = initShader(ShaderType::Vertex, {kUniformsGlobals, kUniformsLocals, kUniformsGrass, kVertGrass});
    auto vertModel = initShader(ShaderType::Vertex, {kUniformsGlobals, kUniformsLocals, kUniformsSkeletal, kVertModel});
    auto vertMVP2D = initShader(ShaderType::Vertex, {kUniformsGlobals, kUniformsLocals, kVertMVP2D});
    auto vertMVP3D = initShader(ShaderType::Vertex, {kUniformsGlobals, kUniformsLocals, kVertMVP3D});
    auto vertParticle = initShader(ShaderType::Vertex, {kUniformsGlobals, kUniformsLocals, kUniformsParticle, kVertParticle});
    auto vertPassthrough = initShader(ShaderType::Vertex, {kVertPassthrough});
    auto vertPoints = initShader(ShaderType::Vertex, {kUniformsGlobals, kUniformsLocals, kUniformsPoints, kVertPoints});
    auto vertShadows = initShader(ShaderType::Vertex, {kUniformsGlobals, kUniformsLocals, kVertShadows});
    auto vertText = initShader(ShaderType::Vertex, {kUniformsGlobals, kUniformsLocals, kUniformsText, kVertText});
    auto vertWalkmesh = initShader(ShaderType::Vertex, {kUniformsGlobals, kUniformsLocals, kUniformsWalkmesh, kVertWalkmesh});

    auto geomDirLightShadows = initShader(ShaderType::Geometry, {kUniformsGlobals, kUniformsLocals, kGeometryDirLightShadows});
    auto geomPointLightShadows = initShader(ShaderType::Geometry, {kUniformsGlobals, kUniformsLocals, kGeometryPointLightShadows});

    auto fragColor = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kFragColor});
    auto fragDeferredAABB = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kFragDeferredAABB});
    auto fragDeferredCombine = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kUniformsLights, kIncludeMath, kIncludeLighting, kIncludeBlinnPhong, kIncludePBR, kIncludeLuma, kIncludeShadowMap, kIncludeFog, kFragDeferredCombine});
    auto fragDeferredGrass = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kUniformsGrass, kIncludeHash, kIncludeHashedAlpha, kFragDeferredGrass});
    auto fragDeferredOpaqueModel = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kIncludeMath, kIncludeHash, kIncludeHashedAlpha, kIncludeEnvMap, kIncludeNormalMap, kFragDeferredOpaqueModel});
    auto fragDeferredSSAO = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kUniformsScreenEffect, kFragDeferredSSAO});
    auto fragDeferredSSR = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kUniformsScreenEffect, kFragDeferredSSR});
    auto fragDeferredWalkmesh = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kUniformsWalkmesh, kFragDeferredWalkmesh});
    auto fragNull = initShader(ShaderType::Fragment, {kFragNull});
    auto fragOITBlend = initShader(ShaderType::Fragment, {kFragOITBlend});
    auto fragOITModel = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kIncludeMath, kIncludeEnvMap, kIncludeNormalMap, kIncludeOIT, kIncludeLuma, kFragOITModel});
    auto fragOITParticle = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kUniformsParticle, kIncludeOIT, kIncludeLuma, kFragOITParticle});
    auto fragPointLightShadows = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kFragPointLightShadows});
    auto fragPostBoxBlur4 = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kUniformsScreenEffect, kFragPostBoxBlur4});
    auto fragPostFXAA = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kUniformsScreenEffect, kIncludeLuma, kFragPostFXAA});
    auto fragPostGaussianBlur13 = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kUniformsScreenEffect, kFragPostGaussianBlur13});
    auto fragPostGaussianBlur9 = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kUniformsScreenEffect, kFragPostGaussianBlur9});
    auto fragPostMedianFilter3 = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kUniformsScreenEffect, kFragPostMedianFilter3});
    auto fragPostMedianFilter5 = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kUniformsScreenEffect, kFragPostMedianFilter5});
    auto fragPostSharpen = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kUniformsScreenEffect, kFragPostSharpen});
    auto fragText = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kUniformsText, kFragText});
    auto fragTexture = initShader(ShaderType::Fragment, {kUniformsGlobals, kUniformsLocals, kFragTexture});

    // Shader Programs
    _shaderRegistry.add(ShaderProgramId::billboard, initShaderProgram({vertBillboard, fragTexture}));
    _shaderRegistry.add(ShaderProgramId::color2D, initShaderProgram({vertMVP2D, fragColor}));
    _shaderRegistry.add(ShaderProgramId::deferredAABB, initShaderProgram({vertMVP3D, fragDeferredAABB}));
    _shaderRegistry.add(ShaderProgramId::deferredCombine, initShaderProgram({vertPassthrough, fragDeferredCombine}));
    _shaderRegistry.add(ShaderProgramId::deferredGrass, initShaderProgram({vertGrass, fragDeferredGrass}));
    _shaderRegistry.add(ShaderProgramId::deferredOpaqueModel, initShaderProgram({vertModel, fragDeferredOpaqueModel}));
    _shaderRegistry.add(ShaderProgramId::deferredSSAO, initShaderProgram({vertPassthrough, fragDeferredSSAO}));
    _shaderRegistry.add(ShaderProgramId::deferredSSR, initShaderProgram({vertPassthrough, fragDeferredSSR}));
    _shaderRegistry.add(ShaderProgramId::deferredWalkmesh, initShaderProgram({vertWalkmesh, fragDeferredWalkmesh}));
    _shaderRegistry.add(ShaderProgramId::dirLightShadows, initShaderProgram({vertShadows, geomDirLightShadows, fragNull}));
    _shaderRegistry.add(ShaderProgramId::oitBlend, initShaderProgram({vertPassthrough, fragOITBlend}));
    _shaderRegistry.add(ShaderProgramId::oitModel, initShaderProgram({vertModel, fragOITModel}));
    _shaderRegistry.add(ShaderProgramId::oitParticle, initShaderProgram({vertParticle, fragOITParticle}));
    _shaderRegistry.add(ShaderProgramId::pointLightShadows, initShaderProgram({vertShadows, geomPointLightShadows, fragPointLightShadows}));
    _shaderRegistry.add(ShaderProgramId::points, initShaderProgram({vertPoints, fragColor}));
    _shaderRegistry.add(ShaderProgramId::postBoxBlur4, initShaderProgram({vertPassthrough, fragPostBoxBlur4}));
    _shaderRegistry.add(ShaderProgramId::postFXAA, initShaderProgram({vertPassthrough, fragPostFXAA}));
    _shaderRegistry.add(ShaderProgramId::postGaussianBlur13, initShaderProgram({vertPassthrough, fragPostGaussianBlur13}));
    _shaderRegistry.add(ShaderProgramId::postGaussianBlur9, initShaderProgram({vertPassthrough, fragPostGaussianBlur9}));
    _shaderRegistry.add(ShaderProgramId::postMedianFilter3, initShaderProgram({vertPassthrough, fragPostMedianFilter3}));
    _shaderRegistry.add(ShaderProgramId::postMedianFilter5, initShaderProgram({vertPassthrough, fragPostMedianFilter5}));
    _shaderRegistry.add(ShaderProgramId::postSharpen, initShaderProgram({vertPassthrough, fragPostSharpen}));
    _shaderRegistry.add(ShaderProgramId::text, initShaderProgram({vertText, fragText}));
    _shaderRegistry.add(ShaderProgramId::texture2D, initShaderProgram({vertMVP2D, fragTexture}));

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
    defines.append("#define R_PBR\n");
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
    program->bindUniformBlock("Lights", UniformBlockBindingPoints::lights);
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
