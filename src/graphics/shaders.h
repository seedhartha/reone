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

    Uniforms &uniforms() { return _uniforms; }

    ShaderProgram &simpleColor() const { return *_spSimpleColor; }
    ShaderProgram &pointLightShadows() const { return *_spPointLightShadows; }
    ShaderProgram &directionalLightShadows() const { return *_spDirectionalLightShadows; }
    ShaderProgram &blinnPhong() const { return *_spBlinnPhong; }
    ShaderProgram &blinnPhongDiffuseless() const { return *_spBlingPhongDiffuseless; }
    ShaderProgram &particle() const { return *_spParticle; }
    ShaderProgram &grass() const { return *_spGrass; }
    ShaderProgram &billboard() const { return *_spBillboard; }
    ShaderProgram &ssr() const { return *_spSSR; }
    ShaderProgram &blur() const { return *_spBlur; }
    ShaderProgram &bloom() const { return *_spBloom; }
    ShaderProgram &fxaa() const { return *_spFXAA; }
    ShaderProgram &presentWorld() const { return *_spPresentWorld; }
    ShaderProgram &gui() const { return *_spGUI; }
    ShaderProgram &text() const { return *_spText; }

private:
    bool _inited {false};
    Uniforms _uniforms;

    ShaderProgram *_usedProgram {nullptr};

    // Shader Programs

    std::shared_ptr<ShaderProgram> _spSimpleColor;
    std::shared_ptr<ShaderProgram> _spPointLightShadows;
    std::shared_ptr<ShaderProgram> _spDirectionalLightShadows;
    std::shared_ptr<ShaderProgram> _spBlinnPhong;
    std::shared_ptr<ShaderProgram> _spBlingPhongDiffuseless;
    std::shared_ptr<ShaderProgram> _spParticle;
    std::shared_ptr<ShaderProgram> _spGrass;
    std::shared_ptr<ShaderProgram> _spBillboard;
    std::shared_ptr<ShaderProgram> _spSSR;
    std::shared_ptr<ShaderProgram> _spBlur;
    std::shared_ptr<ShaderProgram> _spBloom;
    std::shared_ptr<ShaderProgram> _spFXAA;
    std::shared_ptr<ShaderProgram> _spPresentWorld;
    std::shared_ptr<ShaderProgram> _spGUI;
    std::shared_ptr<ShaderProgram> _spText;

    // END Shader Programs

    // Uniform Buffers

    std::shared_ptr<UniformBuffer> _ubGeneral;
    std::shared_ptr<UniformBuffer> _ubText;
    std::shared_ptr<UniformBuffer> _ubLighting;
    std::shared_ptr<UniformBuffer> _ubSkeletal;
    std::shared_ptr<UniformBuffer> _ubParticles;
    std::shared_ptr<UniformBuffer> _ubGrass;

    // END Uniform Buffers

    std::shared_ptr<Shader> initShader(ShaderType type, std::vector<std::string> sources);
    std::shared_ptr<ShaderProgram> initShaderProgram(std::vector<std::shared_ptr<Shader>> shaders);
    std::unique_ptr<UniformBuffer> initUniformBuffer(const void *data, ptrdiff_t size);

    void refreshUniforms();
};

} // namespace graphics

} // namespace reone
