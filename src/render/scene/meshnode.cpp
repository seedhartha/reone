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

#include "meshnode.h"

#include <stdexcept>
#include <unordered_map>

#include <boost/format.hpp>

#include "modelnode.h"
#include "scenegraph.h"

using namespace std;

namespace reone {

namespace render {

MeshSceneNode::MeshSceneNode(SceneGraph *sceneGraph, const ModelSceneNode *model, const ModelNode *modelNode) :
    SceneNode(sceneGraph), _model(model), _modelNode(modelNode) {

    assert(_model && _modelNode);
}

void MeshSceneNode::fillSceneGraph() {
    if (isTransparent()) {
        _sceneGraph->addTransparentMesh(this);
    } else {
        _sceneGraph->addOpaqueMesh(this);
    }
    SceneNode::fillSceneGraph();
}

bool MeshSceneNode::isTransparent() const {
    shared_ptr<ModelMesh> mesh(_modelNode->mesh());
    return (mesh && mesh->isTransparent()) || _modelNode->alpha() < 1.0f;
}

void MeshSceneNode::updateDistanceToCamera(const glm::vec3 &cameraPosition) {
    _distanceToCamera = glm::distance2(_center, cameraPosition);
}

void MeshSceneNode::render() const {
    shared_ptr<ModelMesh> mesh(_modelNode->mesh());
    shared_ptr<ModelNode::Skin> skin(_modelNode->skin());
    const ModelSceneNode::AnimationState &animState = _model->animationState();
    bool skeletal = skin && !animState.name.empty();

    LocalUniforms locals;
    locals.model = _absoluteTransform;
    locals.alpha = _model->alpha() * _modelNode->alpha();

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

            auto bone = animState.boneTransforms.find(nodeIdx);
            if (bone == animState.boneTransforms.end()) continue;

            bones[boneIdx] = bone->second;
        }

        locals.skeletal.bones = move(bones);
    }
    if (_modelNode->isSelfIllumEnabled()) {
        locals.features.selfIllumEnabled = true;
        locals.selfIllumColor = _modelNode->selfIllumColor();
    }
    int lightCount = 0;

    if (_model->isLightingEnabled()) {
        const vector<LightSceneNode *> &lights = _model->lightsAffectedBy();

        locals.features.lightingEnabled = true;
        locals.lighting.ambientColor = _sceneGraph->ambientLightColor();
        locals.lighting.lights.clear();

        for (auto &light : lights) {
            ShaderLight shaderLight;
            shaderLight.position = light->absoluteTransform()[3];
            shaderLight.radius = light->modelNode().radius();
            shaderLight.color = light->modelNode().color();

            locals.lighting.lights.push_back(move(shaderLight));
        }
    }
    Shaders.activate(ShaderProgram::ModelModel, locals);

    mesh->render(_model->textureOverride());

    SceneNode::render();
}

const ModelSceneNode *MeshSceneNode::model() const {
    return _model;
}

const ModelNode *MeshSceneNode::modelNode() const {
    return _modelNode;
}

float MeshSceneNode::distanceToCamera() const {
    return _distanceToCamera;
}

void MeshSceneNode::updateAbsoluteTransform() {
    SceneNode::updateAbsoluteTransform();
    updateCenter();
}

void MeshSceneNode::updateCenter() {
    _center = _absoluteTransform * glm::vec4(_modelNode->getCenterOfAABB(), 1.0f);
}

} // namespace render

} // namespace reone
