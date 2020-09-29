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

static const string &getLightUniformName(int index, const char *propName) {
    static unordered_map<int, unordered_map<const char *, string>> cache;
    auto &cacheByIndex = cache[index];

    auto nameIt = cacheByIndex.find(propName);
    if (nameIt != cacheByIndex.end()) {
        return nameIt->second;
    }

    string name(str(boost::format("lights[%d].%s") % index % propName));
    auto pair = cacheByIndex.insert(make_pair(propName, name));

    return pair.first->second;
}

void MeshSceneNode::render() const {
    shared_ptr<ModelMesh> mesh(_modelNode->mesh());
    shared_ptr<ModelNode::Skin> skin(_modelNode->skin());
    const ModelSceneNode::AnimationState &animState = _model->animationState();
    bool skeletal = skin && !animState.name.empty();

    ShaderManager &shaders = Shaders;
    shaders.activate(ShaderProgram::ModelModel);
    shaders.setUniform("model", _absoluteTransform);
    shaders.setUniform("alpha", _model->alpha() * _modelNode->alpha());

    if (mesh->hasEnvmapTexture()) {
        shaders.setUniform("envmapEnabled", true);
        shaders.setUniform("envmap", 1);
    }
    if (mesh->hasLightmapTexture()) {
        shaders.setUniform("lightmapEnabled", true);
        shaders.setUniform("lightmap", 2);
    }
    if (mesh->hasBumpyShinyTexture()) {
        shaders.setUniform("bumpyShinyEnabled", true);
        shaders.setUniform("bumpyShiny", 3);
    }
    if (mesh->hasBumpmapTexture()) {
        shaders.setUniform("bumpmapEnabled", true);
        shaders.setUniform("bumpmap", 4);
    }

    if (skeletal) {
        shaders.setUniform("skeletalEnabled", true);
        shaders.setUniform("absTransform", _modelNode->absoluteTransform());
        shaders.setUniform("absTransformInv", _modelNode->absoluteTransformInverse());

        const unordered_map<uint16_t, uint16_t> &nodeIdxByBoneIdx = skin->nodeIdxByBoneIdx;
        vector<glm::mat4> bones(nodeIdxByBoneIdx.size(), glm::mat4(1.0f));

        for (auto &pair : nodeIdxByBoneIdx) {
            uint16_t boneIdx = pair.first;
            uint16_t nodeIdx = pair.second;

            auto bone = animState.boneTransforms.find(nodeIdx);
            if (bone == animState.boneTransforms.end()) continue;

            bones[boneIdx] = bone->second;
        }

        shaders.setUniform("bones", bones);
    }

    if (_modelNode->isSelfIllumEnabled()) {
        shaders.setUniform("selfIllumEnabled", true);
        shaders.setUniform("selfIllumColor", _modelNode->selfIllumColor());
    }

    int lightCount = 0;

    if (_model->isLightingEnabled()) {
        const vector<LightSceneNode *> &lights = _model->lightsAffectedBy();
        lightCount = static_cast<int>(lights.size());

        shaders.setUniform("lightingEnabled", true);
        shaders.setUniform("lightCount", lightCount);
        shaders.setUniform("ambientLightColor", _sceneGraph->ambientLightColor());

        for (int i = 0; i < lightCount; ++i) {
            LightSceneNode *light = lights[i];
            shaders.setUniform(getLightUniformName(i, "ambientOnly"), light->modelNode().light()->ambientOnly);
            shaders.setUniform(getLightUniformName(i, "position"), glm::vec3(light->absoluteTransform()[3]));
            shaders.setUniform(getLightUniformName(i, "radius"), light->modelNode().radius());
            shaders.setUniform(getLightUniformName(i, "color"), light->modelNode().color());
            shaders.setUniform(getLightUniformName(i, "multiplier"), light->modelNode().multiplier());
        }
    }

    mesh->render(_model->textureOverride());

    if (skeletal) {
        shaders.setUniform("skeletalEnabled", false);
    }
    if (_model->isLightingEnabled()) {
        shaders.setUniform("lightingEnabled", false);
    }
    if (_modelNode->isSelfIllumEnabled()) {
        shaders.setUniform("selfIllumEnabled", false);
    }
    if (mesh->hasEnvmapTexture()) {
        shaders.setUniform("envmapEnabled", false);
    }
    if (mesh->hasLightmapTexture()) {
        shaders.setUniform("lightmapEnabled", false);
    }
    if (mesh->hasBumpyShinyTexture()) {
        shaders.setUniform("bumpyShinyEnabled", false);
    }
    if (mesh->hasBumpmapTexture()) {
        shaders.setUniform("bumpmapEnabled", false);
    }

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
