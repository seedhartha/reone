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
    static constexpr char billboard[] = "billboard";
    static constexpr char retroOpaqueModel[] = "retro_opaque_model";
    static constexpr char retroGrass[] = "retro_grass";
    static constexpr char retroAABB[] = "retro_aabb";
    static constexpr char retroWalkmesh[] = "retro_walkmesh";
    static constexpr char pbrAABB[] = "pbr_aabb";
    static constexpr char pbrCombine[] = "pbr_combine";
    static constexpr char pbrGrass[] = "pbr_grass";
    static constexpr char pbrOpaqueModel[] = "pbr_opaque_model";
    static constexpr char pbrSSAO[] = "pbr_ssao";
    static constexpr char pbrSSR[] = "pbr_ssr";
    static constexpr char pbrWalkmesh[] = "pbr_walkmesh";
    static constexpr char dirLightShadows[] = "dir_light_shadows";
    static constexpr char mvpColor[] = "mvp_color";
    static constexpr char mvpTexture[] = "mvp_texture";
    static constexpr char ndcTexture[] = "ndc_texture";
    static constexpr char oitBlend[] = "oit_blend";
    static constexpr char oitModel[] = "oit_model";
    static constexpr char oitParticles[] = "oit_particles";
    static constexpr char pointLightShadows[] = "point_light_shadows";
    static constexpr char postBoxBlur4[] = "post_box_blur4";
    static constexpr char postFXAA[] = "post_fxaa";
    static constexpr char postGaussianBlur13[] = "post_gaussian_blur13";
    static constexpr char postGaussianBlur9[] = "post_gaussian_blur9";
    static constexpr char postMedianFilter3[] = "post_median_filter3";
    static constexpr char postMedianFilter5[] = "post_median_filter5";
    static constexpr char postSharpen[] = "post_sharpen";
    static constexpr char text[] = "text";
    static constexpr char pbrIrradiance[] = "pbr_irradiance";
    static constexpr char pbrBRDF[] = "pbr_brdf";
    static constexpr char pbrPrefilter[] = "pbr_prefilter";
    static constexpr char profiler[] = "profiler";
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
