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

class GraphicsContext;

class Shaders : boost::noncopyable {
public:
    Shaders(GraphicsContext &graphicsContext) :
        _graphicsContext(graphicsContext) {
    }

    ~Shaders() { deinit(); }

    void init();
    void deinit();

    void refreshUniforms();

    Uniforms &uniforms() { return _uniforms; }

    std::shared_ptr<ShaderProgram> simpleColor() const { return _spSimpleColor; }
    std::shared_ptr<ShaderProgram> modelColor() const { return _spModelColor; }
    std::shared_ptr<ShaderProgram> depth() const { return _spDepth; }
    std::shared_ptr<ShaderProgram> gui() const { return _spGUI; }
    std::shared_ptr<ShaderProgram> blur() const { return _spBlur; }
    std::shared_ptr<ShaderProgram> presentWorld() const { return _spPresentWorld; }
    std::shared_ptr<ShaderProgram> blinnPhong() const { return _spBlinnPhong; }
    std::shared_ptr<ShaderProgram> blinnPhongDiffuseless() const { return _spBlingPhongDiffuseless; }
    std::shared_ptr<ShaderProgram> particle() const { return _spParticle; }
    std::shared_ptr<ShaderProgram> grass() const { return _spGrass; }
    std::shared_ptr<ShaderProgram> text() const { return _spText; }
    std::shared_ptr<ShaderProgram> billboard() const { return _spBillboard; }

private:
    GraphicsContext &_graphicsContext;

    bool _inited {false};
    Uniforms _uniforms;

    // Shader Programs
    std::shared_ptr<ShaderProgram> _spSimpleColor;
    std::shared_ptr<ShaderProgram> _spModelColor;
    std::shared_ptr<ShaderProgram> _spDepth;
    std::shared_ptr<ShaderProgram> _spGUI;
    std::shared_ptr<ShaderProgram> _spBlur;
    std::shared_ptr<ShaderProgram> _spPresentWorld;
    std::shared_ptr<ShaderProgram> _spBlinnPhong;
    std::shared_ptr<ShaderProgram> _spBlingPhongDiffuseless;
    std::shared_ptr<ShaderProgram> _spParticle;
    std::shared_ptr<ShaderProgram> _spGrass;
    std::shared_ptr<ShaderProgram> _spText;
    std::shared_ptr<ShaderProgram> _spBillboard;

    // Uniform Buffers
    std::shared_ptr<UniformBuffer> _ubGeneral;
    std::shared_ptr<UniformBuffer> _ubText;
    std::shared_ptr<UniformBuffer> _ubLighting;
    std::shared_ptr<UniformBuffer> _ubSkeletal;
    std::shared_ptr<UniformBuffer> _ubParticles;
    std::shared_ptr<UniformBuffer> _ubGrass;
    std::shared_ptr<UniformBuffer> _ubDanglymesh;

    std::shared_ptr<Shader> initShader(ShaderType type, std::vector<std::string> sources);
    std::shared_ptr<ShaderProgram> initShaderProgram(std::vector<std::shared_ptr<Shader>> shaders);
    std::unique_ptr<UniformBuffer> initUniformBuffer(const void *data, ptrdiff_t size);
};

} // namespace graphics

} // namespace reone
