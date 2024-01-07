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

#include "reone/scene/node/mesh.h"

#include "reone/graphics/context.h"
#include "reone/graphics/di/services.h"
#include "reone/graphics/lumautil.h"
#include "reone/graphics/material.h"
#include "reone/graphics/mesh.h"
#include "reone/graphics/shaderregistry.h"
#include "reone/graphics/texture.h"
#include "reone/graphics/textureutil.h"
#include "reone/graphics/uniforms.h"
#include "reone/resource/di/services.h"
#include "reone/resource/provider/textures.h"
#include "reone/scene/graph.h"
#include "reone/scene/node/camera.h"
#include "reone/scene/node/light.h"
#include "reone/scene/node/model.h"
#include "reone/scene/renderpipeline.h"
#include "reone/system/logutil.h"
#include "reone/system/randomutil.h"

using namespace reone::graphics;

namespace reone {

namespace scene {

static constexpr float kUvAnimationSpeed = 250.0f;

void MeshSceneNode::init() {
    _point = false;
    _alpha = _modelNode.alpha().getByFrameOrElse(0, 1.0f);
    _selfIllumColor = _modelNode.selfIllumColor().getByFrameOrElse(0, glm::vec3(0.0f));

    initTextures();
    initDanglyMesh();
}

void MeshSceneNode::initTextures() {
    std::shared_ptr<ModelNode::TriangleMesh> mesh(_modelNode.mesh());
    if (!mesh) {
        return;
    }
    if (!mesh->diffuseMap.empty()) {
        auto diffuseMap = _resourceSvc.textures.get(mesh->diffuseMap, TextureUsage::Diffuse);
        _nodeTextures.diffuse = diffuseMap.get();
    }
    if (!mesh->lightmap.empty()) {
        auto lightmap = _resourceSvc.textures.get(mesh->lightmap, TextureUsage::Lightmap);
        _nodeTextures.lightmap = lightmap.get();
    }
    if (!mesh->bumpmap.empty()) {
        auto bumpmap = _resourceSvc.textures.get(mesh->bumpmap, TextureUsage::BumpMap);
        _nodeTextures.bumpmap = bumpmap.get();
    }
    refreshAdditionalTextures();
}

void MeshSceneNode::refreshAdditionalTextures() {
    _nodeTextures.bumpmap = nullptr;
    if (!_nodeTextures.diffuse) {
        return;
    }
    const Texture::Features &features = _nodeTextures.diffuse->features();
    if (!features.envmapTexture.empty()) {
        _nodeTextures.envmap = _resourceSvc.textures.get(features.envmapTexture, TextureUsage::EnvironmentMap).get();
    } else if (!features.bumpyShinyTexture.empty()) {
        _nodeTextures.envmap = _resourceSvc.textures.get(features.bumpyShinyTexture, TextureUsage::EnvironmentMap).get();
    }
    if (!features.bumpmapTexture.empty()) {
        _nodeTextures.bumpmap = _resourceSvc.textures.get(features.bumpmapTexture, TextureUsage::BumpMap).get();
    }
}

void MeshSceneNode::update(float dt) {
    SceneNode::update(dt);

    std::shared_ptr<ModelNode::TriangleMesh> mesh(_modelNode.mesh());
    if (mesh) {
        updateUVAnimation(dt, *mesh);
        updateBumpmapAnimation(dt, *mesh);
        if (mesh->danglymesh) {
            updateDanglyAnimation(dt, *mesh->danglymesh);
        }
    }
}

void MeshSceneNode::updateUVAnimation(float dt, const ModelNode::TriangleMesh &mesh) {
    if (mesh.uvAnimation.dir.x != 0.0f || mesh.uvAnimation.dir.y != 0.0f) {
        _uvOffset += kUvAnimationSpeed * mesh.uvAnimation.dir * dt;
        _uvOffset -= glm::floor(_uvOffset);
    }
}

void MeshSceneNode::updateBumpmapAnimation(float dt, const ModelNode::TriangleMesh &mesh) {
    if (!_nodeTextures.bumpmap) {
        return;
    }
    const Texture::Features &features = _nodeTextures.bumpmap->features();
    if (features.procedureType == Texture::ProcedureType::Cycle) {
        int frameCount = features.numX * features.numY;
        float length = frameCount / static_cast<float>(features.fps);
        _bumpmapCycleTime = glm::min(_bumpmapCycleTime + dt, length);
        _bumpmapCycleFrame = static_cast<int>(glm::round((frameCount - 1) * (_bumpmapCycleTime / length)));
        if (_bumpmapCycleTime == length) {
            _bumpmapCycleTime = 0.0f;
        }
    }
}

static std::string vectorToString(const glm::vec3 &vec) {
    return str(boost::format("[%.04f, %.04f, %.04f]") % vec.x % vec.y % vec.z);
}

void MeshSceneNode::updateDanglyAnimation(float dt, const ModelNode::Danglymesh &mesh) {
    if (dt < 0.0125f) {
        dt = 0.0125f;
    } else if (dt > 0.035f) {
        dt = 0.035f;
    }
    glm::vec3 worldPos = _absTransform[3];
    if (glm::distance(worldPos, _dangly.prevWorldPos) < 5.0f) {
        for (size_t i = 0; i < _dangly.vertices.size(); ++i) {
            if (mesh.constraints[i] == 0.0f) {
                continue;
            }
            auto &vertex = _dangly.vertices[i];
            glm::vec3 acceleration {0.0f};
            acceleration += -vertex.displacement * (0.5f * mesh.tightness * mesh.constraints[i]);
            acceleration += -vertex.velocity * (1.5f * mesh.period);
            bool wind = true;
            if (wind) {
                // TODO: implement wind
            }
            vertex.velocity += acceleration * dt;
            vertex.displacement += vertex.velocity * dt;

            auto inertia = 20.0f * mesh.displacement * (1.0f - mesh.constraints[i] / 255.0f) * (worldPos - _dangly.prevWorldPos);
            inertia = _absTransformInv * glm::vec4 {inertia, 0.0f};
            vertex.displacement += inertia;

            float dispmag = glm::length(vertex.displacement);
            if (dispmag > 0.0f) {
                float maxdisp = mesh.displacement * mesh.constraints[i] / 255.0f;
                vertex.displacement = glm::min(dispmag, maxdisp) * vertex.displacement / dispmag;
            }
        }
    }
    _dangly.prevWorldPos = std::move(worldPos);
}

bool MeshSceneNode::shouldRender() const {
    auto mesh = _modelNode.mesh();
    if (!mesh || !mesh->render || _modelNode.alpha().getByFrameOrElse(0, 1.0f) == 0.0f) {
        return false;
    }
    return !_modelNode.isAABBMesh() && !mesh->diffuseMap.empty();
}

bool MeshSceneNode::shouldCastShadows() const {
    std::shared_ptr<ModelNode::TriangleMesh> mesh(_modelNode.mesh());
    if (!mesh) {
        return false;
    }
    if (_model.usage() == ModelUsage::Creature) {
        return mesh->shadow && !_modelNode.isSkinMesh();
    } else if (_model.usage() == ModelUsage::Placeable) {
        return mesh->render;
    } else {
        return false;
    }
}

bool MeshSceneNode::isTransparent() const {
    if (!_nodeTextures.diffuse) {
        return false;
    }
    auto blending = _nodeTextures.diffuse->features().blending;
    switch (blending) {
    case Texture::Blending::Additive:
        return true;
    case Texture::Blending::PunchThrough:
        return false;
    default:
        break;
    }
    if (_alpha < 1.0f) {
        return true;
    }
    if (_nodeTextures.envmap || _nodeTextures.bumpmap) {
        return false;
    }
    if ((1.0f - rgbToLuma(_selfIllumColor)) < 0.01f) {
        return false;
    }
    return hasAlphaChannel(_nodeTextures.diffuse->pixelFormat());
}

static bool isLightingEnabledByUsage(ModelUsage usage) {
    return usage != ModelUsage::Projectile;
}

static bool isReceivingShadows(const ModelSceneNode &model, const MeshSceneNode &modelNode) {
    return model.usage() == ModelUsage::Room;
}

void MeshSceneNode::render(IRenderPass &pass) {
    auto mesh = _modelNode.mesh();
    if (!mesh || !_nodeTextures.diffuse) {
        return;
    }
    Material material;
    material.programId = isTransparent()
                             ? ShaderProgramId::oitModel
                             : ShaderProgramId::deferredOpaqueModel;
    material.textures.insert({TextureUnits::mainTex, *_nodeTextures.diffuse});
    if (_nodeTextures.lightmap) {
        material.textures.insert({TextureUnits::lightmap, *_nodeTextures.lightmap});
    }
    if (_nodeTextures.envmap) {
        if (_nodeTextures.envmap->isCubemap()) {
            material.textures.insert({TextureUnits::environmentMapCube, *_nodeTextures.envmap});
        } else {
            material.textures.insert({TextureUnits::environmentMap, *_nodeTextures.envmap});
        }
    }
    if (_nodeTextures.bumpmap) {
        material.textures.insert({TextureUnits::bumpMap, *_nodeTextures.bumpmap});
        if (_nodeTextures.bumpmap->isGrayscale()) {
            material.heightMapFrame = _bumpmapCycleFrame;
        }
    }
    material.uv = glm::mat3x4(
        glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
        glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
        glm::vec4(_uvOffset.x, _uvOffset.y, 0.0f, 0.0f));
    material.color = glm::vec4(1.0f, 1.0f, 1.0f, _alpha);
    material.ambient = mesh->ambient;
    material.diffuse = mesh->diffuse;
    material.selfIllumColor = _selfIllumColor;
    if (_sceneGraph.hasShadowLight() && isReceivingShadows(_model, *this)) {
        material.affectedByShadows = true;
    }
    if (_sceneGraph.isFogEnabled() && _model.model().isAffectedByFog()) {
        material.affectedByFog = true;
    }
    material.faceCulling = FaceCullMode::Back;
    if (_modelNode.isSkinMesh()) {
        const auto &skin = *mesh->skin;
        auto bones = std::vector<glm::mat4>(kMaxBones, glm::mat4(1.0f));
        for (size_t i = 0; i < kMaxBones; ++i) {
            if (i >= skin.boneNodeNumber.size()) {
                break;
            }
            auto nodeNumber = skin.boneNodeNumber[i];
            if (nodeNumber == 0xffff) {
                continue;
            }
            auto bone = _model.getNodeByNumber(nodeNumber);
            if (!bone) {
                continue;
            }
            bones[i] = _modelNode.absoluteTransformInverse(); // convert bone transform in model space to bone transform in this model node space
            bones[i] *= _model.absoluteTransformInverse();    // convert bone transform in world space to bone transform in model space
            bones[i] *= bone->absoluteTransform();
            bones[i] *= skin.boneMatrices[skin.boneSerial[i]]; // extract changes to the bone transform in this model node space
        }
        pass.drawSkinned(*mesh->mesh, material, _absTransform, _absTransformInv, std::move(bones));
    } else if (_modelNode.isDanglymesh()) {
        std::vector<glm::vec4> positions;
        positions.reserve(_dangly.vertices.size());
        for (const auto &vertex : _dangly.vertices) {
            positions.emplace_back(vertex.position + vertex.displacement, 1.0f);
        }
        pass.drawDangly(*mesh->mesh,
                        material,
                        _absTransform,
                        _absTransformInv,
                        positions);
    } else {
        pass.draw(*mesh->mesh, material, _absTransform, _absTransformInv);
    }
}

void MeshSceneNode::renderShadow(IRenderPass &pass) {
    std::shared_ptr<ModelNode::TriangleMesh> mesh(_modelNode.mesh());
    if (!mesh) {
        return;
    }
    Material material;
    material.programId = _sceneGraph.isShadowLightDirectional()
                             ? ShaderProgramId::dirLightShadows
                             : ShaderProgramId::pointLightShadows;
    material.color = glm::vec4(1.0f, 1.0f, 1.0f, _alpha);
    pass.draw(*mesh->mesh, material, _absTransform, _absTransformInv);
}

bool MeshSceneNode::isLightingEnabled() const {
    if (!isLightingEnabledByUsage(_model.usage())) {
        return false;
    }
    // Lighting is disabled when diffuse texture is additive
    if (_nodeTextures.diffuse && _nodeTextures.diffuse->features().blending == Texture::Blending::Additive) {
        return false;
    }
    return true;
}

void MeshSceneNode::setDiffuseMap(Texture *texture) {
    ModelNodeSceneNode::setDiffuseMap(texture);
    _nodeTextures.diffuse = texture;
    refreshAdditionalTextures();
}

void MeshSceneNode::setEnvironmentMap(Texture *texture) {
    ModelNodeSceneNode::setEnvironmentMap(texture);
    _nodeTextures.envmap = std::move(texture);
}

void MeshSceneNode::initDanglyMesh() {
    auto mesh = _modelNode.mesh();
    if (!mesh || !mesh->danglymesh) {
        return;
    }
    _dangly.vertices.reserve(mesh->danglymesh->positions.size());
    for (const auto &position : mesh->danglymesh->positions) {
        DanglyVertex vertex;
        vertex.position = position;
        _dangly.vertices.push_back(std::move(vertex));
    }
}

} // namespace scene

} // namespace reone
