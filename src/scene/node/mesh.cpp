/*
 * Copyright (c) 2020-2021 The reone project contributors
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

#include "mesh.h"

#include "../../common/logutil.h"
#include "../../common/randomutil.h"
#include "../../graphics/context.h"
#include "../../graphics/mesh.h"
#include "../../graphics/shaders.h"
#include "../../graphics/texture.h"
#include "../../graphics/textures.h"

#include "../graph.h"

#include "camera.h"
#include "light.h"
#include "model.h"

using namespace std;

using namespace reone::graphics;

namespace reone {

namespace scene {

static constexpr float kUvAnimationSpeed = 250.0f;

static bool g_debugWalkmesh = false;

void MeshSceneNode::init() {
    _point = false;
    _alpha = _modelNode->alpha().getByFrameOrElse(0, 1.0f);
    _selfIllumColor = _modelNode->selfIllumColor().getByFrameOrElse(0, glm::vec3(0.0f));

    initTextures();
}

void MeshSceneNode::initTextures() {
    shared_ptr<ModelNode::TriangleMesh> mesh(_modelNode->mesh());
    if (!mesh)
        return;

    _nodeTextures.diffuse = mesh->diffuseMap;
    _nodeTextures.lightmap = mesh->lightmap;
    _nodeTextures.bumpmap = mesh->bumpmap;

    refreshAdditionalTextures();
}

void MeshSceneNode::refreshAdditionalTextures() {
    _nodeTextures.envmap.reset();
    _nodeTextures.bumpmap.reset();

    if (!_nodeTextures.diffuse)
        return;

    const Texture::Features &features = _nodeTextures.diffuse->features();
    if (!features.envmapTexture.empty()) {
        _nodeTextures.envmap = _textures.get(features.envmapTexture, TextureUsage::EnvironmentMap);
    } else if (!features.bumpyShinyTexture.empty()) {
        _nodeTextures.envmap = _textures.get(features.bumpyShinyTexture, TextureUsage::EnvironmentMap);
    }
    if (!features.bumpmapTexture.empty()) {
        auto bumpmap = _textures.get(features.bumpmapTexture, TextureUsage::NormalMap);
        if (bumpmap) {
            _nodeTextures.bumpmap = _textures.get(features.bumpmapTexture, bumpmap->isGrayscale() ? TextureUsage::HeightMap : TextureUsage::NormalMap);
        }
    }
}

void MeshSceneNode::update(float dt) {
    SceneNode::update(dt);

    shared_ptr<ModelNode::TriangleMesh> mesh(_modelNode->mesh());
    if (mesh) {
        updateUVAnimation(dt, *mesh);
        updateBumpmapAnimation(dt, *mesh);
        updateDanglyMeshAnimation(dt, *mesh);
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

void MeshSceneNode::updateDanglyMeshAnimation(float dt, const ModelNode::TriangleMesh &mesh) {
    shared_ptr<ModelNode::DanglyMesh> danglyMesh(mesh.danglyMesh);
    if (!danglyMesh)
        return;

    bool forceApplied = glm::length2(_danglymeshAnimation.force) > 0.0f;
    if (forceApplied) {
        // When force is applied, stride in the opposite direction from the applied force
        glm::vec3 strideDir(-_danglymeshAnimation.force);
        glm::vec3 maxStride(danglyMesh->displacement);
        _danglymeshAnimation.stride = glm::clamp(_danglymeshAnimation.stride + danglyMesh->period * strideDir * dt, -maxStride, maxStride);
    } else {
        // When force is not applied, gradually nullify stride
        float strideMag2 = glm::length2(_danglymeshAnimation.stride);
        if (strideMag2 > 0.0f) {
            glm::vec3 strideDir(-_danglymeshAnimation.stride);
            _danglymeshAnimation.stride += danglyMesh->period * strideDir * dt;
            if ((strideDir.x > 0.0f && _danglymeshAnimation.stride.x > 0.0f) || (strideDir.x < 0.0f && _danglymeshAnimation.stride.x < 0.0f)) {
                _danglymeshAnimation.stride.x = 0.0f;
            }
            if ((strideDir.y > 0.0f && _danglymeshAnimation.stride.y > 0.0f) || (strideDir.y < 0.0f && _danglymeshAnimation.stride.y < 0.0f)) {
                _danglymeshAnimation.stride.y = 0.0f;
            }
            if ((strideDir.z > 0.0f && _danglymeshAnimation.stride.z > 0.0f) || (strideDir.z < 0.0f && _danglymeshAnimation.stride.z < 0.0f)) {
                _danglymeshAnimation.stride.z = 0.0f;
            }
        }
    }
}

bool MeshSceneNode::shouldRender() const {
    if (g_debugWalkmesh) {
        return _modelNode->isAABBMesh();
    }
    shared_ptr<ModelNode::TriangleMesh> mesh(_modelNode->mesh());
    if (!mesh || !mesh->render || _modelNode->alpha().getByFrameOrElse(0, 1.0f) == 0.0f) {
        return false;
    }
    return !_modelNode->isAABBMesh();
}

bool MeshSceneNode::shouldCastShadows() const {
    shared_ptr<ModelNode::TriangleMesh> mesh(_modelNode->mesh());
    if (!mesh) {
        return false;
    }
    if (_model.usage() == ModelUsage::Creature) {
        return mesh->shadow && !_modelNode->isSkinMesh();
    } else {
        return false;
    }
}

bool MeshSceneNode::isTransparent() const {
    shared_ptr<ModelNode::TriangleMesh> mesh(_modelNode->mesh());
    if (!mesh) {
        return false; // Meshless nodes are opaque
    }

    // Character models are opaque
    if (_model.model().classification() == MdlClassification::character) {
        return false;
    }

    // Model nodes with alpha less than 1.0 are transparent
    if (_alpha < 1.0f) {
        return true;
    }

    // Model nodes without a diffuse texture are opaque
    if (!_nodeTextures.diffuse) {
        return false;
    }

    // Model nodes with transparency hint greater than 0 are transparent
    if (mesh->transparency > 0) {
        return true;
    }

    // Model nodes with additive diffuse texture are opaque
    if (_nodeTextures.diffuse->isAdditive()) {
        return true;
    }

    // Model nodes with an environment map or a bump map are opaque
    if (_nodeTextures.envmap || _nodeTextures.bumpmap) {
        return false;
    }

    // Model nodes with RGB diffuse textures are opaque
    PixelFormat format = _nodeTextures.diffuse->pixelFormat();
    if (format == PixelFormat::RGB || format == PixelFormat::BGR || format == PixelFormat::DXT1) {
        return false;
    }

    return true;
}

static bool isLightingEnabledByUsage(ModelUsage usage) {
    return usage != ModelUsage::Projectile;
}

bool MeshSceneNode::isSelfIlluminated() const {
    return !_nodeTextures.lightmap && glm::dot(_selfIllumColor, _selfIllumColor) > 0.0f;
}

static bool isReceivingShadows(const ModelSceneNode &model, const MeshSceneNode &modelNode) {
    // Only room models receive shadows, unless model node is self-illuminated
    return model.usage() == ModelUsage::Room && !modelNode.isSelfIlluminated();
}

void MeshSceneNode::draw() {
    shared_ptr<ModelNode::TriangleMesh> mesh(_modelNode->mesh());
    if (!mesh) {
        return;
    }

    // Setup shaders

    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetLocals();
    uniforms.general.model = _absTransform;
    uniforms.general.alpha = _alpha;

    auto &program = _nodeTextures.diffuse ? _shaders.blinnPhong() : _shaders.blinnPhongDiffuseless();

    if (_nodeTextures.diffuse) {
        uniforms.general.featureMask |= UniformsFeatureFlags::diffuse;
    }
    if (_nodeTextures.envmap) {
        uniforms.general.featureMask |= UniformsFeatureFlags::envmap;
    }
    if (_nodeTextures.lightmap) {
        uniforms.general.featureMask |= UniformsFeatureFlags::lightmap;
    }
    if (_nodeTextures.bumpmap) {
        if (_nodeTextures.bumpmap->isGrayscale()) {
            uniforms.general.featureMask |= UniformsFeatureFlags::heightmap;
            uniforms.general.heightMapScaling = _nodeTextures.bumpmap->features().bumpMapScaling;
            if (_nodeTextures.bumpmap->features().procedureType == Texture::ProcedureType::Cycle) {
                int gridX = _nodeTextures.bumpmap->features().numX;
                int gridY = _nodeTextures.bumpmap->features().numY;
                float oneOverGridX = 1.0f / static_cast<float>(gridX);
                float oneOverGridY = 1.0f / static_cast<float>(gridY);
                uniforms.general.heightMapFrameBounds = glm::vec4(
                    oneOverGridX * (_bumpmapCycleFrame % gridX),
                    oneOverGridY * (_bumpmapCycleFrame / gridX),
                    oneOverGridX,
                    oneOverGridY);
            } else {
                uniforms.general.heightMapFrameBounds = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
            }
        } else {
            uniforms.general.featureMask |= UniformsFeatureFlags::normalmap;
        }
    }

    bool receivesShadows = isReceivingShadows(_model, *this);
    if (receivesShadows && _sceneGraph.hasShadowLight()) {
        uniforms.general.featureMask |= UniformsFeatureFlags::shadows;
    }

    if (mesh->skin) {
        uniforms.general.featureMask |= UniformsFeatureFlags::skeletal;

        // Offset bone matrices by 1 to account for negative bone indices
        uniforms.skeletal.bones[0] = glm::mat4(1.0f);
        for (size_t i = 1; i < kMaxBones; ++i) {
            glm::mat4 tmp(1.0f);
            if (i < 1 + mesh->skin->boneNodeNumber.size()) {
                uint16_t nodeNumber = mesh->skin->boneNodeNumber[i - 1];
                if (nodeNumber != 0xffff) {
                    shared_ptr<ModelNodeSceneNode> bone(_model.getNodeByNumber(nodeNumber));
                    if (bone && bone->type() == SceneNodeType::Mesh) {
                        tmp = _modelNode->absoluteTransformInverse() *
                              _model.absoluteTransformInverse() *
                              bone->absoluteTransform() *
                              mesh->skin->boneMatrices[mesh->skin->boneSerial[i - 1]];
                    }
                }
            }
            uniforms.skeletal.bones[i] = move(tmp);
        }
    }

    if (isSelfIlluminated()) {
        uniforms.general.featureMask |= UniformsFeatureFlags::selfIllum;
        uniforms.general.selfIllumColor = glm::vec4(_selfIllumColor, 1.0f);
    }
    if (isLightingEnabled()) {
        const vector<LightSceneNode *> &lights = _sceneGraph.activeLights();

        uniforms.general.featureMask |= UniformsFeatureFlags::lighting;
        uniforms.general.ambientColor = glm::vec4(mesh->ambient, 1.0f);
        uniforms.general.diffuseColor = glm::vec4(mesh->diffuse, 1.0f);
        uniforms.lighting.lightCount = static_cast<int>(lights.size());

        for (size_t i = 0; i < lights.size(); ++i) {
            glm::vec4 position(lights[i]->absoluteTransform()[3]);
            position.w = lights[i]->isDirectional() ? 0.0f : 1.0f;

            LightUniforms &shaderLight = uniforms.lighting.lights[i];
            shaderLight.position = move(position);
            shaderLight.color = glm::vec4(lights[i]->color(), 1.0f);
            shaderLight.multiplier = lights[i]->multiplier() * (1.0f - lights[i]->fadeFactor());
            shaderLight.radius = lights[i]->radius();
            shaderLight.ambientOnly = static_cast<int>(lights[i]->modelNode().light()->ambientOnly);
        }
    }

    if (_nodeTextures.diffuse) {
        uniforms.general.uv = glm::mat3x4(
            glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
            glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
            glm::vec4(_uvOffset.x, _uvOffset.y, 0.0f, 0.0f));
        float waterAlpha = _nodeTextures.diffuse->features().waterAlpha;
        if (waterAlpha != -1.0f) {
            uniforms.general.featureMask |= UniformsFeatureFlags::water;
            uniforms.general.waterAlpha = waterAlpha;
        }
    }

    if (_sceneGraph.isFogEnabled() && _model.model().isAffectedByFog()) {
        uniforms.general.featureMask |= UniformsFeatureFlags::fog;
    }

    auto danglyMesh = mesh->danglyMesh;
    if (danglyMesh) {
        uniforms.general.featureMask |= UniformsFeatureFlags::danglymesh;
        uniforms.danglymesh.stride = glm::vec4(_danglymeshAnimation.stride, 0.0f);
        uniforms.danglymesh.displacement = danglyMesh->displacement;
        for (size_t i = 0; i < danglyMesh->constraints.size(); ++i) {
            uniforms.danglymesh.constraints[i / 4][i % 4] = danglyMesh->constraints[i].multiplier;
        }
    }

    _shaders.use(program, true);

    bool additive = false;

    // Setup textures

    if (_nodeTextures.diffuse) {
        _textures.bind(*_nodeTextures.diffuse, TextureUnits::diffuseMap);
        additive = _nodeTextures.diffuse->isAdditive();
    }
    if (_nodeTextures.lightmap) {
        _textures.bind(*_nodeTextures.lightmap, TextureUnits::lightmap);
    }
    if (_nodeTextures.envmap) {
        _textures.bind(*_nodeTextures.envmap, TextureUnits::environmentMap);
    }
    if (_nodeTextures.bumpmap) {
        _textures.bind(*_nodeTextures.bumpmap, TextureUnits::bumpMap);
    }

    auto blendMode = additive ? BlendMode::Add : BlendMode::Default;
    _graphicsContext.withBlending(blendMode, [this, &mesh]() {
        mesh->mesh->draw();
    });
}

void MeshSceneNode::drawShadow() {
    shared_ptr<ModelNode::TriangleMesh> mesh(_modelNode->mesh());
    if (!mesh) {
        return;
    }
    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetLocals();
    uniforms.general.model = _absTransform;
    uniforms.general.alpha = _alpha;

    auto &program = _sceneGraph.isShadowLightDirectional() ? _shaders.directionalLightShadows() : _shaders.pointLightShadows();
    _shaders.use(program, true);
    mesh->mesh->draw();
}

bool MeshSceneNode::isLightingEnabled() const {
    if (!isLightingEnabledByUsage(_model.usage())) {
        return false;
    }

    // Lighting is disabled for lightmapped models, unless dynamic room lighting is enabled
    if (_nodeTextures.lightmap) {
        return false;
    }

    // Lighting is disabled for self-illuminated model nodes, e.g. sky boxes
    if (isSelfIlluminated()) {
        return false;
    }

    // Lighting is disabled when diffuse texture is additive
    if (_nodeTextures.diffuse && _nodeTextures.diffuse->isAdditive()) {
        return false;
    }

    return true;
}

void MeshSceneNode::setAppliedForce(glm::vec3 force) {
    if (_modelNode->isDanglyMesh()) {
        // Convert force from world to object space
        _danglymeshAnimation.force = _absTransformInv * glm::vec4(force, 0.0f);
    }
}

void MeshSceneNode::setDiffuseTexture(const shared_ptr<Texture> &texture) {
    _nodeTextures.diffuse = texture;
    refreshAdditionalTextures();
}

} // namespace scene

} // namespace reone
