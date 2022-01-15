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

#pragma once

#include "shader.h"
#include "shaderprogram.h"
#include "uniformbuffer.h"
#include "uniforms.h"

namespace reone {

namespace graphics {

class Shaders : boost::noncopyable {
public:
    ~Shaders() { deinit(); }

    void init();
    void deinit();

    void use(ShaderProgram &program, bool refreshUniforms = false);

    void refreshSSAOUniforms();

    Uniforms &uniforms() { return _uniforms; }

    ShaderProgram &simpleColor() const { return *_spSimpleColor; }
    ShaderProgram &simpleTexture() const { return *_spSimpleTexture; }
    ShaderProgram &gui() const { return *_spGUI; }
    ShaderProgram &text() const { return *_spText; }

    ShaderProgram &pointLightShadows() const { return *_spPointLightShadows; }
    ShaderProgram &directionalLightShadows() const { return *_spDirectionalLightShadows; }
    ShaderProgram &modelOpaque() const { return *_spModelOpaque; }
    ShaderProgram &modelTransparent() const { return *_spModelTransparent; }
    ShaderProgram &particle() const { return *_spParticle; }
    ShaderProgram &grass() const { return *_spGrass; }
    ShaderProgram &billboard() const { return *_spBillboard; }
    ShaderProgram &ssao() const { return *_spSSAO; }
    ShaderProgram &ssr() const { return *_spSSR; }
    ShaderProgram &combineOpaque() const { return *_spCombineOpaque; }
    ShaderProgram &combineOIT() const { return *_spCombineOIT; }

    ShaderProgram &gaussianBlur9() const { return *_spGaussianBlur9; }
    ShaderProgram &gaussianBlur13() const { return *_spGaussianBlur13; }
    ShaderProgram &medianFilter3() const { return *_spMedianFilter3; }
    ShaderProgram &medianFilter5() const { return *_spMedianFilter5; }
    ShaderProgram &fxaa() const { return *_spFXAA; }

private:
    bool _inited {false};
    Uniforms _uniforms;

    ShaderProgram *_usedProgram {nullptr};

    // Shader Programs

    std::shared_ptr<ShaderProgram> _spSimpleColor;
    std::shared_ptr<ShaderProgram> _spSimpleTexture;
    std::shared_ptr<ShaderProgram> _spGUI;
    std::shared_ptr<ShaderProgram> _spText;

    std::shared_ptr<ShaderProgram> _spPointLightShadows;
    std::shared_ptr<ShaderProgram> _spDirectionalLightShadows;
    std::shared_ptr<ShaderProgram> _spModelOpaque;
    std::shared_ptr<ShaderProgram> _spModelTransparent;
    std::shared_ptr<ShaderProgram> _spParticle;
    std::shared_ptr<ShaderProgram> _spGrass;
    std::shared_ptr<ShaderProgram> _spBillboard;
    std::shared_ptr<ShaderProgram> _spSSAO;
    std::shared_ptr<ShaderProgram> _spSSR;
    std::shared_ptr<ShaderProgram> _spCombineOpaque;
    std::shared_ptr<ShaderProgram> _spCombineOIT;

    std::shared_ptr<ShaderProgram> _spGaussianBlur9;
    std::shared_ptr<ShaderProgram> _spGaussianBlur13;
    std::shared_ptr<ShaderProgram> _spMedianFilter3;
    std::shared_ptr<ShaderProgram> _spMedianFilter5;
    std::shared_ptr<ShaderProgram> _spFXAA;

    // END Shader Programs

    // Uniform Buffers

    std::shared_ptr<UniformBuffer> _ubGeneral;
    std::shared_ptr<UniformBuffer> _ubText;
    std::shared_ptr<UniformBuffer> _ubLighting;
    std::shared_ptr<UniformBuffer> _ubSkeletal;
    std::shared_ptr<UniformBuffer> _ubParticles;
    std::shared_ptr<UniformBuffer> _ubGrass;
    std::shared_ptr<UniformBuffer> _ubSSAO;

    // END Uniform Buffers

    std::shared_ptr<Shader> initShader(ShaderType type, std::vector<std::string> sources);
    std::shared_ptr<ShaderProgram> initShaderProgram(std::vector<std::shared_ptr<Shader>> shaders);
    std::unique_ptr<UniformBuffer> initUniformBuffer(const void *data, ptrdiff_t size);

    void refreshGeneralUniforms();
    void refreshFeatureUniforms();
};

} // namespace graphics

} // namespace reone
