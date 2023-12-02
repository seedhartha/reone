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

struct ShaderProgramId {
    static constexpr char simpleColor[] = "simple_color";
    static constexpr char simpleTexture[] = "simple_texture";
    static constexpr char gui[] = "gui";
    static constexpr char text[] = "text";
    static constexpr char points[] = "points";
    static constexpr char pointLightShadows[] = "point_light_shadows";
    static constexpr char directionalLightShadows[] = "directional_light_shadows";
    static constexpr char modelOpaque[] = "model_opaque";
    static constexpr char modelTransparent[] = "model_transparent";
    static constexpr char aabb[] = "aabb";
    static constexpr char walkmesh[] = "walkmesh";
    static constexpr char particle[] = "particle";
    static constexpr char grass[] = "grass";
    static constexpr char billboard[] = "billboard";
    static constexpr char ssao[] = "ssao";
    static constexpr char ssr[] = "ssr";
    static constexpr char combineOpaque[] = "combine_opaque";
    static constexpr char combineGeometry[] = "combine_geometry";
    static constexpr char boxBlur4[] = "box_blur4";
    static constexpr char gaussianBlur9[] = "gaussian_blur9";
    static constexpr char gaussianBlur13[] = "gaussian_blur13";
    static constexpr char medianFilter3[] = "median_filter3";
    static constexpr char medianFilter5[] = "median_filter5";
    static constexpr char fxaa[] = "fxaa";
    static constexpr char sharpen[] = "sharpen";
};

struct GraphicsOptions;

class IShaderRegistry {
public:
    virtual ~IShaderRegistry() = default;

    virtual ShaderProgram &get(const std::string &programId) = 0;
};

class ShaderRegistry : public IShaderRegistry, boost::noncopyable {
public:
    void add(std::string programId, std::shared_ptr<ShaderProgram> program) {
        _idToProgram[std::move(programId)] = std::move(program);
    }

    ShaderProgram &get(const std::string &programId) {
        auto program = _idToProgram.find(programId);
        if (program == _idToProgram.end()) {
            throw std::runtime_error("Shader program not found by id: " + programId);
        }
        return *program->second;
    }

private:
    std::map<std::string, std::shared_ptr<ShaderProgram>> _idToProgram;
};

} // namespace graphics

} // namespace reone
