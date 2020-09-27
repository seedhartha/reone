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

namespace reone {

namespace render {

class LightSceneNode;
class MeshSceneNode;

class ModelSceneNode : public SceneNode {
public:
    struct AnimationState {
        std::string nextAnimation;
        int nextFlags { 0 };
        float nextSpeed { 1.0f };
        std::string name;
        int flags { 0 };
        float speed { 1.0f };
        std::shared_ptr<Animation> animation;
        const Model *model { nullptr };
        float time { 0.0f };
        std::unordered_map<std::string, glm::mat4> localTransforms;
        std::unordered_map<uint16_t, glm::mat4> boneTransforms;
    };

    ModelSceneNode(const std::shared_ptr<Model> &model);

    void attach(const std::string &parentNode, const std::shared_ptr<Model> &model);
    void update(float dt);
    void fillSceneGraph() override;
    void renderImmediate() const override;

    glm::vec3 getNodeAbsolutePosition(const std::string &name) const;

    const std::string &name() const;
    std::shared_ptr<Model> model() const;
    std::shared_ptr<Texture> textureOverride() const;
    bool isVisible() const;
    bool isOnScreen() const;
    float alpha() const;

    void setModel(const std::shared_ptr<Model> &model);
    void setTextureOverride(const std::shared_ptr<Texture> &texture);
    void setVisible(bool visible);
    void setOnScreen(bool onScreen);
    void setAlpha(float alpha);

    // Animation

    void playDefaultAnimation();
    void animate(const std::string &parent, const std::string &anim, int flags = 0, float speed = 1.0f);
    void animate(const std::string &anim, int flags = 0, float speed = 1.0f);

    const AnimationState &animationState() const;

    void setDefaultAnimation(const std::string &name);

    // END Animation

    // Dynamic lighting

    void updateLighting();
    void setLightingIsDirty();

    bool isLightingEnabled() const;
    const std::vector<LightSceneNode *> &lightsAffectedBy() const;

    void setLightingEnabled(bool affected);
    void setLightsAffectedBy(const std::vector<LightSceneNode *> &lights);

    // END Dynamic lighting

private:
    std::shared_ptr<Model> _model;
    std::unordered_map<uint16_t, glm::mat4> _nodeTransforms;
    AnimationState _animState;
    std::unordered_map<uint16_t, std::shared_ptr<MeshSceneNode>> _meshes;
    std::unordered_map<uint16_t, std::shared_ptr<LightSceneNode>> _lights;
    std::unordered_map<uint16_t, std::shared_ptr<ModelSceneNode>> _attachedModels;
    std::shared_ptr<Texture> _textureOverride;
    std::string _defaultAnimation;
    bool _visible { true };
    bool _onScreen { true };
    float _alpha { 1.0f };
    bool _drawAABB { false };
    bool _lightingEnabled { false };
    std::vector<LightSceneNode *> _lightsAffectedBy;
    bool _lightingDirty { true };

    void initChildren();
    void doUpdate(float dt, const std::set<std::string> &skipNodes);
    void startNextAnimation();
    void advanceAnimation(float dt, const std::set<std::string> &skipNodes);
    void updateAnimTransforms(const ModelNode &animNode, const glm::mat4 &transform, float time, const std::set<std::string> &skipNodes);
    void updateNodeTansforms(const ModelNode &node, const glm::mat4 &transform);
    bool shouldRender(const ModelNode &node) const;
    glm::mat4 getNodeTransform(const ModelNode &node) const;
    void updateAbsoluteTransform() override;
};

} // namespace render

} // namespace reone
