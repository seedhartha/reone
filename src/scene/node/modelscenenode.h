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

#include "../../render/aabb.h"
#include "../../render/model/model.h"
#include "../../render/shaders.h"

#include "../animation/scenenodeanimator.h"

#include "scenenode.h"

namespace reone {

namespace scene {

class EmitterSceneNode;
class LightSceneNode;
class ModelNodeSceneNode;

class ModelSceneNode : public SceneNode {
public:
    ModelSceneNode(
        SceneGraph *sceneGraph,
        const std::shared_ptr<render::Model> &model,
        std::set<std::string> ignoreNodes = std::set<std::string>());

    void update(float dt) override;
    void render() const override;

    std::shared_ptr<ModelSceneNode> attach(const std::string &parent, const std::shared_ptr<render::Model> &model);
    void attach(const std::string &parent, const std::shared_ptr<SceneNode> &node);

    void refreshAABB();
    void signalEvent(const std::string &name);

    bool isOnScreen() const { return _onScreen; }

    ModelNodeSceneNode *getModelNode(const std::string &name) const;
    ModelNodeSceneNode *getModelNodeByIndex(int index) const;
    std::shared_ptr<ModelSceneNode> getAttachedModel(const std::string &parent) const;
    bool getNodeAbsolutePosition(const std::string &name, glm::vec3 &position) const;
    glm::vec3 getCenterOfAABB() const;
    const std::string &getName() const;

    std::shared_ptr<render::Model> model() const { return _model; }
    std::shared_ptr<render::Texture> textureOverride() const { return _textureOverride; }
    float alpha() const { return _alpha; }
    const AABB &aabb() const { return _aabb; }
    float projectileSpeed() const { return _projectileSpeed; }
    SceneNodeAnimator &animator() { return _animator; }

    void setVisible(bool visible) override;

    void setTextureOverride(const std::shared_ptr<render::Texture> &texture);
    void setOnScreen(bool onScreen);
    void setAlpha(float alpha);
    void setProjectileSpeed(float speed);

    // Dynamic lighting

    void updateLighting();
    void setLightingIsDirty();

    bool isLightingEnabled() const { return _lightingEnabled; }
    const std::vector<LightSceneNode *> &lightsAffectedBy() const { return _lightsAffectedBy; }

    void setLightingEnabled(bool affected);
    void setLightsAffectedBy(const std::vector<LightSceneNode *> &lights);

    // END Dynamic lighting

private:
    std::shared_ptr<render::Model> _model;
    SceneNodeAnimator _animator;
    std::unordered_map<uint16_t, ModelNodeSceneNode *> _modelNodeByIndex;
    std::unordered_map<uint16_t, ModelNodeSceneNode *> _modelNodeByNumber;
    std::vector<std::shared_ptr<EmitterSceneNode>> _emitters;
    std::unordered_map<uint16_t, std::shared_ptr<ModelSceneNode>> _attachedModels;
    std::shared_ptr<render::Texture> _textureOverride;
    bool _visible { true };
    bool _onScreen { true };
    float _alpha { 1.0f };
    bool _lightingEnabled { false };
    std::vector<LightSceneNode *> _lightsAffectedBy;
    bool _lightingDirty { true };
    AABB _aabb;
    float _projectileSpeed;

    void initModelNodes();
    std::unique_ptr<ModelNodeSceneNode> getModelNodeSceneNode(render::ModelNode &node) const;
    void updateAbsoluteTransform() override;
};

} // namespace scene

} // namespace reone
