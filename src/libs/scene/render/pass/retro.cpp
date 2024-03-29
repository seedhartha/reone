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

#include "reone/scene/render/pass/retro.h"

#include "reone/graphics/context.h"
#include "reone/graphics/material.h"
#include "reone/graphics/mesh.h"
#include "reone/graphics/meshregistry.h"
#include "reone/graphics/shaderregistry.h"
#include "reone/graphics/texture.h"
#include "reone/graphics/uniforms.h"
#include "reone/system/logutil.h"

using namespace reone::graphics;

namespace reone {

namespace scene {

void RetroRenderPass::draw(Mesh &mesh,
                           Material &material,
                           const glm::mat4 &transform,
                           const glm::mat4 &transformInv) {
    withMaterialAppliedToContext(material, [&](auto &program) {
        _uniforms.setLocals([this, &material, &transform, &transformInv](auto &locals) {
            locals.reset();
            locals.model = transform;
            locals.modelInv = transformInv;
            applyMaterialToLocals(material, locals);
        });
        mesh.draw(_statistic);
    });
}

void RetroRenderPass::withMaterialAppliedToContext(const Material &material, std::function<void(ShaderProgram &)> block) {
    static const std::unordered_map<MaterialType, std::string> kMatTypeToProgramId {
        {MaterialType::DirLightShadow, ShaderProgramId::dirLightShadows},     //
        {MaterialType::PointLightShadow, ShaderProgramId::pointLightShadows}, //
        {MaterialType::OpaqueModel, ShaderProgramId::retroOpaqueModel},       //
        {MaterialType::TransparentModel, ShaderProgramId::oitModel},          //
        {MaterialType::Walkmesh, ShaderProgramId::retroWalkmesh}              //
    };
    if (kMatTypeToProgramId.count(material.type) == 0) {
        throw std::invalid_argument(str(boost::format("Material type %1% is not associated with a shader program") % static_cast<int>(material.type)));
    }
    auto &program = _shaderRegistry.get(kMatTypeToProgramId.at(material.type));
    _context.useProgram(program);
    for (const auto &[unit, texture] : material.textures) {
        _context.bindTexture(texture, unit);
    }
    auto prevBlending = _context.blendMode();
    if (material.blending && *material.blending != prevBlending) {
        _context.pushBlendMode(*material.blending);
    }
    auto prevFaceCulling = _context.faceCullMode();
    if (material.faceCulling && *material.faceCulling != prevFaceCulling) {
        _context.pushFaceCullMode(*material.faceCulling);
    }
    auto prevPolygonMode = _context.polygonMode();
    if (material.polygonMode && *material.polygonMode != prevPolygonMode) {
        _context.pushPolygonMode(*material.polygonMode);
    }
    block(program);
    if (material.blending && *material.blending != prevBlending) {
        _context.popBlendMode();
    }
    if (material.faceCulling && *material.faceCulling != prevFaceCulling) {
        _context.popFaceCullMode();
    }
    if (material.polygonMode && *material.polygonMode != prevPolygonMode) {
        _context.popPolygonMode();
    }
}

int RetroRenderPass::materialFeatureMask(const Material &material) const {
    int mask = 0;
    const auto &textures = material.textures;
    if (textures.count(TextureUnits::mainTex) > 0) {
        const auto &mainTex = textures.at(TextureUnits::mainTex).get();
        switch (mainTex.features().blending) {
        case Texture::Blending::PunchThrough:
            mask |= UniformsFeatureFlags::hashedalphatest;
            break;
        case Texture::Blending::Additive:
            if (textures.count(TextureUnits::envMap) == 0 &&
                textures.count(TextureUnits::envMapCube) == 0) {
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
    if (textures.count(TextureUnits::envMap) > 0) {
        mask |= UniformsFeatureFlags::envmap;
    }
    if (textures.count(TextureUnits::envMapCube) > 0) {
        mask |= UniformsFeatureFlags::envmap | UniformsFeatureFlags::envmapcube;
    }
    if (textures.count(TextureUnits::normalMap) > 0) {
        mask |= UniformsFeatureFlags::normalmap;
    }
    if (textures.count(TextureUnits::bumpMapArray) > 0) {
        mask |= UniformsFeatureFlags::bumpmap;
    }
    if (material.staticObject) {
        mask |= UniformsFeatureFlags::staticobj;
    }
    if (material.affectedByShadows) {
        mask |= UniformsFeatureFlags::shadows;
    }
    if (material.affectedByFog) {
        mask |= UniformsFeatureFlags::fog;
    }
    return mask;
}

void RetroRenderPass::drawSkinned(Mesh &mesh,
                                  Material &material,
                                  const glm::mat4 &transform,
                                  const glm::mat4 &transformInv,
                                  const std::vector<glm::mat4> &bones) {
    withMaterialAppliedToContext(material, [&](auto &program) {
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
        mesh.draw(_statistic);
    });
}

void RetroRenderPass::drawDangly(Mesh &mesh,
                                 Material &material,
                                 const glm::mat4 &transform,
                                 const glm::mat4 &transformInv,
                                 const std::vector<glm::vec4> &positions) {
    withMaterialAppliedToContext(material, [&](auto &program) {
        _uniforms.setLocals([this, &material, &transform, &transformInv](auto &locals) {
            locals.reset();
            locals.featureMask |= UniformsFeatureFlags::dangly;
            locals.model = transform;
            locals.modelInv = transformInv;
            applyMaterialToLocals(material, locals);
        });
        _uniforms.setDangly([&positions](auto &dangly) {
            auto numPositions = std::min<int>(kMaxDanglyVertices, positions.size());
            std::memcpy(dangly.positions, &positions[0], numPositions * sizeof(glm::vec4));
        });
        mesh.draw(_statistic);
    });
}

void RetroRenderPass::drawSaber(Mesh &mesh,
                                Material &material,
                                const glm::mat4 &transform,
                                const glm::mat4 &transformInv,
                                const glm::vec4 &displacement) {
    withMaterialAppliedToContext(material, [&](auto &program) {
        _uniforms.setLocals([this, &material, &transform, &transformInv](auto &locals) {
            locals.reset();
            locals.featureMask |= UniformsFeatureFlags::saber;
            locals.model = transform;
            locals.modelInv = transformInv;
            applyMaterialToLocals(material, locals);
        });
        program.setUniform("uSaberDisplacement", displacement);
        mesh.draw(_statistic);
    });
}

void RetroRenderPass::drawBillboard(Texture &texture,
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
    _context.pushBlendMode(BlendMode::Additive);
    _meshRegistry.get(MeshName::billboard).draw(_statistic);
    _context.popBlendMode();
}

void RetroRenderPass::drawParticles(Texture &texture,
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
    auto prevFaceCulling = _context.faceCullMode();
    if (faceCulling != prevFaceCulling) {
        _context.pushFaceCullMode(faceCulling);
    }
    _meshRegistry.get(MeshName::billboard).drawInstanced(particles.size(), _statistic);
    if (faceCulling != prevFaceCulling) {
        _context.popFaceCullMode();
    }
}

void RetroRenderPass::drawGrass(float radius,
                                float quadSize,
                                Texture &texture,
                                std::optional<std::reference_wrapper<Texture>> &lightmap,
                                const std::vector<GrassInstance> &instances) {
    _context.useProgram(_shaderRegistry.get(ShaderProgramId::retroGrass));
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
    _meshRegistry.get(MeshName::grass).drawInstanced(instances.size(), _statistic);
}

void RetroRenderPass::drawAABB(const std::vector<glm::vec4> &corners) {
    auto &program = _shaderRegistry.get(ShaderProgramId::retroAABB);
    _context.useProgram(program);
    program.setUniform("uCorners", corners);
    _context.withDepthMask(false, [this]() {
        _context.withPolygonMode(PolygonMode::Line, [this]() {
            _meshRegistry.get(MeshName::aabb).draw(_statistic);
        });
    });
}

void RetroRenderPass::applyMaterialToLocals(const Material &material,
                                            LocalUniforms &locals) {
    locals.featureMask |= materialFeatureMask(material);
    locals.uv = material.uv;
    locals.color = material.color;
    locals.ambientColor = glm::vec4 {material.ambientColor, 0.0f};
    locals.diffuseColor = glm::vec4 {material.diffuseColor, 0.0f};
    locals.selfIllumColor = glm::vec4(material.selfIllumColor, 1.0f);
    if (material.textures.count(TextureUnits::mainTex) > 0) {
        const auto &mainTex = material.textures.at(TextureUnits::mainTex).get();
        if (mainTex.features().waterAlpha != -1.0f) {
            locals.waterAlpha = mainTex.features().waterAlpha;
        }
    }
    if (material.textures.count(TextureUnits::bumpMapArray) > 0) {
        const auto &bumpmap = material.textures.at(TextureUnits::bumpMapArray).get();
        locals.bumpMapFrame = material.bumpMapFrame;
        locals.bumpMapScale = bumpmap.features().bumpMapScaling;
    }
}

void RetroRenderPass::drawImage(Texture &texture,
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
    _meshRegistry.get(MeshName::quad).draw(_statistic);
}

} // namespace scene

} // namespace reone
