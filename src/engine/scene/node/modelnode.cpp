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

#include <stdexcept>

#include "../../common/collectionutil.h"
#include "../../common/log.h"
#include "../../common/guardutil.h"

#include "../scenegraph.h"
#include "../types.h"

#include "emitternode.h"
#include "lightnode.h"
#include "meshnode.h"
#include "modelnode.h"

using namespace std;

using namespace reone::graphics;

namespace reone {

namespace scene {

ModelSceneNode::ModelSceneNode(
    shared_ptr<Model> model,
    ModelUsage usage,
    SceneGraph *sceneGraph,
    IAnimationEventListener *animEventListener
) :
    SceneNode(model->name(), SceneNodeType::Model, sceneGraph),
    _model(model),
    _usage(usage),
    _animEventListener(animEventListener) {

    ensureNotNull(model, "model");

    _volumetric = true;

    buildNodeTree(_model->rootNode(), this);
    computeAABB();
}

void ModelSceneNode::buildNodeTree(shared_ptr<ModelNode> node, SceneNode *parent) {
    // Convert model node to scene node
    shared_ptr<ModelNodeSceneNode> sceneNode;
    if (node->isMesh()) {
        sceneNode = newMeshSceneNode(node);
    } else if (node->isLight()) {
        sceneNode = newLightSceneNode(node);
    } else if (node->isEmitter()) {
        sceneNode = newEmitterSceneNode(node);
    } else {
        sceneNode = newDummySceneNode(node);
    }

    if (node->isSkinMesh()) {
        // Reparent skin meshes to prevent animation being applied twice
        glm::mat4 transform(node->parent()->absoluteTransform() * node->localTransform());
        sceneNode->setLocalTransform(move(transform));
        addChild(sceneNode);
    } else {
        sceneNode->setLocalTransform(node->localTransform());
        parent->addChild(sceneNode);
    }
    _nodeByName.insert(make_pair(node->name(), sceneNode));

    if (node->isReference()) {
        auto model = make_shared<ModelSceneNode>(node->reference()->model, _usage, _sceneGraph, _animEventListener);
        attach(node->name(), move(model));
    }
    for (auto &child : node->children()) {
        buildNodeTree(child, sceneNode.get());
    }
}

void ModelSceneNode::update(float dt) {
    // Optimization: skip invisible models
    if (!_visible) return;

    SceneNode::update(dt);
    updateAnimations(dt);
}

void ModelSceneNode::computeAABB() {
    _aabb.reset();

    for (auto &node : _nodeByName) {
        if (node.second->type() == SceneNodeType::Mesh) {
            shared_ptr<ModelNode> modelNode(node.second->modelNode());
            AABB modelSpaceAABB(modelNode->mesh()->mesh->aabb() * modelNode->absoluteTransform());
            _aabb.expand(modelSpaceAABB);
        }
    }
    for (auto &attachment : _attachments) {
        if (attachment.second->type() == SceneNodeType::Model) {
            AABB modelSpaceAABB(attachment.second->aabb() * attachment.second->absoluteTransform() * _absTransformInv);
            _aabb.expand(modelSpaceAABB);
        }
    }
}

unique_ptr<DummySceneNode> ModelSceneNode::newDummySceneNode(shared_ptr<ModelNode> node) const {
    return make_unique<DummySceneNode>(node, _sceneGraph);
}

unique_ptr<MeshSceneNode> ModelSceneNode::newMeshSceneNode(shared_ptr<ModelNode> node) const {
    return make_unique<MeshSceneNode>(this, node, _sceneGraph);
}

unique_ptr<LightSceneNode> ModelSceneNode::newLightSceneNode(shared_ptr<ModelNode> node) const {
    return make_unique<LightSceneNode>(this, node, _sceneGraph);
}

unique_ptr<EmitterSceneNode> ModelSceneNode::newEmitterSceneNode(shared_ptr<ModelNode> node) const {
    return make_unique<EmitterSceneNode>(this, node, _sceneGraph);
}

void ModelSceneNode::signalEvent(const string &name) {
    debug(boost::format("Model '%s': event '%s' signalled") % _model->name() % name, 3);

    if (name == "detonate") {
        for (auto &node : _nodeByName) {
            if (node.second->type() == SceneNodeType::Emitter) {
                static_pointer_cast<EmitterSceneNode>(node.second)->detonate();
            }
        }
    } else if (_animEventListener) {
        _animEventListener->onEventSignalled(name);
    }
}

void ModelSceneNode::attach(const string &parentName, shared_ptr<SceneNode> node) {
    auto maybeParent = _nodeByName.find(parentName);
    if (maybeParent == _nodeByName.end()) return;

    shared_ptr<ModelNodeSceneNode> parent(maybeParent->second);
    parent->addChild(node);

    _attachments.insert(make_pair(parentName, node));

    computeAABB();
}

shared_ptr<ModelNodeSceneNode> ModelSceneNode::getNodeByName(const string &name) const {
    return getFromLookupOrNull(_nodeByName, name);
}

shared_ptr<SceneNode> ModelSceneNode::getAttachment(const string &parentName) const {
    auto parent = _model->getNodeByName(parentName);
    return parent ? getFromLookupOrNull(_attachments, parent->name()) : nullptr;
}

void ModelSceneNode::setDiffuseTexture(shared_ptr<Texture> texture) {
    for (auto &child : _children) {
        if (child->type() == SceneNodeType::Mesh) {
            static_pointer_cast<MeshSceneNode>(child)->setDiffuseTexture(texture);
        }
    }
}

void ModelSceneNode::setAppliedForce(glm::vec3 force) {
    for (auto &node : _nodeByName) {
        if (node.second->type() == SceneNodeType::Mesh) {
            static_pointer_cast<MeshSceneNode>(node.second)->setAppliedForce(force);
        }
    }
    for (auto &attachment : _attachments) {
        if (attachment.second->type() == SceneNodeType::Model) {
            static_pointer_cast<ModelSceneNode>(attachment.second)->setAppliedForce(force);
        }
    }
}

} // namespace scene

} // namespace reone
