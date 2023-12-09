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

#include "reone/graphics/renderpass.h"

#include "reone/graphics/context.h"
#include "reone/graphics/material.h"
#include "reone/graphics/mesh.h"
#include "reone/graphics/meshregistry.h"
#include "reone/graphics/shaderregistry.h"
#include "reone/graphics/texture.h"
#include "reone/graphics/uniforms.h"

namespace reone {

namespace graphics {

void RenderPass::draw(Mesh &mesh,
                      Material &material,
                      const glm::mat4 &transform,
                      const glm::mat4 &transformInv) {
    withMaterialAppliedToContext(material, [&]() {
        _uniforms.setLocals([this, &material, &transform, &transformInv](auto &locals) {
            locals.reset();
            locals.model = transform;
            locals.modelInv = transformInv;
            applyMaterialToLocals(material, locals);
        });
        mesh.draw();
    });
}

void RenderPass::withMaterialAppliedToContext(const Material &material, std::function<void()> block) {
    _context.useProgram(_shaderRegistry.get(material.programId));
    for (const auto &[unit, texture] : material.textures) {
        _context.bindTexture(texture, unit);
    }
    auto prevBlending = _context.blending();
    if (material.blending && *material.blending != prevBlending) {
        _context.pushBlending(*material.blending);
    }
    auto prevFaceCulling = _context.faceCulling();
    if (material.faceCulling && *material.faceCulling != prevFaceCulling) {
        _context.pushFaceCulling(*material.faceCulling);
    }
    auto prevPolygonMode = _context.polygonMode();
    if (material.polygonMode && *material.polygonMode != prevPolygonMode) {
        _context.pushPolygonMode(*material.polygonMode);
    }
    block();
    if (material.blending && *material.blending != prevBlending) {
        _context.popBlending();
    }
    if (material.faceCulling && *material.faceCulling != prevFaceCulling) {
        _context.popFaceCulling();
    }
    if (material.polygonMode && *material.polygonMode != prevPolygonMode) {
        _context.popPolygonMode();
    }
}

int RenderPass::materialFeatureMask(const Material &material) const {
    int mask = 0;
    const auto &textures = material.textures;
    if (textures.count(TextureUnits::mainTex) > 0) {
        const auto &mainTex = textures.at(TextureUnits::mainTex).get();
        switch (mainTex.features().blending) {
        case Texture::Blending::PunchThrough:
            mask |= UniformsFeatureFlags::hashedalphatest;
            break;
        case Texture::Blending::Additive:
            if (textures.count(TextureUnits::environmentMap) == 0 &&
                textures.count(TextureUnits::environmentMapCube) == 0) {
                mask |= UniformsFeatureFlags::premulalpha;
            }
            break;
        default:
            break;
        }
        if (mainTex.features().waterAlpha != -1.0f) {
            mask |= UniformsFeatureFlags::water;
        }
    }
    if (textures.count(TextureUnits::lightmap) > 0) {
        mask |= UniformsFeatureFlags::lightmap;
    }
    if (textures.count(TextureUnits::environmentMap) > 0) {
        mask |= UniformsFeatureFlags::envmap;
    }
    if (textures.count(TextureUnits::environmentMapCube) > 0) {
        mask |= UniformsFeatureFlags::envmap | UniformsFeatureFlags::envmapcube;
    }
    if (textures.count(TextureUnits::bumpMap) > 0) {
        mask |= textures.at(TextureUnits::bumpMap).get().isGrayscale()
                    ? UniformsFeatureFlags::heightmap
                    : UniformsFeatureFlags::normalmap;
    }
    if (material.affectedByShadows) {
        mask |= UniformsFeatureFlags::shadows;
    }
    if (material.affectedByFog) {
        mask |= UniformsFeatureFlags::fog;
    }
    return mask;
}

void RenderPass::drawSkinned(Mesh &mesh,
                             Material &material,
                             const glm::mat4 &transform,
                             const glm::mat4 &transformInv,
                             const std::vector<glm::mat4> &bones) {
    withMaterialAppliedToContext(material, [&]() {
        _uniforms.setLocals([this, &material, &transform, &transformInv](auto &locals) {
            locals.reset();
            locals.featureMask |= UniformsFeatureFlags::skin;
            locals.model = transform;
            locals.modelInv = transformInv;
            applyMaterialToLocals(material, locals);
        });
        _uniforms.setBones([&bones](auto &b) {
            std::memcpy(b.bones, &bones[0], kMaxBones * sizeof(glm::mat4));
        });
        mesh.draw();
    });
}

void RenderPass::drawBillboard(Texture &texture,
                               const glm::vec4 &color,
                               const glm::mat4 &transform,
                               const glm::mat4 &transformInv,
                               std::optional<float> size) {
    _context.useProgram(_shaderRegistry.get(ShaderProgramId::billboard));
    _context.bindTexture(texture, TextureUnits::mainTex);
    _uniforms.setLocals([&transform, &transformInv, &size, &color](auto &locals) {
        locals.reset();
        locals.model = transform;
        locals.modelInv = transformInv;
        locals.color = color;
        if (size) {
            locals.featureMask |= UniformsFeatureFlags::fixedsize;
            locals.billboardSize = *size;
        }
    });
    _context.pushBlending(BlendMode::Additive);
    _meshRegistry.get(MeshName::billboard).draw();
    _context.popBlending();
}

void RenderPass::drawParticles(Texture &texture,
                               FaceCullMode faceCulling,
                               bool premultipliedAlpha,
                               const glm::ivec2 &gridSize,
                               const std::vector<ParticleInstance> &particles) {
    _context.useProgram(_shaderRegistry.get(ShaderProgramId::oitParticles));
    _context.bindTexture(texture, TextureUnits::mainTex);
    _uniforms.setLocals([&premultipliedAlpha](auto &locals) {
        locals.reset();
        if (premultipliedAlpha) {
            locals.featureMask |= UniformsFeatureFlags::premulalpha;
        }
    });
    _uniforms.setParticles([&gridSize, &premultipliedAlpha, &particles](auto &p) {
        p.gridSize = gridSize;
        for (size_t i = 0; i < particles.size(); ++i) {
            const auto &particle = particles[i];
            p.particles[i].positionFrame = glm::vec4(particle.position, static_cast<float>(particle.frame));
            p.particles[i].size = particle.size;
            p.particles[i].color = particle.color;
            p.particles[i].right = glm::vec4(particle.right, 0.0f);
            p.particles[i].up = glm::vec4(particle.up, 0.0f);
        }
    });
    auto prevFaceCulling = _context.faceCulling();
    if (faceCulling != prevFaceCulling) {
        _context.pushFaceCulling(faceCulling);
    }
    _meshRegistry.get(MeshName::billboard).drawInstanced(particles.size());
    if (faceCulling != prevFaceCulling) {
        _context.popFaceCulling();
    }
}

void RenderPass::drawGrass(float radius,
                           float quadSize,
                           Texture &texture,
                           std::optional<std::reference_wrapper<Texture>> &lightmap,
                           const std::vector<GrassInstance> &instances) {
    _context.useProgram(_shaderRegistry.get(ShaderProgramId::deferredGrass));
    _context.bindTexture(texture, TextureUnits::mainTex);
    if (lightmap) {
        _context.bindTexture(lightmap->get(), TextureUnits::lightmap);
    }
    _uniforms.setLocals([&lightmap](auto &locals) {
        locals.reset();
        locals.featureMask |= UniformsFeatureFlags::hashedalphatest;
        if (lightmap) {
            locals.featureMask |= UniformsFeatureFlags::lightmap;
        }
    });
    _uniforms.setGrass([&radius, &quadSize, &instances](auto &grass) {
        grass.radius = radius;
        grass.quadSize = glm::vec2(quadSize);
        for (size_t i = 0; i < instances.size(); ++i) {
            const auto &instance = instances[i];
            grass.clusters[i].positionVariant = glm::vec4(instance.position, static_cast<float>(instance.variant));
            grass.clusters[i].lightmapUV = instance.lightmapUV;
        }
    });
    _meshRegistry.get(MeshName::grass).drawInstanced(instances.size());
}

void RenderPass::applyMaterialToLocals(const Material &material,
                                       LocalUniforms &locals) {
    locals.featureMask |= materialFeatureMask(material);
    locals.uv = material.uv;
    locals.color = material.color;
    locals.selfIllumColor = glm::vec4(material.selfIllumColor, 1.0f);
    if (material.textures.count(TextureUnits::mainTex) > 0) {
        const auto &mainTex = material.textures.at(TextureUnits::mainTex).get();
        if (mainTex.features().waterAlpha != -1.0f) {
            locals.waterAlpha = mainTex.features().waterAlpha;
        }
    }
    if (material.textures.count(TextureUnits::bumpMap) > 0) {
        const auto &bumpmap = material.textures.at(TextureUnits::bumpMap).get();
        locals.heightMapScaling = bumpmap.features().bumpMapScaling;
        int bumpmapW = bumpmap.width();
        int bumpmapH = bumpmap.height();
        int gridX = bumpmap.features().numX;
        int gridY = bumpmap.features().numY;
        int frameW = bumpmapW / gridX;
        int frameH = bumpmapH / gridY;
        switch (bumpmap.features().procedureType) {
        case Texture::ProcedureType::Cycle:
            locals.heightMapFrameBounds = glm::vec4(
                static_cast<float>(frameW * (material.heightMapFrame % gridX)),
                static_cast<float>(frameH * (material.heightMapFrame / gridX)),
                static_cast<float>(frameW),
                static_cast<float>(frameH));
            break;
        default:
            locals.heightMapFrameBounds = glm::vec4(
                0.0f,
                0.0f,
                static_cast<float>(frameW),
                static_cast<float>(frameH));
            break;
        }
    }
}

void RenderPass::drawImage(Texture &texture,
                           const glm::ivec2 &position,
                           const glm::ivec2 &scale,
                           glm::vec4 color = glm::vec4(1.0f),
                           glm::mat3x4 uv = glm::mat3x4(1.0f)) {
    _uniforms.setLocals([&position, &color, &uv, &scale](auto &locals) {
        locals.reset();
        locals.model = glm::translate(glm::vec3(position.x, position.y, 0.0f));
        locals.model *= glm::scale(glm::vec3(scale.x, scale.y, 1.0f));
        locals.uv = std::move(uv);
        locals.color = std::move(color);
    });
    _context.useProgram(_shaderRegistry.get(ShaderProgramId::mvpTexture));
    _context.bindTexture(texture, TextureUnits::mainTex);
    _meshRegistry.get(MeshName::quad).draw();
}

} // namespace graphics

} // namespace reone
