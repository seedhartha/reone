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

#include "uniforms.h"

namespace reone {

namespace graphics {

class IGraphicsContext;
class ShaderProgram;
class Texture;

class Material {
public:
    using TextureUnit = int;

    virtual ~Material() = default;

    void setTexture(Texture &texture, TextureUnit unit) {
        _textures[unit] = &texture;
    }

    virtual void apply(IGraphicsContext &context);

protected:
    using TextureMap = std::map<TextureUnit, Texture *>;

    Material(ShaderProgram &program) :
        _program(program) {
    }

    ShaderProgram &_program;
    TextureMap _textures;
};

class ModelMaterial : public Material {
public:
    ModelMaterial(ShaderProgram &program) :
        Material(program) {
    }

    void apply(IGraphicsContext &context) override;

    LocalsUniforms &localsUniforms() {
        return _localsUniforms;
    }

private:
    LocalsUniforms _localsUniforms;
};

class SkinnedModelMaterial : public ModelMaterial {
public:
    SkinnedModelMaterial(ShaderProgram &program) :
        ModelMaterial(program) {
    }

    void apply(IGraphicsContext &context) override;

    SkeletalUniforms &skeletalUniforms() {
        return _skeletalUniforms;
    }

private:
    SkeletalUniforms _skeletalUniforms;
};

} // namespace graphics

} // namespace reone
