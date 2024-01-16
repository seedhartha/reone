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

#include "../pass.h"

namespace reone {

namespace graphics {

class ShaderProgram;

}

namespace scene {

class PBRRenderPass : public IRenderPass, boost::noncopyable {
public:
    PBRRenderPass(graphics::GraphicsOptions &options,
                  graphics::IContext &context,
                  graphics::IShaderRegistry &shaderRegistry,
                  graphics::IMeshRegistry &meshRegistry,
                  graphics::IPBRTextures &pbrTextures,
                  graphics::ITextureRegistry &textureRegistry,
                  graphics::IUniforms &uniforms) :
        _options(options),
        _context(context),
        _shaderRegistry(shaderRegistry),
        _meshRegistry(meshRegistry),
        _pbrTextures(pbrTextures),
        _textureRegistry(textureRegistry),
        _uniforms(uniforms) {
    }

    void draw(graphics::Mesh &mesh,
              graphics::Material &material,
              const glm::mat4 &transform,
              const glm::mat4 &transformInv) override;

    void drawSkinned(graphics::Mesh &mesh,
                     graphics::Material &material,
                     const glm::mat4 &transform,
                     const glm::mat4 &transformInv,
                     const std::vector<glm::mat4> &bones) override;

    void drawDangly(graphics::Mesh &mesh,
                    graphics::Material &material,
                    const glm::mat4 &transform,
                    const glm::mat4 &transformInv,
                    const std::vector<glm::vec4> &positions) override;

    void drawSaber(graphics::Mesh &mesh,
                   graphics::Material &material,
                   const glm::mat4 &transform,
                   const glm::mat4 &transformInv,
                   const glm::vec4 &displacement) override;

    void drawBillboard(graphics::Texture &texture,
                       const glm::vec4 &color,
                       const glm::mat4 &transform,
                       const glm::mat4 &transformInv,
                       std::optional<float> size) override;

    void drawParticles(graphics::Texture &texture,
                       graphics::FaceCullMode faceCulling,
                       bool premultipliedAlpha,
                       const glm::ivec2 &gridSize,
                       const std::vector<ParticleInstance> &particles) override;

    void drawGrass(float radius,
                   float quadSize,
                   graphics::Texture &texture,
                   std::optional<std::reference_wrapper<graphics::Texture>> &lightmap,
                   const std::vector<GrassInstance> &instances) override;

    void drawAABB(const std::vector<glm::vec4> &corners) override;

    void drawImage(graphics::Texture &texture,
                   const glm::ivec2 &position,
                   const glm::ivec2 &scale,
                   glm::vec4 color,
                   glm::mat3x4 uv) override;

private:
    graphics::GraphicsOptions &_options;
    graphics::IContext &_context;
    graphics::IShaderRegistry &_shaderRegistry;
    graphics::IMeshRegistry &_meshRegistry;
    graphics::IPBRTextures &_pbrTextures;
    graphics::ITextureRegistry &_textureRegistry;
    graphics::IUniforms &_uniforms;

    void applyMaterialToLocals(const graphics::Material &material, graphics::LocalUniforms &locals);

    int materialFeatureMask(const graphics::Material &material) const;

    void withMaterialAppliedToContext(const graphics::Material &material, std::function<void(graphics::ShaderProgram &)> block);
};

} // namespace scene

} // namespace reone
