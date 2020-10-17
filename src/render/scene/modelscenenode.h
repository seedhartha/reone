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

#pragma once

#include <set>
#include <unordered_map>

#include "../aabb.h"
#include "../model/model.h"
#include "../shaders.h"

#include "scenenode.h"
#include "scenenodeanimator.h"

namespace reone {

namespace render {

class ModelNodeSceneNode;

class ModelSceneNode : public SceneNode {
public:
    ModelSceneNode(SceneGraph *sceneGraph, const std::shared_ptr<Model> &model, const std::set<std::string> &skipNodes = std::set<std::string>());

    void update(float dt);
    void render() const override;

    void attach(const std::string &parentNode, const std::shared_ptr<Model> &model);
    void attach(const std::string &parentNode, const std::shared_ptr<SceneNode> &node);
    void fillSceneGraph() override;

    ModelNodeSceneNode *getModelNode(const std::string &name) const;
    ModelNodeSceneNode *getModelNodeByIndex(int index) const;
    bool getNodeAbsolutePosition(const std::string &name, glm::vec3 &position) const;
    glm::vec3 getCenterOfAABB() const;

    const std::string &name() const;
    std::shared_ptr<Model> model() const;
    bool hasTextureOverride() const;
    std::shared_ptr<Texture> textureOverride() const;
    bool isVisible() const;
    bool isOnScreen() const;
    float alpha() const;
    const AABB &aabb() const;

    void setModel(const std::shared_ptr<Model> &model);
    void setTextureOverride(const std::shared_ptr<Texture> &texture);
    void setVisible(bool visible);
    void setOnScreen(bool onScreen);
    void setAlpha(float alpha);

    // Animation

    void playDefaultAnimation();
    void playAnimation(const std::string &name, int flags = 0, float speed = 1.0f);
    void playAnimation(const std::string &parent, const std::string &anim, int flags = 0, float speed = 1.0f);

    void setDefaultAnimation(const std::string &name);

    // END Animation

    // Dynamic lighting

    void updateLighting();
    void setLightingIsDirty();

    bool isLightingEnabled() const;
    const std::vector<ModelNodeSceneNode *> &lightsAffectedBy() const;

    void setLightingEnabled(bool affected);
    void setLightsAffectedBy(const std::vector<ModelNodeSceneNode *> &lights);

    // END Dynamic lighting

private:
    std::shared_ptr<Model> _model;
    SceneNodeAnimator _animator;
    std::unordered_map<uint16_t, ModelNodeSceneNode *> _modelNodeByIndex;
    std::unordered_map<uint16_t, ModelNodeSceneNode *> _modelNodeByNumber;
    std::unordered_map<uint16_t, std::shared_ptr<ModelSceneNode>> _attachedModels;
    std::shared_ptr<Texture> _textureOverride;
    bool _visible { true };
    bool _onScreen { true };
    float _alpha { 1.0f };
    bool _drawAABB { false };
    bool _lightingEnabled { false };
    std::vector<ModelNodeSceneNode *> _lightsAffectedBy;
    bool _lightingDirty { true };

    void initModelNodes();
    std::unique_ptr<ModelNodeSceneNode> getModelNodeSceneNode(ModelNode &node) const;
    void updateAbsoluteTransform() override;
};

} // namespace render

} // namespace reone
