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

#include "aabb.h"
#include "model.h"
#include "shadermanager.h"

namespace reone {

namespace render {

class ModelInstance;

struct RenderListItem {
    const ModelInstance *model { nullptr };
    const ModelNode *node { nullptr };
    glm::mat4 transform { 1.0f };
    glm::vec3 center { 0.0f };

    RenderListItem() = default;
    RenderListItem(const ModelInstance *model, const ModelNode *node, const glm::mat4 &transform);
};

typedef std::vector<RenderListItem> RenderList;

/**
 * Encapsulates model state changes (e.g. animation, attachments).
 * Contains a pointer to the actual model.
 *
 * @see reone::render::Model
 * @see reone::render::AnimationState
 */
class ModelInstance {
public:
    ModelInstance(const std::shared_ptr<Model> &model);

    void animate(const std::string &name, int flags = 0);
    void attach(const std::string &parentNode, const std::shared_ptr<Model> &model);
    void changeTexture(const std::string &resRef);
    void update(float dt);
    void fillRenderLists(const glm::mat4 &transform, RenderList &opaque, RenderList &transparent);
    void initGL();
    void render(const ModelNode &node, const glm::mat4 &transform, bool debug) const;
    void playDefaultAnimation();

    void show();
    void hide();

    void setAlpha(float alpha);
    void setDefaultAnimation(const std::string &name);

    glm::vec3 getNodeAbsolutePosition(const std::string &name) const;

    const std::string &name() const;
    std::shared_ptr<Model> model() const;
    bool visible() const;

private:
    struct AnimationState {
        std::string nextAnimation;
        int nextFlags { 0 };
        std::string name;
        int flags { 0 };
        std::shared_ptr<Animation> animation;
        const Model *model { nullptr };
        float time { 0.0f };
        std::map<std::string, glm::mat4> localTransforms;
    };

    std::shared_ptr<Model> _model;
    std::map<uint16_t, glm::mat4> _nodeTransforms;
    std::map<uint16_t, glm::mat4> _boneTransforms;
    AnimationState _animState;
    std::map<uint16_t, std::unique_ptr<ModelInstance>> _attachedModels;
    std::shared_ptr<Texture> _textureOverride;
    std::string _defaultAnimation;
    bool _visible { true };
    float _alpha { 1.0f };
    bool _drawAABB { false };

    void doUpdate(float dt, const std::set<std::string> &skipNodes);
    void startNextAnimation();
    void advanceAnimation(float dt, const std::set<std::string> &skipNodes);
    void updateAnimTransforms(const ModelNode &animNode, const glm::mat4 &transform, float time, const std::set<std::string> &skipNodes);
    void updateNodeTansforms(const ModelNode &node, const glm::mat4 &transform);
    void fillRenderLists(const ModelNode &node, const glm::mat4 &transform, RenderList &opaque, RenderList &transparent);
    glm::mat4 getNodeTransform(const ModelNode &node) const;
    void renderMesh(const ModelNode &node, const glm::mat4 &transform) const;
    ShaderProgram getShaderProgram(const ModelMesh &mesh, bool skeletal) const;
};

} // namespace render

} // namespace reone
