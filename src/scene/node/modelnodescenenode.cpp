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
#include "../../render/pbribl.h"

#include "../scenegraph.h"

#include "lightnode.h"
#include "modelscenenode.h"

using namespace std;

using namespace reone::render;

namespace reone {

namespace scene {

static bool g_pbrEnabled = false;

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
    shared_ptr<ModelMesh> mesh(_modelNode->mesh());
    return mesh && mesh->shouldCastShadows() && !static_cast<bool>(_modelNode->skin());
}

bool ModelNodeSceneNode::isTransparent() const {
    shared_ptr<ModelMesh> mesh(_modelNode->mesh());
    if (!mesh || _modelSceneNode->model()->classification() == Model::Classification::Character) return false;

    return mesh->isTransparent() || _modelNode->alpha() < 1.0f;
}

static bool isLightDirectional(const LightSceneNode &light) {
    // Consider all lights with a radius of 50.0 and more directional
    return light.radius() >= 50.0f;
}

void ModelNodeSceneNode::renderSingle(bool shadowPass) const {
    shared_ptr<ModelMesh> mesh(_modelNode->mesh());
    if (!mesh) return;

    shared_ptr<Texture> diffuseTexture(_modelSceneNode->textureOverride());
    if (!diffuseTexture) {
        diffuseTexture = mesh->diffuseTexture();
    }

    LocalUniforms locals;
    locals.general.model = _absoluteTransform;
    locals.general.alpha = _modelSceneNode->alpha() * _modelNode->alpha();

    ShaderProgram program;

    if (shadowPass) {
        program = ShaderProgram::SimpleDepth;

    } else {
        program = g_pbrEnabled ? ShaderProgram::ModelPBR : ShaderProgram::ModelBlinnPhong;

        if (diffuseTexture) {
            locals.general.featureMask |= UniformFeatureFlags::diffuse;
        }

        if (mesh->hasEnvmapTexture()) {
            locals.general.featureMask |= UniformFeatureFlags::envmap;

            if (g_pbrEnabled) {
                bool derived = PBRIBL::instance().contains(mesh->envmapTexture().get());
                if (derived) {
                    locals.general.featureMask |= UniformFeatureFlags::pbrIbl;
                }
            }
        }

        if (mesh->hasLightmapTexture()) {
            locals.general.featureMask |= UniformFeatureFlags::lightmap;
        }

        shared_ptr<Texture> bumpmapTexture(mesh->bumpmapTexture());
        if (bumpmapTexture) {
            locals.general.featureMask |= UniformFeatureFlags::bumpmap;
            locals.bumpmap.grayscale = bumpmapTexture->isGrayscale();
            locals.bumpmap.scaling = bumpmapTexture->features().bumpMapScaling;
            locals.bumpmap.gridSize = glm::vec2(bumpmapTexture->features().numX, bumpmapTexture->features().numY);
            locals.bumpmap.frame = _bumpmapFrame;
            locals.bumpmap.swizzled = mesh->isBumpmapSwizzled();
        }

        bool receivesShadows =
            _modelSceneNode->model()->classification() == Model::Classification::Other &&
            !_modelNode->isSelfIllumEnabled();

        if (receivesShadows) {
            locals.general.featureMask |= UniformFeatureFlags::shadows;
        }

        shared_ptr<ModelNode::Skin> skin(_modelNode->skin());
        if (skin) {
            locals.general.featureMask |= UniformFeatureFlags::skeletal;
            locals.skeletal = Shaders::instance().skeletalUniforms();
            locals.skeletal->absTransform = _modelNode->absoluteTransform();
            locals.skeletal->absTransformInv = _modelNode->absoluteTransformInverse();

            for (int i = 0; i < kMaxBoneCount; ++i) {
                locals.skeletal->bones[i] = glm::mat4(1.0f);
            }
            for (auto &pair : skin->nodeIdxByBoneIdx) {
                uint16_t boneIdx = pair.first;
                uint16_t nodeIdx = pair.second;

                ModelNodeSceneNode *bone = _modelSceneNode->getModelNodeByIndex(nodeIdx);
                if (bone) {
                    locals.skeletal->bones[boneIdx] = _modelNode->absoluteTransformInverse() * bone->boneTransform() * _modelNode->absoluteTransform();
                }
            }
        }

        if (_modelNode->isSelfIllumEnabled()) {
            locals.general.featureMask |= UniformFeatureFlags::selfIllum;
            locals.general.selfIllumColor = glm::vec4(_modelNode->selfIllumColor(), 1.0f);
        }
        if (
            _modelSceneNode->isLightingEnabled() &&
            !_modelNode->isSelfIllumEnabled() &&
            !mesh->hasLightmapTexture() &&
            (!diffuseTexture || !diffuseTexture->isAdditive())) {

            const vector<LightSceneNode *> &lights = _modelSceneNode->lightsAffectedBy();

            locals.general.featureMask |= UniformFeatureFlags::lighting;
            locals.lighting = Shaders::instance().lightingUniforms();
            locals.lighting->ambientLightColor = glm::vec4(_sceneGraph->ambientLightColor(), 1.0f);
            locals.lighting->materialAmbient = glm::vec4(mesh->ambientColor(), 1.0f);
            locals.lighting->materialDiffuse = glm::vec4(mesh->diffuseColor(), 1.0f);
            locals.lighting->materialSpecular = mesh->material().specular;
            locals.lighting->materialShininess = mesh->material().shininess;
            locals.lighting->materialMetallic = mesh->material().metallic;
            locals.lighting->materialRoughness = mesh->material().roughness;
            locals.lighting->lightCount = static_cast<int>(lights.size());

            for (int i = 0; i < locals.lighting->lightCount; ++i) {
                ShaderLight &shaderLight = locals.lighting->lights[i];
                shaderLight.position = glm::vec4(glm::vec3(lights[i]->absoluteTransform()[3]), isLightDirectional(*lights[i]) ? 0.0f : 1.0f);
                shaderLight.color = glm::vec4(lights[i]->color(), 1.0f);
                shaderLight.radius = lights[i]->radius();
                shaderLight.multiplier = lights[i]->multiplier();
            }
        }

        if (diffuseTexture) {
            float waterAlpha = diffuseTexture->features().waterAlpha;
            locals.general.uvOffset = _uvOffset;
            if (waterAlpha != -1.0f) {
                locals.general.featureMask |= UniformFeatureFlags::water;
            }
            locals.general.waterAlpha = waterAlpha;
        }
    }

    Shaders::instance().activate(program, locals);

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
