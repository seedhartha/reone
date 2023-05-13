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

#pragma once

#include "shader.h"
#include "shaderprogram.h"

namespace reone {

namespace graphics {

struct GraphicsOptions;

class IShaders {
public:
    virtual ~IShaders() = default;

    virtual void use(ShaderProgram &program) = 0;

    virtual ShaderProgram &simpleColor() const = 0;
    virtual ShaderProgram &simpleTexture() const = 0;
    virtual ShaderProgram &gui() const = 0;
    virtual ShaderProgram &text() const = 0;
    virtual ShaderProgram &points() const = 0;

    virtual ShaderProgram &pointLightShadows() const = 0;
    virtual ShaderProgram &directionalLightShadows() const = 0;
    virtual ShaderProgram &modelOpaque() const = 0;
    virtual ShaderProgram &modelTransparent() const = 0;
    virtual ShaderProgram &aabb() const = 0;
    virtual ShaderProgram &walkmesh() const = 0;
    virtual ShaderProgram &particle() const = 0;
    virtual ShaderProgram &grass() const = 0;
    virtual ShaderProgram &billboard() const = 0;
    virtual ShaderProgram &ssao() const = 0;
    virtual ShaderProgram &ssr() const = 0;
    virtual ShaderProgram &combineOpaque() const = 0;
    virtual ShaderProgram &combineGeometry() const = 0;

    virtual ShaderProgram &boxBlur4() const = 0;
    virtual ShaderProgram &gaussianBlur9() const = 0;
    virtual ShaderProgram &gaussianBlur13() const = 0;
    virtual ShaderProgram &medianFilter3() const = 0;
    virtual ShaderProgram &medianFilter5() const = 0;
    virtual ShaderProgram &fxaa() const = 0;
    virtual ShaderProgram &sharpen() const = 0;
};

class Shaders : public IShaders, boost::noncopyable {
public:
    Shaders(GraphicsOptions &options) :
        _options(options) {
    }

    ~Shaders() { deinit(); }

    void init();
    void deinit();

    void use(ShaderProgram &program) override;

    ShaderProgram &simpleColor() const override { return *_spSimpleColor; }
    ShaderProgram &simpleTexture() const override { return *_spSimpleTexture; }
    ShaderProgram &gui() const override { return *_spGUI; }
    ShaderProgram &text() const override { return *_spText; }
    ShaderProgram &points() const override { return *_spPoints; }

    ShaderProgram &pointLightShadows() const override { return *_spPointLightShadows; }
    ShaderProgram &directionalLightShadows() const override { return *_spDirectionalLightShadows; }
    ShaderProgram &modelOpaque() const override { return *_spModelOpaque; }
    ShaderProgram &modelTransparent() const override { return *_spModelTransparent; }
    ShaderProgram &aabb() const override { return *_spAABB; }
    ShaderProgram &walkmesh() const override { return *_spWalkmesh; }
    ShaderProgram &particle() const override { return *_spParticle; }
    ShaderProgram &grass() const override { return *_spGrass; }
    ShaderProgram &billboard() const override { return *_spBillboard; }
    ShaderProgram &ssao() const override { return *_spSSAO; }
    ShaderProgram &ssr() const override { return *_spSSR; }
    ShaderProgram &combineOpaque() const override { return *_spCombineOpaque; }
    ShaderProgram &combineGeometry() const override { return *_spCombineGeometry; }

    ShaderProgram &boxBlur4() const override { return *_spBoxBlur4; }
    ShaderProgram &gaussianBlur9() const override { return *_spGaussianBlur9; }
    ShaderProgram &gaussianBlur13() const override { return *_spGaussianBlur13; }
    ShaderProgram &medianFilter3() const override { return *_spMedianFilter3; }
    ShaderProgram &medianFilter5() const override { return *_spMedianFilter5; }
    ShaderProgram &fxaa() const override { return *_spFXAA; }
    ShaderProgram &sharpen() const override { return *_spSharpen; }

private:
    GraphicsOptions &_options;

    bool _inited {false};

    ShaderProgram *_usedProgram {nullptr};

    // Shader Programs

    std::shared_ptr<ShaderProgram> _spSimpleColor;
    std::shared_ptr<ShaderProgram> _spSimpleTexture;
    std::shared_ptr<ShaderProgram> _spGUI;
    std::shared_ptr<ShaderProgram> _spText;
    std::shared_ptr<ShaderProgram> _spPoints;

    std::shared_ptr<ShaderProgram> _spPointLightShadows;
    std::shared_ptr<ShaderProgram> _spDirectionalLightShadows;
    std::shared_ptr<ShaderProgram> _spModelOpaque;
    std::shared_ptr<ShaderProgram> _spModelTransparent;
    std::shared_ptr<ShaderProgram> _spAABB;
    std::shared_ptr<ShaderProgram> _spWalkmesh;
    std::shared_ptr<ShaderProgram> _spParticle;
    std::shared_ptr<ShaderProgram> _spGrass;
    std::shared_ptr<ShaderProgram> _spBillboard;
    std::shared_ptr<ShaderProgram> _spSSAO;
    std::shared_ptr<ShaderProgram> _spSSR;
    std::shared_ptr<ShaderProgram> _spCombineOpaque;
    std::shared_ptr<ShaderProgram> _spCombineGeometry;

    std::shared_ptr<ShaderProgram> _spBoxBlur4;
    std::shared_ptr<ShaderProgram> _spGaussianBlur9;
    std::shared_ptr<ShaderProgram> _spGaussianBlur13;
    std::shared_ptr<ShaderProgram> _spMedianFilter3;
    std::shared_ptr<ShaderProgram> _spMedianFilter5;
    std::shared_ptr<ShaderProgram> _spFXAA;
    std::shared_ptr<ShaderProgram> _spSharpen;

    // END Shader Programs

    std::shared_ptr<Shader> initShader(ShaderType type, std::list<std::string> sources);
    std::shared_ptr<ShaderProgram> initShaderProgram(std::vector<std::shared_ptr<Shader>> shaders);
};

} // namespace graphics

} // namespace reone
