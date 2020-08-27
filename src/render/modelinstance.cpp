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

#include <stack>
#include <stdexcept>

#include "GL/glew.h"

#include "SDL2/SDL_opengl.h"

#include "../core/log.h"
#include "../resources/resources.h"

#include "mesh/aabb.h"
#include "scene/modelnode.h"
#include "scene/scenegraph.h"

#include "modelinstance.h"

using namespace std;

using namespace reone::resources;

namespace reone {

namespace render {

ModelInstance::ModelInstance(const shared_ptr<Model> &model) : _model(model) {
    assert(_model);
}

void ModelInstance::animate(const string &parent, const string &anim, int flags, float speed) {
    if (!_model) return;

    shared_ptr<ModelNode> node(_model->findNodeByName(parent));
    if (!node) {
        warn("ModelInstance: node not found: " + parent);
        return;
    }

    auto attached = _attachedModels.find(node->nodeNumber());
    if (attached == _attachedModels.end()) {
        warn("ModelInstance: attached model not found: " + to_string(node->nodeNumber()));
        return;
    }

    attached->second->animate(anim, flags, speed);
}

void ModelInstance::animate(const string &anim, int flags, float speed) {
    if (!_model || _animState.name == anim) return;

    _animState.nextAnimation = anim;
    _animState.nextFlags = flags;
    _animState.nextSpeed = speed;

    if (flags & kAnimationPropagate) {
        for (auto &pair : _attachedModels) {
            pair.second->animate(anim, flags, speed);
        }
    }
}

void ModelInstance::attach(const string &parentNode, const shared_ptr<Model> &model) {
    if (!model) return;

    shared_ptr<ModelNode> parent(_model->findNodeByName(parentNode));
    if (!parent) {
        warn("Parent node not found: " + parentNode);
        return;
    }
    _attachedModels.insert(make_pair(parent->nodeNumber(), make_unique<ModelInstance>(model)));
}

void ModelInstance::changeTexture(const string &resRef) {
    _textureOverride = ResMan.findTexture(resRef, TextureType::Diffuse);
}

void ModelInstance::update(float dt) {
    doUpdate(dt, set<string>());
}

void ModelInstance::doUpdate(float dt, const set<string> &skipNodes) {
    if (!_visible) return;

    if (!_animState.nextAnimation.empty()) {
        startNextAnimation();
    }
    if (!_animState.name.empty()) {
        advanceAnimation(dt, skipNodes);
    }

    _nodeTransforms.clear();
    _boneTransforms.clear();
    updateNodeTansforms(_model->rootNode(), glm::mat4(1.0f));

    for (auto &pair : _attachedModels) {
        set<string> skipNodes;
        shared_ptr<ModelNode> parent(_model->findNodeByNumber(pair.first));

        const ModelNode *pn = &*parent;
        while (pn) {
            skipNodes.insert(pn->name());
            pn = pn->parent();
        }

        pair.second->doUpdate(dt, skipNodes);
    }
}

void ModelInstance::startNextAnimation() {
    const Model *model = nullptr;
    shared_ptr<Animation> anim(_model->findAnimation(_animState.nextAnimation, &model));
    if (!anim) return;

    _animState.animation = move(anim);
    _animState.model = model;
    _animState.name = _animState.nextAnimation;
    _animState.flags = _animState.nextFlags;
    _animState.speed = _animState.nextSpeed;
    _animState.time = 0.0f;
    _animState.localTransforms.clear();

    _animState.nextAnimation.clear();
    _animState.nextFlags = 0;
}

void ModelInstance::advanceAnimation(float dt, const set<string> &skipNodes) {
    float length = _animState.animation->length();
    float time = _animState.time + _animState.speed * dt;

    if (_animState.flags & kAnimationLoop) {
        _animState.time = glm::mod(time, length);
    } else {
        _animState.time = glm::min(time, length);
        if (_animState.time == length && !_defaultAnimation.empty()) {
            playDefaultAnimation();
        }
    }

    _animState.localTransforms.clear();
    updateAnimTransforms(*_animState.animation->rootNode(), glm::mat4(1.0f), _animState.time, skipNodes);
}

void ModelInstance::updateAnimTransforms(const ModelNode &animNode, const glm::mat4 &transform, float time, const set<string> &skipNodes) {
    string name(animNode.name());
    glm::mat4 absTransform(transform);

    if (skipNodes.count(name) == 0) {
        shared_ptr<ModelNode> refNode(_model->findNodeByName(name));
        if (refNode) {
            glm::mat4 localTransform(glm::translate(glm::mat4(1.0f), refNode->position()));

            glm::vec3 position;
            if (animNode.getPosition(time, position, _model->animationScale())) {
                localTransform = glm::translate(localTransform, move(position));
            }

            glm::quat orientation;
            if (animNode.getOrientation(time, orientation)) {
                localTransform *= glm::mat4_cast(move(orientation));
            } else {
                localTransform *= glm::mat4_cast(refNode->orientation());
            }

            absTransform *= localTransform;
            _animState.localTransforms.insert(make_pair(name, localTransform));
        }
    }

    for (auto &child : animNode.children()) {
        updateAnimTransforms(*child, absTransform, time, skipNodes);
    }
}

void ModelInstance::updateNodeTansforms(const ModelNode &node, const glm::mat4 &transform) {
    glm::mat4 finalTransform(transform);
    bool animApplied = false;

    if (!_animState.name.empty()) {
        auto it = _animState.localTransforms.find(node.name());
        if (it != _animState.localTransforms.end()) {
            finalTransform *= it->second;
            animApplied = true;
        }
    }
    if (!animApplied) {
        finalTransform = glm::translate(finalTransform, node.position());
        finalTransform *= glm::mat4_cast(node.orientation());
    }

    _nodeTransforms.insert(make_pair(node.nodeNumber(), finalTransform));
    _boneTransforms.insert(make_pair(node.index(), finalTransform * node.absoluteTransformInverse()));

    for (auto &child : node.children()) {
        updateNodeTansforms(*child, finalTransform);
    }
}

void ModelInstance::fill(SceneGraph &scene, const glm::mat4 &baseTransform, bool debug) {
    if (!_model || !_visible) return;

    stack<const ModelNode *> nodes;
    nodes.push(&_model->rootNode());

    while (!nodes.empty()) {
        const ModelNode &node = *nodes.top();
        nodes.pop();

        if (shouldRender(node)) {
            glm::mat4 transform(baseTransform * getNodeTransform(node));

            shared_ptr<ModelSceneNode> sceneNode(new ModelSceneNode(this, &node, transform));
            scene.add(sceneNode);

            if (debug) {
                shared_ptr<AABBSceneNode> aabbNode(new AABBSceneNode(node.mesh()->aabb(), transform));
                scene.add(aabbNode);
            }
        }
        for (auto &child : node.children()) {
            nodes.push(child.get());
        }
    }
    for (auto &pair : _attachedModels) {
        shared_ptr<ModelNode> parent(_model->findNodeByNumber(pair.first));
        if (!parent) continue;
    
        glm::mat4 finalTransform(baseTransform * getNodeTransform(*parent));
        pair.second->fill(scene, finalTransform, debug);
    }
}

bool ModelInstance::shouldRender(const ModelNode &node) const {
    shared_ptr<ModelMesh> mesh(node.mesh());
    if (!mesh) return false;

    return mesh->shouldRender() && (mesh->hasDiffuseTexture() || _textureOverride);
}

glm::mat4 ModelInstance::getNodeTransform(const ModelNode &node) const {
    if (node.mesh() && node.skin()) {
        return node.absoluteTransform();
    }
    auto it = _nodeTransforms.find(node.nodeNumber());

    return it != _nodeTransforms.end() ? it->second : node.absoluteTransform();
}

void ModelInstance::render(const glm::mat4 &transform) const {
    if (!_visible) return;

    stack<const ModelNode *> nodes;
    nodes.push(&_model->rootNode());

    while (!nodes.empty()) {
        const ModelNode &node = *nodes.top();
        nodes.pop();

        if (shouldRender(node)) {
            render(node, transform * getNodeTransform(node));
        }
        for (auto &child : node.children()) {
            nodes.push(child.get());
        }
    }

    for (auto &attached : _attachedModels) {
        shared_ptr<ModelNode> parent(_model->findNodeByNumber(attached.first));
        if (!parent) continue;

        attached.second->render(transform * getNodeTransform(*parent));
    }
}

void ModelInstance::render(const ModelNode &node, const glm::mat4 &transform) const {
    shared_ptr<ModelMesh> mesh(node.mesh());
    shared_ptr<ModelNode::Skin> skin(node.skin());
    bool skeletal = skin && !_animState.name.empty();
    ShaderProgram program = getShaderProgram(*mesh, skeletal);

    ShaderManager &shaders = ShaderManager::instance();
    shaders.activate(program);
    shaders.setUniform("model", transform);
    shaders.setUniform("color", glm::vec3(1.0f));
    shaders.setUniform("alpha", _alpha * node.alpha());

    if (mesh->hasEnvmapTexture()) {
        shaders.setUniform("envmap", 1);
    }
    if (mesh->hasLightmapTexture()) {
        shaders.setUniform("lightmap", 2);
    }
    if (mesh->hasBumpyShinyTexture()) {
        shaders.setUniform("bumpyShiny", 3);
    }
    if (mesh->hasBumpmapTexture()) {
        shaders.setUniform("bumpmap", 4);
    }

    if (skeletal) {
        shaders.setUniform("absTransform", node.absoluteTransform());
        shaders.setUniform("absTransformInv", node.absoluteTransformInverse());

        const map<uint16_t, uint16_t> &nodeIdxByBoneIdx = skin->nodeIdxByBoneIdx;
        vector<glm::mat4> bones(nodeIdxByBoneIdx.size(), glm::mat4(1.0f));

        for (auto &pair : nodeIdxByBoneIdx) {
            uint16_t boneIdx = pair.first;
            uint16_t nodeIdx = pair.second;

            auto bone = _boneTransforms.find(nodeIdx);
            if (bone == _boneTransforms.end()) continue;

            bones[boneIdx] = bone->second;
        }

        shaders.setUniform("bones", bones);
    }

    mesh->render(_textureOverride);
}

ShaderProgram ModelInstance::getShaderProgram(const ModelMesh &mesh, bool skeletal) const {
    ShaderProgram program = ShaderProgram::None;

    bool hasEnvmap = mesh.hasEnvmapTexture();
    bool hasLightmap = mesh.hasLightmapTexture();
    bool hasBumpyShiny = mesh.hasBumpyShinyTexture();
    bool hasBumpmap = mesh.hasBumpmapTexture();

    if (skeletal) {
        if (hasEnvmap && !hasLightmap && !hasBumpyShiny && !hasBumpmap) {
            program = ShaderProgram::SkeletalDiffuseEnvmap;
        } else if (hasBumpyShiny && !hasEnvmap && !hasLightmap /* && !hasBumpmap */) {
            program = ShaderProgram::SkeletalDiffuseBumpyShiny;
        } else if (hasBumpmap && !hasEnvmap && !hasLightmap && !hasBumpyShiny) {
            program = ShaderProgram::SkeletalDiffuseBumpmap;
        } else if (!hasEnvmap && !hasLightmap && !hasBumpyShiny && !hasBumpmap) {
            program = ShaderProgram::SkeletalDiffuse;
        }
    } else {
        if (hasEnvmap && !hasLightmap && !hasBumpyShiny) {
            program = ShaderProgram::BasicDiffuseEnvmap;
        } else if (hasBumpyShiny && !hasEnvmap && !hasLightmap) {
            program = ShaderProgram::BasicDiffuseBumpyShiny;
        } else if (hasLightmap && !hasEnvmap && !hasBumpyShiny) {
            program = ShaderProgram::BasicDiffuseLightmap;
        } else if (hasEnvmap && hasLightmap && !hasBumpyShiny) {
            program = ShaderProgram::BasicDiffuseLightmapEnvmap;
        } else if (hasLightmap && hasBumpyShiny && !hasEnvmap) {
            program = ShaderProgram::BasicDiffuseLightmapBumpyShiny;
        } else if (!hasEnvmap && !hasLightmap && !hasBumpyShiny) {
            program = ShaderProgram::BasicDiffuse;
        }
    }

    if (program == ShaderProgram::None) {
        throw logic_error("Shader program not selected");
    }

    return program;
}

void ModelInstance::playDefaultAnimation() {
    animate(_defaultAnimation, kAnimationLoop | kAnimationPropagate);
}

void ModelInstance::show() {
    _visible = true;

    for (auto &pair : _attachedModels) {
        pair.second->show();
    }
}

void ModelInstance::hide() {
    _visible = false;

    for (auto &pair : _attachedModels) {
        pair.second->hide();
    }
}

void ModelInstance::setAlpha(float alpha) {
    _alpha = alpha;

    for (auto &pair : _attachedModels) {
        pair.second->setAlpha(alpha);
    }
}

void ModelInstance::setDefaultAnimation(const string &name) {
    _defaultAnimation = name;
}

glm::vec3 ModelInstance::getNodeAbsolutePosition(const string &name) const {
    glm::vec3 position(0.0f);

    shared_ptr<ModelNode> node(_model->findNodeByName(name));
    if (!node) {
        shared_ptr<Model> superModel(_model->superModel());
        if (superModel) {
            node = superModel->findNodeByName(name);
        }
    }
    if (!node) {
        warn(boost::format("Model node not found: %s %s") % _model->name() % name);
        return glm::vec3(0.0f);
    }

    return node->absoluteTransform()[3];
}

const string &ModelInstance::name() const {
    return _model->name();
}

shared_ptr<Model> ModelInstance::model() const {
    return _model;
}

bool ModelInstance::visible() const {
    return _visible;
}

} // namespace render

} // namespace reone
