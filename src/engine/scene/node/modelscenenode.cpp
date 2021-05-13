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

#include <stack>
#include <stdexcept>

#include "../../common/collectionutil.h"
#include "../../common/log.h"
#include "../../graphics/featureutil.h"
#include "../../graphics/mesh/meshes.h"
#include "../../resource/resources.h"

#include "../scenegraph.h"
#include "../types.h"

#include "cameranode.h"
#include "emitternode.h"
#include "lightnode.h"
#include "modelnodescenenode.h"
#include "modelscenenode.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace scene {

const float kMinDirectionalLightRadius = 1000.0f;

static bool g_debugAABB = false;

ModelSceneNode::ModelSceneNode(
    ModelUsage usage,
    const shared_ptr<Model> &model,
    SceneGraph *sceneGraph,
    set<string> ignoreNodes,
    IAnimationEventListener *animEventListener
) :
    SceneNode(SceneNodeType::Model, sceneGraph),
    _animEventListener(animEventListener),
    _usage(usage),
    _model(model),
    _animator(this, ignoreNodes) {

    initModelNodes();

    _volumetric = true;
}

static bool validateEmitter(const ModelNode::Emitter &emitter) {
    switch (emitter.updateMode) {
        case ModelNode::Emitter::UpdateMode::Fountain:
        case ModelNode::Emitter::UpdateMode::Single:
        case ModelNode::Emitter::UpdateMode::Explosion:
            break;
        default:
            warn("validateEmitter: unsupported update mode: " + to_string(static_cast<int>(emitter.updateMode)));
            return false;
    }

    switch (emitter.renderMode) {
        case ModelNode::Emitter::RenderMode::Normal:
        case ModelNode::Emitter::RenderMode::BillboardToWorldZ:
        case ModelNode::Emitter::RenderMode::MotionBlur:
        case ModelNode::Emitter::RenderMode::BillboardToLocalZ:
        case ModelNode::Emitter::RenderMode::AlignedToParticleDir:
            break;
        default:
            warn("validateEmitter: unsupported render mode: " + to_string(static_cast<int>(emitter.renderMode)));
            return false;
    }

    switch (emitter.blendMode) {
        case ModelNode::Emitter::BlendMode::Normal:
        case ModelNode::Emitter::BlendMode::Lighten:
            break;
        default:
            warn("validateEmitter: unsupported blend mode: " + to_string(static_cast<int>(emitter.blendMode)));
            return false;
    }

    return true;
}

void ModelSceneNode::initModelNodes() {
    shared_ptr<ModelNodeSceneNode> rootNode(getModelNodeSceneNode(*_model->rootNode()));
    addChild(rootNode);

    stack<ModelNodeSceneNode *> nodes;
    nodes.push(rootNode.get());

    while (!nodes.empty()) {
        ModelNodeSceneNode *sceneNode = nodes.top();
        nodes.pop();

        const ModelNode *modelNode = sceneNode->modelNode();
        _modelNodeById.insert(make_pair(modelNode->id(), sceneNode));

        for (auto &child : modelNode->children()) {
            shared_ptr<ModelNodeSceneNode> childNode(getModelNodeSceneNode(*child));
            addChild(childNode);
            nodes.push(childNode.get());

            shared_ptr<ModelNode::Light> light(child->light());
            if (light) {
                // Light is considered directional if its radius exceeds a certain threshold
                float radius = child->radius().getByFrameOrElse(0, 1.0f);
                bool directional = radius >= kMinDirectionalLightRadius;

                auto lightNode = make_shared<LightSceneNode>(light->priority, _sceneGraph);
                lightNode->setColor(child->color().getByFrameOrElse(0, glm::vec3(1.0f)));
                lightNode->setMultiplier(child->multiplier().getByFrameOrElse(0, 1.0f));
                lightNode->setRadius(radius);
                lightNode->setShadow(light->shadow);
                lightNode->setAmbientOnly(light->ambientOnly);
                lightNode->setDirectional(directional);
                lightNode->setFlareRadius(light->flareRadius);
                lightNode->setFlares(light->flares);

                childNode->addChild(lightNode);
                _lightNodeById.insert(make_pair(modelNode->id(), lightNode.get()));
            }

            shared_ptr<ModelNode::Emitter> emitter(child->emitter());
            if (emitter && validateEmitter(*emitter)) {
                auto emitterNode = make_shared<EmitterSceneNode>(this, emitter, _sceneGraph);
                childNode->addChild(emitterNode);
                _emitters.push_back(emitterNode);
            }

            // If model node is a reference, attach the model it contains to the model nodes scene node
            shared_ptr<ModelNode::Reference> reference(child->reference());
            if (reference) {
                attach(*childNode, reference->model, _usage);
            }
        }
    }

    computeAABB();
}

unique_ptr<ModelNodeSceneNode> ModelSceneNode::getModelNodeSceneNode(ModelNode &node) const {
    auto sceneNode = make_unique<ModelNodeSceneNode>(_sceneGraph, this, &node);
    sceneNode->setLocalTransform(node.absoluteTransform());
    return move(sceneNode);
}

void ModelSceneNode::update(float dt) {
    _animator.update(dt, !_culled);
    SceneNode::update(dt);
}

void ModelSceneNode::draw() {
    if (g_debugAABB) {
        glm::mat4 transform(_absoluteTransform * _aabb.transform());

        ShaderUniforms uniforms(_sceneGraph->uniformsPrototype());
        uniforms.combined.general.model = move(transform);

        Shaders::instance().activate(ShaderProgram::ModelColor, uniforms);
        Meshes::instance().getAABB()->draw();
    }
}

shared_ptr<ModelSceneNode> ModelSceneNode::attach(const string &parent, const shared_ptr<Model> &model, ModelUsage usage) {
    ModelNodeSceneNode *parentNode = getModelNode(parent);
    return parentNode ? attach(*parentNode, model, usage) : nullptr;
}

shared_ptr<ModelSceneNode> ModelSceneNode::attach(ModelNodeSceneNode &parent, const shared_ptr<Model> &model, ModelUsage usage) {
    const ModelNode *parentModelNode = parent.modelNode();
    uint16_t parentId = parentModelNode->id();

    auto maybeAttached = _attachedModels.find(parentId);
    if (maybeAttached != _attachedModels.end()) {
        parent.removeChild(*maybeAttached->second);
        _attachedModels.erase(maybeAttached);
    }
    if (model) {
        set<string> ignoreNodes;
        for (const ModelNode *node = parentModelNode; node; node = node->parent()) {
            ignoreNodes.insert(node->name());
        }
        auto modelNode = make_shared<ModelSceneNode>(usage, model, _sceneGraph, ignoreNodes);
        parent.addChild(modelNode);

        return _attachedModels.insert(make_pair(parentId, move(modelNode))).first->second;
    }

    return nullptr;

}

ModelNodeSceneNode *ModelSceneNode::getModelNode(const string &name) const {
    shared_ptr<ModelNode> modelNode(_model->getNodeByName(name));
    if (!modelNode) return nullptr;

    return getFromLookupOrNull(_modelNodeById, modelNode->id());
}

ModelNodeSceneNode *ModelSceneNode::getModelNodeById(uint16_t nodeId) const {
    return getFromLookupOrNull(_modelNodeById, nodeId);
}

LightSceneNode *ModelSceneNode::getLightNodeById(uint16_t nodeId) const {
    return getFromLookupOrNull(_lightNodeById, nodeId);
}

shared_ptr<ModelSceneNode> ModelSceneNode::getAttachedModel(const string &parent) const {
    shared_ptr<ModelNode> parentModelNode(_model->getNodeByName(parent));
    if (!parentModelNode) return nullptr;

    return getFromLookupOrNull(_attachedModels, parentModelNode->id());
}

void ModelSceneNode::attach(const string &parent, const shared_ptr<SceneNode> &node) {
    shared_ptr<ModelNode> parentModelNode(_model->getNodeByName(parent));
    if (!parentModelNode) {
        warn(boost::format("Scene node %s: model node not found: %s") % _model->name() % parent);
        return;
    }
    uint16_t parentId = parentModelNode->id();

    auto maybeNode = _modelNodeById.find(parentId);
    if (maybeNode != _modelNodeById.end()) {
        maybeNode->second->addChild(node);
    }
}

bool ModelSceneNode::getNodeAbsolutePosition(const string &name, glm::vec3 &position) const {
    shared_ptr<ModelNode> node(_model->getNodeByName(name));
    if (!node) {
        shared_ptr<Model> superModel(_model->superModel());
        if (superModel) {
            node = superModel->getNodeByName(name);
        }
    }
    if (!node) return false;

    position = node->absoluteTransform()[3];

    return true;
}

glm::vec3 ModelSceneNode::getWorldCenterAABB() const {
    return _absoluteTransform * glm::vec4(_aabb.center(), 1.0f);
}

const string &ModelSceneNode::getName() const {
    return _model->name();
}

void ModelSceneNode::setDiffuseTexture(const shared_ptr<Texture> &texture) {
    for (auto &child : _children) {
        if (child->type() == SceneNodeType::ModelNode) {
            static_pointer_cast<ModelNodeSceneNode>(child)->setDiffuseTexture(texture);
        }
    }
}

void ModelSceneNode::setVisible(bool visible) {
    if (_visible == visible) return;

    _visible = visible;

    for (auto &attached : _attachedModels) {
        attached.second->setVisible(visible);
    }
}

void ModelSceneNode::setAlpha(float alpha) {
    _alpha = alpha;

    for (auto &attached : _attachedModels) {
        attached.second->setAlpha(alpha);
    }
}

void ModelSceneNode::computeAABB() {
    _aabb.reset();

    stack<SceneNode *> nodes;
    nodes.push(this);

    while (!nodes.empty()) {
        SceneNode *node = nodes.top();
        nodes.pop();

        if (node->type() == SceneNodeType::ModelNode) {
            auto modelNode = static_cast<ModelNodeSceneNode *>(node);
            shared_ptr<ModelNode::TriangleMesh> mesh(modelNode->modelNode()->mesh());
            if (mesh) {
                _aabb.expand(mesh->mesh->aabb() * node->localTransform());
            }
        }

        for (auto &child : node->children()) {
            nodes.push(child.get());
        }
    }
}

void ModelSceneNode::signalEvent(const string &name) {
    debug(boost::format("Animation event signalled: %s %s") % _model->name() % name, 3);

    if (name == "detonate") {
        for (auto &emitter : _emitters) {
            emitter->detonate();
        }
    } else if (_animEventListener) {
        _animEventListener->onEventSignalled(name);
    }
}

void ModelSceneNode::setAppliedForce(glm::vec3 force) {
    for (auto &nodePair : _modelNodeById) {
        nodePair.second->setAppliedForce(force);
    }
    for (auto &attached : _attachedModels) {
        attached.second->setAppliedForce(force);
    }
}

} // namespace scene

} // namespace reone
