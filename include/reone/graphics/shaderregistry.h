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

enum class ShaderProgramId {
    None,

    SimpleColor,
    SimpleTexture,
    GUI,
    Text,
    Points,

    PointLightShadows,
    DirectionalLightShadows,
    ModelOpaque,
    ModelTransparent,
    AABB,
    Walkmesh,
    Particle,
    Grass,
    Billboard,
    SSAO,
    SSR,
    CombineOpaque,
    CombineGeometry,

    BoxBlur4,
    GaussianBlur9,
    GaussianBlur13,
    MedianFilter3,
    MedianFilter5,
    FXAA,
    Sharpen
};

class IShaderRegistry {
public:
    virtual ~IShaderRegistry() = default;

    virtual ShaderProgram &get(ShaderProgramId programId) = 0;
};

class ShaderRegistry : public IShaderRegistry, boost::noncopyable {
public:
    void add(ShaderProgramId programId, std::shared_ptr<ShaderProgram> program) {
        _idToProgram[programId] = std::move(program);
    }

    ShaderProgram &get(ShaderProgramId programId) {
        auto program = _idToProgram.find(programId);
        if (program == _idToProgram.end()) {
            throw std::runtime_error("Shader program not found by id: " + std::to_string(static_cast<int>(programId)));
        }
        return *program->second;
    }

private:
    std::map<ShaderProgramId, std::shared_ptr<ShaderProgram>> _idToProgram;
};

} // namespace graphics

} // namespace reone
