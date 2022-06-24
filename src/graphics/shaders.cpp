/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "shaders.h"

#include "glsl/common.h"
#include "glsl/fragment.h"
#include "glsl/geometry.h"
#include "glsl/vertex.h"

using namespace std;

namespace reone {

namespace graphics {

void Shaders::init() {
    if (_inited) {
        return;
    }

    // Shaders
    auto vsObjectSpace = initShader(ShaderType::Vertex, {g_glslHeader, g_vsObjectSpace});
    auto vsClipSpace = initShader(ShaderType::Vertex, {g_glslHeader, g_glslGeneralUniforms, g_vsClipSpace});
    auto vsShadows = initShader(ShaderType::Vertex, {g_glslHeader, g_glslGeneralUniforms, g_vsShadows});
    auto vsModel = initShader(ShaderType::Vertex, {g_glslHeader, g_glslGeneralUniforms, g_glslSkeletalUniforms, g_vsModel});
    auto vsWalkmesh = initShader(ShaderType::Vertex, {g_glslHeader, g_glslGeneralUniforms, g_glslWalkmeshUniforms, g_vsWalkmesh});
    auto vsBillboard = initShader(ShaderType::Vertex, {g_glslHeader, g_glslGeneralUniforms, g_vsBillboard});
    auto vsParticle = initShader(ShaderType::Vertex, {g_glslHeader, g_glslGeneralUniforms, g_glslParticleUniforms, g_vsParticle});
    auto vsGrass = initShader(ShaderType::Vertex, {g_glslHeader, g_glslGeneralUniforms, g_glslGrassUniforms, g_vsGrass});
    auto vsText = initShader(ShaderType::Vertex, {g_glslHeader, g_glslGeneralUniforms, g_glslTextUniforms, g_vsText});
    auto vsPoints = initShader(ShaderType::Vertex, {g_glslHeader, g_glslGeneralUniforms, g_glslPointsUniforms, g_vsPoints});
    auto gsPointLightShadows = initShader(ShaderType::Geometry, {g_glslHeader, g_glslGeneralUniforms, g_gsPointLightShadows});
    auto gsDirectionalLightShadows = initShader(ShaderType::Geometry, {g_glslHeader, g_glslGeneralUniforms, g_gsDirectionalLightShadows});
    auto fsColor = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_fsColor});
    auto fsTexture = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_fsTexture});
    auto fsGUI = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_fsGUI});
    auto fsText = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_glslTextUniforms, g_fsText});
    auto fsPointLightShadows = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_fsPointLightShadows});
    auto fsDirectionalLightShadows = initShader(ShaderType::Fragment, {g_glslHeader, g_fsDirectionalLightShadows});
    auto fsModelOpaque = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_glslMath, g_glslHash, g_glslHashedAlphaTest, g_glslEnvironmentMapping, g_glslNormalMapping, g_fsModelOpaque});
    auto fsModelTransparent = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_glslMath, g_glslEnvironmentMapping, g_glslNormalMapping, g_glslOIT, g_glslLuma, g_fsModelTransparent});
    auto fsAABB = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_fsAABB});
    auto fsWalkmesh = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_glslWalkmeshUniforms, g_fsWalkmesh});
    auto fsBillboard = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_fsBillboard});
    auto fsParticle = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_glslParticleUniforms, g_glslOIT, g_glslLuma, g_fsParticle});
    auto fsGrass = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_glslGrassUniforms, g_glslHash, g_glslHashedAlphaTest, g_fsGrass});
    auto fsSSAO = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_glslSSAOUniforms, g_fsSSAO});
    auto fsSSR = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_fsSSR});
    auto fsCombineOpaque = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_glslLightingUniforms, g_glslMath, g_glslBRDF, g_glslLighting, g_glslLuma, g_glslShadowMapping, g_glslFog, g_fsCombineOpaque});
    auto fsCombineGeometry = initShader(ShaderType::Fragment, {g_glslHeader, g_fsCombineGeometry});
    auto fsBoxBlur4 = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_fsBoxBlur4});
    auto fsGaussianBlur9 = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_fsGaussianBlur9});
    auto fsGaussianBlur13 = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_fsGaussianBlur13});
    auto fsMedianFilter3 = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_fsMedianFilter3});
    auto fsMedianFilter5 = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_fsMedianFilter5});
    auto fsFXAA = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_glslLuma, g_fsFXAA});
    auto fsSharpen = initShader(ShaderType::Fragment, {g_glslHeader, g_glslGeneralUniforms, g_fsSharpen});

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

void Shaders::use(ShaderProgram &program) {
    if (_usedProgram != &program) {
        program.use();
        _usedProgram = &program;
    }
}

shared_ptr<Shader> Shaders::initShader(ShaderType type, vector<string> sources) {
    auto shader = make_unique<Shader>(type, move(sources));
    shader->init();
    return move(shader);
}

shared_ptr<ShaderProgram> Shaders::initShaderProgram(vector<shared_ptr<Shader>> shaders) {
    auto program = make_unique<ShaderProgram>(move(shaders));
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

    return move(program);
}

} // namespace graphics

} // namespace reone
