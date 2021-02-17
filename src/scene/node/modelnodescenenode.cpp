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

#include "modelnodescenenode.h"

#include <stdexcept>

#include "../../common/log.h"
#include "../../common/random.h"
#include "../../render/featureutil.h"
#include "../../render/pbribl.h"

#include "../scenegraph.h"

#include "cameranode.h"
#include "lightnode.h"
#include "modelscenenode.h"

using namespace std;

using namespace reone::render;

namespace reone {

namespace scene {

static constexpr float kUvAnimationSpeed = 250.0f;

ModelNodeSceneNode::ModelNodeSceneNode(SceneGraph *sceneGraph, const ModelSceneNode *modelSceneNode, ModelNode *modelNode) :
    SceneNode(sceneGraph),
    _modelSceneNode(modelSceneNode),
    _modelNode(modelNode) {

    if (!modelSceneNode) {
        throw invalid_argument("modelSceneNode must not be null");
    }
    if (!modelNode) {
        throw invalid_argument("modelNode must not be null");
    }
}

void ModelNodeSceneNode::update(float dt) {
    shared_ptr<ModelMesh> mesh(_modelNode->mesh());
    if (mesh) {
        // UV animation
        const ModelMesh::UVAnimation &uvAnimation = mesh->uvAnimation();
        if (uvAnimation.animated) {
            glm::vec2 dir(uvAnimation.directionX, uvAnimation.directionY);
            _uvOffset += kUvAnimationSpeed * dir * dt;
            _uvOffset -= glm::floor(_uvOffset);
        }

        // Bumpmap UV animation
        shared_ptr<Texture> bumpmap(mesh->bumpmapTexture());
        if (bumpmap) {
            const Texture::Features &features = bumpmap->features();
            if (features.procedureType == Texture::ProcedureType::Cycle) {
                int frameCount = features.numX * features.numY;
                float length = frameCount / static_cast<float>(features.fps);
                _bumpmapTime = glm::min(_bumpmapTime + dt, length);
                _bumpmapFrame = glm::round((frameCount - 1) * (_bumpmapTime / length));
                if (_bumpmapTime == length) {
                    _bumpmapTime = 0.0f;
                }
            }
        }
    }
    SceneNode::update(dt);
}

bool ModelNodeSceneNode::shouldRender() const {
    shared_ptr<ModelMesh> mesh(_modelNode->mesh());
    return mesh && mesh->shouldRender();
}

bool ModelNodeSceneNode::shouldCastShadows() const {
    if (_modelSceneNode->classification() == ModelSceneNode::Classification::Door) return false;

    shared_ptr<ModelMesh> mesh(_modelNode->mesh());

    return mesh && mesh->shouldCastShadows() && !static_cast<bool>(_modelNode->skin());
}

bool ModelNodeSceneNode::isTransparent() const {
    shared_ptr<ModelMesh> mesh(_modelNode->mesh());
    if (!mesh || _modelSceneNode->model()->classification() == Model::Classification::Character) return false;

    return mesh->isTransparent() || _modelNode->alpha() < 1.0f;
}

static bool isLightingEnabledByClassification(ModelSceneNode::Classification classification) {
    if (classification == ModelSceneNode::Classification::Room) {
        return isFeatureEnabled(Feature::DynamicRoomLighting);
    }
    return classification != ModelSceneNode::Classification::Projectile;
}

static bool isLightingEnabled(const ModelSceneNode &model, const ModelNode &modelNode, const shared_ptr<Texture> &diffuse) {
    if (!isLightingEnabledByClassification(model.classification())) return false;

    // Lighting is disabled for lightmapped models, unless dynamic room lighting is enabled
    if (modelNode.mesh()->hasLightmapTexture() && !isFeatureEnabled(Feature::DynamicRoomLighting)) return false;

    // Lighting is disabled for self-illuminated room model nodes, e.g. the skybox
    if (modelNode.isSelfIllumEnabled() && model.classification() == ModelSceneNode::Classification::Room) return false;

    // Lighting is disabled when diffuse texture is additive
    if (diffuse && diffuse->isAdditive()) return false;

    return true;
}

static bool isReceivingShadows(const ModelSceneNode &modelSceneNode, const ModelNode &modelNode) {
    // Only room models receive shadows, unless model node is self-illuminated
    return
        modelSceneNode.classification() == ModelSceneNode::Classification::Room &&
        !modelNode.isSelfIllumEnabled();
}

void ModelNodeSceneNode::renderSingle(bool shadowPass) {
    shared_ptr<ModelMesh> mesh(_modelNode->mesh());
    if (!mesh) return;

    shared_ptr<Texture> diffuseTexture(_modelSceneNode->textureOverride());
    if (!diffuseTexture) {
        diffuseTexture = mesh->diffuseTexture();
    }

    ShaderUniforms uniforms(_sceneGraph->uniformsPrototype());
    if (isFeatureEnabled(Feature::HDR)) {
        uniforms.general.featureMask |= UniformFeatureFlags::hdr;
    }
    uniforms.general.model = _absoluteTransform;
    uniforms.general.alpha = _modelSceneNode->alpha() * _modelNode->alpha();
    uniforms.general.ambientColor = glm::vec4(_sceneGraph->ambientLightColor(), 1.0f);
    uniforms.general.exposure = _sceneGraph->exposure();

    ShaderProgram program;

    if (shadowPass) {
        program = ShaderProgram::SimpleDepth;

    } else {
        program = isFeatureEnabled(Feature::PBR) ? ShaderProgram::ModelPBR : ShaderProgram::ModelBlinnPhong;

        if (diffuseTexture) {
            uniforms.general.featureMask |= UniformFeatureFlags::diffuse;
        }

        if (mesh->hasEnvmapTexture()) {
            uniforms.general.featureMask |= UniformFeatureFlags::envmap;

            if (isFeatureEnabled(Feature::PBR)) {
                bool derived = PBRIBL::instance().contains(mesh->envmapTexture().get());
                if (derived) {
                    uniforms.general.featureMask |= UniformFeatureFlags::pbrIbl;
                }
            }
        }

        if (mesh->hasLightmapTexture() && !isFeatureEnabled(Feature::DynamicRoomLighting)) {
            uniforms.general.featureMask |= UniformFeatureFlags::lightmap;
        }

        shared_ptr<Texture> bumpmapTexture(mesh->bumpmapTexture());
        if (bumpmapTexture) {
            uniforms.general.featureMask |= UniformFeatureFlags::bumpmap;
            uniforms.bumpmap.grayscale = bumpmapTexture->isGrayscale();
            uniforms.bumpmap.scaling = bumpmapTexture->features().bumpMapScaling;
            uniforms.bumpmap.gridSize = glm::vec2(bumpmapTexture->features().numX, bumpmapTexture->features().numY);
            uniforms.bumpmap.frame = _bumpmapFrame;
            uniforms.bumpmap.swizzled = mesh->isBumpmapSwizzled();
        }

        bool receivesShadows = isReceivingShadows(*_modelSceneNode, *_modelNode);
        if (receivesShadows) {
            uniforms.general.featureMask |= UniformFeatureFlags::shadows;
        }

        shared_ptr<ModelNode::Skin> skin(_modelNode->skin());
        if (skin) {
            uniforms.general.featureMask |= UniformFeatureFlags::skeletal;

            for (int i = 0; i < kMaxBoneCount; ++i) {
                uniforms.skeletal.bones[i] = glm::mat4(1.0f);
            }
            for (auto &pair : skin->nodeIdxByBoneIdx) {
                uint16_t boneIdx = pair.first;
                uint16_t nodeIdx = pair.second;

                ModelNodeSceneNode *bone = _modelSceneNode->getModelNodeByIndex(nodeIdx);
                if (bone) {
                    uniforms.skeletal.bones[boneIdx] = _modelNode->absoluteTransformInverse() * bone->boneTransform() * _modelNode->absoluteTransform();
                }
            }
        }

        if (_modelNode->isSelfIllumEnabled()) {
            uniforms.general.featureMask |= UniformFeatureFlags::selfIllum;
            uniforms.general.selfIllumColor = glm::vec4(_modelNode->selfIllumColor(), 1.0f);
        }
        if (isLightingEnabled(*_modelSceneNode, *_modelNode, diffuseTexture)) {
            const vector<LightSceneNode *> &lights = _modelSceneNode->lightsAffectedBy();

            uniforms.general.featureMask |= UniformFeatureFlags::lighting;
            if (mesh->material().custom) {
                uniforms.general.featureMask |= UniformFeatureFlags::customMat;
            }
            uniforms.lighting.materialAmbient = glm::vec4(mesh->ambientColor(), 1.0f);
            uniforms.lighting.materialDiffuse = glm::vec4(mesh->diffuseColor(), 1.0f);
            uniforms.lighting.materialSpecular = mesh->material().specular;
            uniforms.lighting.materialShininess = mesh->material().shininess;
            uniforms.lighting.materialMetallic = mesh->material().metallic;
            uniforms.lighting.materialRoughness = mesh->material().roughness;
            uniforms.lighting.lightCount = static_cast<int>(lights.size());

            for (int i = 0; i < uniforms.lighting.lightCount; ++i) {
                ShaderLight &shaderLight = uniforms.lighting.lights[i];
                shaderLight.position = lights[i]->absoluteTransform()[3];
                shaderLight.color = glm::vec4(lights[i]->color(), 1.0f);
                shaderLight.multiplier = lights[i]->multiplier();
                shaderLight.radius = lights[i]->radius();
            }
        }

        if (diffuseTexture) {
            uniforms.general.uvOffset = _uvOffset;

            float waterAlpha = diffuseTexture->features().waterAlpha;
            if (waterAlpha != -1.0f) {
                uniforms.general.featureMask |= UniformFeatureFlags::water;
                uniforms.general.waterAlpha = waterAlpha;
            }
        }
    }

    Shaders::instance().activate(program, uniforms);

    mesh->render(diffuseTexture);
}

glm::vec3 ModelNodeSceneNode::getOrigin() const {
    return _absoluteTransform * glm::vec4(_modelNode->getCenterOfAABB(), 1.0f);
}

void ModelNodeSceneNode::setBoneTransform(const glm::mat4 &transform) {
    _boneTransform = transform;
}

} // namespace scene

} // namespace reone
