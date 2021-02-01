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

#include "../scenegraph.h"

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
            const TextureFeatures &features = bumpmap->features();
            if (features.procedureType == TextureProcedureType::Cycle) {
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
    return mesh && mesh->shouldRender() && (mesh->hasDiffuseTexture() || _modelSceneNode->hasTextureOverride());
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

void ModelNodeSceneNode::renderSingle(bool shadowPass) const {
    shared_ptr<ModelMesh> mesh(_modelNode->mesh());
    if (!mesh) return;

    shared_ptr<Texture> diffuseTexture;
    if (_modelSceneNode->hasTextureOverride()) {
        diffuseTexture = _modelSceneNode->textureOverride();
    }
    if (!diffuseTexture) {
        diffuseTexture = mesh->diffuseTexture();
    }

    LocalUniforms locals;
    locals.general.model = _absoluteTransform;
    locals.general.alpha = _modelSceneNode->alpha() * _modelNode->alpha();

    if (!shadowPass) {
        if (mesh->hasEnvmapTexture()) {
            locals.general.envmapEnabled = true;
        }
        if (mesh->hasLightmapTexture()) {
            locals.general.lightmapEnabled = true;
        }
        if (mesh->hasBumpyShinyTexture()) {
            locals.general.bumpyShinyEnabled = true;
        }
        shared_ptr<Texture> bumpmapTexture(mesh->bumpmapTexture());
        if (bumpmapTexture) {
            locals.general.bumpmapEnabled = true;
            locals.bumpmap.grayscale = bumpmapTexture->isGrayscale();
            locals.bumpmap.scaling = bumpmapTexture->features().bumpMapScaling;
            locals.bumpmap.gridSize = glm::vec2(bumpmapTexture->features().numX, bumpmapTexture->features().numY);
            locals.bumpmap.frame = _bumpmapFrame;
            locals.bumpmap.fromTOR = mesh->isBumpmapFromTOR();
        }

        bool receivesShadows =
            _modelSceneNode->model()->classification() == Model::Classification::Other &&
            !_modelNode->isSelfIllumEnabled();

        if (receivesShadows) {
            locals.general.shadowsEnabled = true;
        }

        shared_ptr<ModelNode::Skin> skin(_modelNode->skin());
        if (skin) {
            locals.general.skeletalEnabled = true;
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
                    locals.skeletal->bones[boneIdx] = bone->boneTransform();
                }
            }
        }
        if (_modelNode->isSelfIllumEnabled()) {
            locals.general.selfIllumEnabled = true;
            locals.general.selfIllumColor = glm::vec4(_modelNode->selfIllumColor(), 1.0f);
        }
        if (_modelSceneNode->isLightingEnabled() &&
            !mesh->hasLightmapTexture() &&
            !_modelNode->isSelfIllumEnabled() &&
            (!diffuseTexture || !diffuseTexture->isAdditive())) {

            const vector<LightSceneNode *> &lights = _modelSceneNode->lightsAffectedBy();

            locals.general.lightingEnabled = true;
            locals.lighting = Shaders::instance().lightingUniforms();
            locals.lighting->meshDiffuseColor = glm::vec4(mesh->diffuseColor(), 1.0f);
            locals.lighting->meshAmbientColor = glm::vec4(mesh->ambientColor(), 1.0f);
            locals.lighting->ambientLightColor = glm::vec4(_sceneGraph->ambientLightColor(), 1.0f);
            locals.lighting->lightCount = static_cast<int>(lights.size());

            for (int i = 0; i < locals.lighting->lightCount; ++i) {
                ShaderLight &shaderLight = locals.lighting->lights[i];
                shaderLight.position = lights[i]->absoluteTransform()[3];
                shaderLight.color = glm::vec4(lights[i]->color(), 1.0f);
                shaderLight.radius = lights[i]->radius();
                shaderLight.multiplier = lights[i]->multiplier();
            }
        }
        if (diffuseTexture) {
            float waterAlpha = diffuseTexture->features().waterAlpha;
            locals.general.uvOffset = _uvOffset;
            locals.general.water = waterAlpha == -1.0f ? 0 : 1;
            locals.general.waterAlpha = waterAlpha;
        }
    }

    ShaderProgram program = shadowPass ? ShaderProgram::DepthDepth : ShaderProgram::ModelModel;
    Shaders::instance().activate(program, locals);

    mesh->render(diffuseTexture);
}

float ModelNodeSceneNode::distanceTo(const glm::vec3 &point) const {
    glm::vec3 origin(_absoluteTransform * glm::vec4(_modelNode->getCenterOfAABB(), 1.0f));
    return glm::distance(origin, point);
}

void ModelNodeSceneNode::setBoneTransform(const glm::mat4 &transform) {
    _boneTransform = transform;
}

} // namespace scene

} // namespace reone
