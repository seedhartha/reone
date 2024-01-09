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

#include "types.h"

namespace reone {

namespace graphics {

class IContext;
class IMeshRegistry;
class IShaderRegistry;
class ITextureRegistry;
class IUniforms;
class Mesh;
class Texture;

struct LocalUniforms;
struct Material;

enum class RenderPassName {
    None,
    DirLightShadowsPass,
    PointLightShadows,
    OpaqueGeometry,
    TransparentGeometry,
    PostProcessing
};

struct ParticleInstance {
    int frame {0};
    glm::vec3 position {0.0f};
    glm::vec2 size {0.0f};
    glm::vec4 color {1.0f};
    glm::vec3 right {0.0f};
    glm::vec3 up {0.0f};
};

struct GrassInstance {
    int variant {0};
    glm::vec3 position {0.0f};
    glm::vec2 lightmapUV {0.0f};
};

class IRenderPass {
public:
    virtual ~IRenderPass() = default;

    virtual void draw(Mesh &mesh,
                      Material &material,
                      const glm::mat4 &transform,
                      const glm::mat4 &transformInv) = 0;

    virtual void drawSkinned(Mesh &mesh,
                             Material &material,
                             const glm::mat4 &transform,
                             const glm::mat4 &transformInv,
                             const std::vector<glm::mat4> &bones) = 0;

    virtual void drawDangly(Mesh &mesh,
                            Material &material,
                            const glm::mat4 &transform,
                            const glm::mat4 &transformInv,
                            const std::vector<glm::vec4> &positions) = 0;

    virtual void drawSaber(Mesh &mesh,
                           Material &material,
                           const glm::mat4 &transform,
                           const glm::mat4 &transformInv,
                           const std::vector<glm::vec4> &positions) = 0;

    virtual void drawBillboard(Texture &texture,
                               const glm::vec4 &color,
                               const glm::mat4 &transform,
                               const glm::mat4 &transformInv,
                               std::optional<float> size) = 0;

    virtual void drawParticles(Texture &texture,
                               FaceCullMode faceCulling,
                               bool premultipliedAlpha,
                               const glm::ivec2 &gridSize,
                               const std::vector<ParticleInstance> &particles) = 0;

    virtual void drawGrass(float radius,
                           float quadSize,
                           Texture &texture,
                           std::optional<std::reference_wrapper<Texture>> &lightmap,
                           const std::vector<GrassInstance> &instances) = 0;

    virtual void drawImage(Texture &texture,
                           const glm::ivec2 &position,
                           const glm::ivec2 &scale,
                           glm::vec4 color = glm::vec4(1.0f),
                           glm::mat3x4 uv = glm::mat3x4(1.0f)) = 0;
};

class RenderPass : public IRenderPass, boost::noncopyable {
public:
    RenderPass(IContext &context,
               IShaderRegistry &shaderRegistry,
               IMeshRegistry &meshRegistry,
               ITextureRegistry &textureRegistry,
               IUniforms &uniforms) :
        _context(context),
        _shaderRegistry(shaderRegistry),
        _meshRegistry(meshRegistry),
        _textureRegistry(textureRegistry),
        _uniforms(uniforms) {
    }

    void draw(Mesh &mesh,
              Material &material,
              const glm::mat4 &transform,
              const glm::mat4 &transformInv) override;

    void drawSkinned(Mesh &mesh,
                     Material &material,
                     const glm::mat4 &transform,
                     const glm::mat4 &transformInv,
                     const std::vector<glm::mat4> &bones) override;

    void drawDangly(Mesh &mesh,
                    Material &material,
                    const glm::mat4 &transform,
                    const glm::mat4 &transformInv,
                    const std::vector<glm::vec4> &positions) override;

    void drawSaber(Mesh &mesh,
                   Material &material,
                   const glm::mat4 &transform,
                   const glm::mat4 &transformInv,
                   const std::vector<glm::vec4> &positions) override;

    void drawBillboard(Texture &texture,
                       const glm::vec4 &color,
                       const glm::mat4 &transform,
                       const glm::mat4 &transformInv,
                       std::optional<float> size) override;

    void drawParticles(Texture &texture,
                       FaceCullMode faceCulling,
                       bool premultipliedAlpha,
                       const glm::ivec2 &gridSize,
                       const std::vector<ParticleInstance> &particles) override;

    void drawGrass(float radius,
                   float quadSize,
                   Texture &texture,
                   std::optional<std::reference_wrapper<Texture>> &lightmap,
                   const std::vector<GrassInstance> &instances) override;

    void drawImage(Texture &texture,
                   const glm::ivec2 &position,
                   const glm::ivec2 &scale,
                   glm::vec4 color,
                   glm::mat3x4 uv) override;

private:
    IContext &_context;
    IShaderRegistry &_shaderRegistry;
    IMeshRegistry &_meshRegistry;
    ITextureRegistry &_textureRegistry;
    IUniforms &_uniforms;

    void applyMaterialToLocals(const Material &material, LocalUniforms &locals);

    int materialFeatureMask(const Material &material) const;

    void withMaterialAppliedToContext(const Material &material, std::function<void()> block);
};

} // namespace graphics

} // namespace reone
