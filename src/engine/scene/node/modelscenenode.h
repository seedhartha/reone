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

#pragma once

#include <set>
#include <unordered_map>

#include "../../graphics/model/model.h"
#include "../../graphics/shader/shaders.h"
#include "../../graphics/walkmesh/walkmesh.h"

#include "../animation/eventlistener.h"
#include "../animation/scenenodeanimator.h"
#include "../types.h"

#include "scenenode.h"

namespace reone {

namespace scene {

class EmitterSceneNode;
class LightSceneNode;
class ModelNodeSceneNode;

class ModelSceneNode : public SceneNode {
public:
    ModelSceneNode(
        ModelUsage usage,
        const std::shared_ptr<graphics::Model> &model,
        SceneGraph *sceneGraph,
        std::set<std::string> ignoreNodes = std::set<std::string>(),
        IAnimationEventListener *animEventListener = nullptr);

    void update(float dt) override;
    void draw() override;

    void computeAABB();
    void signalEvent(const std::string &name);
    void setAppliedForce(glm::vec3 force);

    ModelNodeSceneNode *getModelNode(const std::string &name) const;
    ModelNodeSceneNode *getModelNodeById(uint16_t nodeId) const;
    LightSceneNode *getLightNodeById(uint16_t nodeId) const;
    std::shared_ptr<ModelSceneNode> getAttachedModel(const std::string &parent) const;
    bool getNodeAbsolutePosition(const std::string &name, glm::vec3 &position) const;
    glm::vec3 getWorldCenterAABB() const;
    const std::string &getName() const;

    ModelUsage usage() const { return _usage; }
    std::shared_ptr<graphics::Model> model() const { return _model; }
    std::shared_ptr<graphics::Walkmesh> walkmesh() const { return _walkmesh; }
    float alpha() const { return _alpha; }
    float projectileSpeed() const { return _projectileSpeed; }
    SceneNodeAnimator &animator() { return _animator; }

    void setVisible(bool visible) override;
    void setDiffuseTexture(const std::shared_ptr<graphics::Texture> &texture);
    void setAlpha(float alpha);
    void setProjectileSpeed(float speed) { _projectileSpeed = speed; }
    void setWalkmesh(std::shared_ptr<graphics::Walkmesh> walkmesh) { _walkmesh = std::move(walkmesh); }

    // Attachments

    std::shared_ptr<ModelSceneNode> attach(const std::string &parent, const std::shared_ptr<graphics::Model> &model, ModelUsage usage);
    std::shared_ptr<ModelSceneNode> attach(ModelNodeSceneNode &parent, const std::shared_ptr<graphics::Model> &model, ModelUsage usage);
    void attach(const std::string &parent, const std::shared_ptr<SceneNode> &node);

    // END Attachments

private:
    IAnimationEventListener *_animEventListener;

    ModelUsage _usage;
    std::shared_ptr<graphics::Model> _model;
    std::shared_ptr<graphics::Walkmesh> _walkmesh;
    SceneNodeAnimator _animator;

    std::unordered_map<uint16_t, ModelNodeSceneNode *> _modelNodeById;
    std::unordered_map<uint16_t, LightSceneNode *> _lightNodeById;
    std::vector<std::shared_ptr<EmitterSceneNode>> _emitters;
    std::unordered_map<uint16_t, std::shared_ptr<ModelSceneNode>> _attachedModels;
    bool _visible { true };
    float _alpha { 1.0f };
    float _projectileSpeed { 0.0f };

    void initModelNodes();

    std::unique_ptr<ModelNodeSceneNode> getModelNodeSceneNode(graphics::ModelNode &node) const;
};

} // namespace scene

} // namespace reone
