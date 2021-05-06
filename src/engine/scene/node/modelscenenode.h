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
#include "../../graphics/shaders.h"
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

    void refreshAABB();
    void signalEvent(const std::string &name);
    void setAppliedForce(glm::vec3 force);

    ModelNodeSceneNode *getModelNode(const std::string &name) const;
    ModelNodeSceneNode *getModelNodeByIndex(int index) const;
    LightSceneNode *getLightNodeByNumber(uint16_t nodeNumber) const;
    std::shared_ptr<ModelSceneNode> getAttachedModel(const std::string &parent) const;
    bool getNodeAbsolutePosition(const std::string &name, glm::vec3 &position) const;
    glm::vec3 getCenterOfAABB() const;
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
    void setProjectileSpeed(float speed);
    void setWalkmesh(std::shared_ptr<graphics::Walkmesh> walkmesh);

    // Attachments

    std::shared_ptr<ModelSceneNode> attach(const std::string &parent, const std::shared_ptr<graphics::Model> &model, ModelUsage usage);
    std::shared_ptr<ModelSceneNode> attach(ModelNodeSceneNode &parent, const std::shared_ptr<graphics::Model> &model, ModelUsage usage);
    void attach(const std::string &parent, const std::shared_ptr<SceneNode> &node);

    // END Attachments

    // Dynamic lighting

    void updateLighting();
    void setLightingIsDirty();

    const std::vector<LightSceneNode *> &lightsAffectedBy() const { return _lightsAffectedBy; }

    void setLightsAffectedBy(const std::vector<LightSceneNode *> &lights);

    // END Dynamic lighting

private:
    IAnimationEventListener *_animEventListener;

    ModelUsage _usage;
    std::shared_ptr<graphics::Model> _model;
    std::shared_ptr<graphics::Walkmesh> _walkmesh;
    SceneNodeAnimator _animator;

    std::unordered_map<uint16_t, ModelNodeSceneNode *> _modelNodeByIndex;
    std::unordered_map<uint16_t, ModelNodeSceneNode *> _modelNodeByNumber;
    std::unordered_map<uint16_t, LightSceneNode *> _lightNodeByNumber;
    std::vector<std::shared_ptr<EmitterSceneNode>> _emitters;
    std::unordered_map<uint16_t, std::shared_ptr<ModelSceneNode>> _attachedModels;
    bool _visible { true };
    float _alpha { 1.0f };
    std::vector<LightSceneNode *> _lightsAffectedBy;
    bool _lightingDirty { true };
    float _projectileSpeed { 0.0f };

    void initModelNodes();
    void updateAbsoluteTransform() override;

    bool isAffectableByLight(const LightSceneNode &light) const;

    std::unique_ptr<ModelNodeSceneNode> getModelNodeSceneNode(graphics::ModelNode &node) const;
};

} // namespace scene

} // namespace reone
