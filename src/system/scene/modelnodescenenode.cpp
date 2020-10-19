/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "modelscenenode.h"
#include "scenegraph.h"

using namespace std;

using namespace reone::render;

namespace reone {

namespace scene {

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

void ModelNodeSceneNode::fillSceneGraph() {
    shared_ptr<ModelMesh> mesh(_modelNode->mesh());
    if (mesh) {
        bool render = mesh->shouldRender() && (mesh->hasDiffuseTexture() || _modelSceneNode->hasTextureOverride());
        if (render) {
            bool transparent = mesh->isTransparent() || _modelNode->alpha() < 1.0f;
            if (transparent) {
                _sceneGraph->addTransparentMesh(this);
            } else {
                _sceneGraph->addOpaqueMesh(this);
            }
        }
    }

    shared_ptr<ModelNode::Light> light(_modelNode->light());
    if (light) {
        _sceneGraph->addLight(this);
    }

    SceneNode::fillSceneGraph();
}

void ModelNodeSceneNode::renderSingle() const {
    shared_ptr<ModelMesh> mesh(_modelNode->mesh());
    if (!mesh) return;

    bool render = mesh->shouldRender() && (mesh->hasDiffuseTexture() || _modelSceneNode->hasTextureOverride());
    if (!render) return;

    shared_ptr<ModelNode::Skin> skin(_modelNode->skin());
    bool skeletal = static_cast<bool>(skin);

    LocalUniforms locals;
    locals.model = _absoluteTransform;
    locals.alpha = _modelSceneNode->alpha() * _modelNode->alpha();

    if (mesh->hasEnvmapTexture()) {
        locals.features.envmapEnabled = true;
        locals.textures.envmap = 1;
    }
    if (mesh->hasLightmapTexture()) {
        locals.features.lightmapEnabled = true;
        locals.textures.lightmap = 2;
    }
    if (mesh->hasBumpyShinyTexture()) {
        locals.features.bumpyShinyEnabled = true;
        locals.textures.bumpyShiny = 3;
    }
    if (mesh->hasBumpmapTexture()) {
        locals.features.bumpmapEnabled = true;
        locals.textures.bumpmap = 4;
    }
    if (skeletal) {
        locals.features.skeletalEnabled = true;
        locals.skeletal.absTransform = _modelNode->absoluteTransform();
        locals.skeletal.absTransformInv = _modelNode->absoluteTransformInverse();

        const unordered_map<uint16_t, uint16_t> &nodeIdxByBoneIdx = skin->nodeIdxByBoneIdx;
        vector<glm::mat4> bones(nodeIdxByBoneIdx.size(), glm::mat4(1.0f));

        for (auto &pair : nodeIdxByBoneIdx) {
            uint16_t boneIdx = pair.first;
            uint16_t nodeIdx = pair.second;

            ModelNodeSceneNode *bone = _modelSceneNode->getModelNodeByIndex(nodeIdx);
            if (bone) {
                bones[boneIdx] = bone->boneTransform();
            }
        }

        locals.skeletal.bones = move(bones);
    }
    if (_modelNode->isSelfIllumEnabled()) {
        locals.features.selfIllumEnabled = true;
        locals.selfIllumColor = _modelNode->selfIllumColor();
    }
    int lightCount = 0;

    if (_modelSceneNode->isLightingEnabled()) {
        const vector<ModelNodeSceneNode *> &lights = _modelSceneNode->lightsAffectedBy();

        locals.features.lightingEnabled = true;
        locals.lighting.ambientColor = _sceneGraph->ambientLightColor();
        locals.lighting.lights.clear();

        for (auto &light : lights) {
            ShaderLight shaderLight;
            shaderLight.position = light->absoluteTransform()[3];
            shaderLight.radius = light->modelNode()->radius();
            shaderLight.color = light->modelNode()->color();

            locals.lighting.lights.push_back(move(shaderLight));
        }
    }
    Shaders.activate(ShaderProgram::ModelModel, locals);

    mesh->render(_modelSceneNode->textureOverride());
}

float ModelNodeSceneNode::getDistanceFromCenter(const glm::vec3 &point) const {
    glm::vec3 center(_absoluteTransform * glm::vec4(_modelNode->getCenterOfAABB(), 1.0f));
    return glm::distance2(center, point);
}

ModelNode *ModelNodeSceneNode::modelNode() const {
    return _modelNode;
}

const glm::mat4 &ModelNodeSceneNode::boneTransform() const {
    return _boneTransform;
}

void ModelNodeSceneNode::setBoneTransform(const glm::mat4 &transform) {
    _boneTransform = transform;
}

} // namespace scene

} // namespace reone
