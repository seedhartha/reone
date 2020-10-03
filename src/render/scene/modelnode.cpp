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

#include "../../core/log.h"
#include "../../resource/resources.h"

#include "../mesh/aabb.h"
#include "../scene/aabbnode.h"
#include "../scene/meshnode.h"
#include "../scene/scenegraph.h"

#include "modelnode.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace render {

ModelSceneNode::ModelSceneNode(SceneGraph *sceneGraph, const shared_ptr<Model> &model) : SceneNode(sceneGraph), _model(model) {
    initChildren();
}

void ModelSceneNode::initChildren() {
    stack<const ModelNode *> nodes;
    nodes.push(&_model->rootNode());

    while (!nodes.empty()) {
        const ModelNode *node = nodes.top();
        nodes.pop();

        for (auto &child : node->children()) {
            nodes.push(child.get());
        }
        if (shouldRender(*node)) {
            shared_ptr<MeshSceneNode> mesh(new MeshSceneNode(_sceneGraph, this, node));
            mesh->setParent(this);
            mesh->setLocalTransform(node->absoluteTransform());

            shared_ptr<AABBSceneNode> aabb(new AABBSceneNode(_sceneGraph, node->mesh()->aabb()));
            mesh->addChild(aabb);

            _meshes.insert(make_pair(node->nodeNumber(), mesh));
        }

        shared_ptr<ModelNode::Light> modelLight(node->light());
        if (modelLight) {
            shared_ptr<LightSceneNode> light(new LightSceneNode(_sceneGraph, node));
            light->setParent(this);
            light->setLocalTransform(node->absoluteTransform());

            _lights.insert(make_pair(node->nodeNumber(), light));
        }
    }
}

void ModelSceneNode::fillSceneGraph() {
    if (!_visible || !_onScreen) return;

    for (auto &mesh : _meshes) {
        mesh.second->fillSceneGraph();
    }
    for (auto &light : _lights) {
        light.second->fillSceneGraph();
    }
    SceneNode::fillSceneGraph();
}

void ModelSceneNode::animate(const string &parent, const string &anim, int flags, float speed) {
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

void ModelSceneNode::animate(const string &anim, int flags, float speed) {
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

void ModelSceneNode::attach(const string &parentNode, const shared_ptr<Model> &model) {
    shared_ptr<ModelNode> parent(_model->findNodeByName(parentNode));
    if (!parent) {
        warn("Parent node not found: " + parentNode);
        return;
    }
    uint16_t parentNumber = parent->nodeNumber();
    auto maybeAttached = _attachedModels.find(parentNumber);

    if (maybeAttached != _attachedModels.end()) {
        removeChild(maybeAttached->second);
        _attachedModels.erase(maybeAttached);
    }
    if (model) {
        shared_ptr<ModelSceneNode> child(new ModelSceneNode(_sceneGraph, model));
        child->setLocalTransform(parent->absoluteTransform());
        child->setLightingEnabled(_lightingEnabled);
        addChild(child);

        _attachedModels.insert(make_pair(parentNumber, child));
    }
}

void ModelSceneNode::update(float dt) {
    doUpdate(dt, set<string>());
}

void ModelSceneNode::doUpdate(float dt, const set<string> &skipNodes) {
    if (!_visible || !_onScreen) return;

    if (!_animState.nextAnimation.empty()) {
        startNextAnimation();
    }
    if (!_animState.name.empty()) {
        advanceAnimation(dt, skipNodes);
    }

    _nodeTransforms.clear();
    _animState.boneTransforms.clear();
    updateNodeTansforms(_model->rootNode(), glm::mat4(1.0f));

    for (auto &attached : _attachedModels) {
        set<string> skipNodes;
        shared_ptr<ModelNode> parent(_model->findNodeByNumber(attached.first));

        const ModelNode *pn = &*parent;
        while (pn) {
            skipNodes.insert(pn->name());
            pn = pn->parent();
        }

        attached.second->doUpdate(dt, skipNodes);
    }
}

void ModelSceneNode::startNextAnimation() {
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

void ModelSceneNode::advanceAnimation(float dt, const set<string> &skipNodes) {
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

void ModelSceneNode::updateAnimTransforms(const ModelNode &animNode, const glm::mat4 &transform, float time, const set<string> &skipNodes) {
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

void ModelSceneNode::updateNodeTansforms(const ModelNode &node, const glm::mat4 &transform) {
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
    _animState.boneTransforms.insert(make_pair(node.index(), finalTransform * node.absoluteTransformInverse()));

    glm::mat4 nodeTransform(getNodeTransform(node));
    auto meshNode = _meshes.find(node.nodeNumber());
    if (meshNode != _meshes.end()) {
        meshNode->second->setLocalTransform(nodeTransform);
    }
    auto lightNode = _lights.find(node.nodeNumber());
    if (lightNode != _lights.end()) {
        lightNode->second->setLocalTransform(nodeTransform);
    }
    auto attached = _attachedModels.find(node.nodeNumber());
    if (attached != _attachedModels.end()) {
        attached->second->setLocalTransform(nodeTransform);
    }

    for (auto &child : node.children()) {
        updateNodeTansforms(*child, finalTransform);
    }
}

bool ModelSceneNode::shouldRender(const ModelNode &node) const {
    shared_ptr<ModelMesh> mesh(node.mesh());
    if (!mesh) return false;

    return mesh->shouldRender() && (mesh->hasDiffuseTexture() || _textureOverride);
}

glm::mat4 ModelSceneNode::getNodeTransform(const ModelNode &node) const {
    if (node.mesh() && node.skin()) {
        return node.absoluteTransform();
    }
    auto it = _nodeTransforms.find(node.nodeNumber());

    return it != _nodeTransforms.end() ? it->second : node.absoluteTransform();
}

void ModelSceneNode::updateAbsoluteTransform() {
    SceneNode::updateAbsoluteTransform();
    _lightingDirty = true;
}

void ModelSceneNode::playDefaultAnimation() {
    animate(_defaultAnimation, kAnimationLoop | kAnimationPropagate);
}

void ModelSceneNode::updateLighting() {
    if (!_lightingEnabled || !_lightingDirty) return;

    _lightsAffectedBy.clear();
    glm::vec3 center(_absoluteTransform * glm::vec4(_model->aabb().center(), 1.0f));

    _sceneGraph->getLightsAt(center, _lightsAffectedBy);
    _lightingDirty = false;

    for (auto &attached : _attachedModels) {
        attached.second->setLightsAffectedBy(_lightsAffectedBy);
    }
}

void ModelSceneNode::setLightingIsDirty() {
    _lightingDirty = true;
}

bool ModelSceneNode::getNodeAbsolutePosition(const string &name, glm::vec3 &position) const {
    shared_ptr<ModelNode> node(_model->findNodeByName(name));
    if (!node) {
        shared_ptr<Model> superModel(_model->superModel());
        if (superModel) {
            node = superModel->findNodeByName(name);
        }
    }
    if (!node) return false;

    position = node->absoluteTransform()[3];

    return true;
}

glm::vec3 ModelSceneNode::getCenterOfAABB() const {
    return _absoluteTransform * glm::vec4(_model->aabb().center(), 1.0f);
}

const string &ModelSceneNode::name() const {
    return _model->name();
}

shared_ptr<Model> ModelSceneNode::model() const {
    return _model;
}

const ModelSceneNode::AnimationState &ModelSceneNode::animationState() const {
    return _animState;
}

shared_ptr<Texture> ModelSceneNode::textureOverride() const {
    return _textureOverride;
}

bool ModelSceneNode::isVisible() const {
    return _visible;
}

bool ModelSceneNode::isOnScreen() const {
    return _onScreen;
}

float ModelSceneNode::alpha() const {
    return _alpha;
}

const AABB &ModelSceneNode::aabb() const {
    return _model->aabb();
}

bool ModelSceneNode::isLightingEnabled() const {
    return _lightingEnabled;
}

const vector<LightSceneNode *> &ModelSceneNode::lightsAffectedBy() const {
    return _lightsAffectedBy;
}

void ModelSceneNode::setModel(const shared_ptr<Model> &model) {
    _model = model;
    assert(_model);

    _meshes.clear();
    _lights.clear();

    for (auto &attached : _attachedModels) {
        removeChild(attached.second);
    }
    _attachedModels.clear();

    initChildren();
}

void ModelSceneNode::setTextureOverride(const shared_ptr<Texture> &texture) {
    _textureOverride = texture;
}

void ModelSceneNode::setVisible(bool visible) {
    if (_visible == visible) return;

    _visible = visible;

    for (auto &attached : _attachedModels) {
        attached.second->setVisible(visible);
    }
    if (visible) {
        _lightingDirty = true;
    }
}

void ModelSceneNode::setOnScreen(bool onScreen) {
    _onScreen = onScreen;

    for (auto &attached : _attachedModels) {
        attached.second->setOnScreen(onScreen);
    }
}

void ModelSceneNode::setAlpha(float alpha) {
    _alpha = alpha;

    for (auto &attached : _attachedModels) {
        attached.second->setAlpha(alpha);
    }
}

void ModelSceneNode::setDefaultAnimation(const string &name) {
    _defaultAnimation = name;
}

void ModelSceneNode::setLightingEnabled(bool enabled) {
    _lightingEnabled = enabled;
}

void ModelSceneNode::setLightsAffectedBy(const vector<LightSceneNode *> &lights) {
    _lightsAffectedBy = lights;
}

} // namespace render

} // namespace reone
