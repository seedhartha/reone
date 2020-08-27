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
#include "model/model.h"
#include "shaders.h"

namespace reone {

namespace render {

class SceneGraph;

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

    // Rendering
    void fill(SceneGraph &scene, const glm::mat4 &baseTransform, bool debug);
    void render(const glm::mat4 &transform) const;
    void render(const ModelNode &node, const glm::mat4 &transform) const;

    void animate(const std::string &parent, const std::string &anim, int flags = 0, float speed = 1.0f);
    void animate(const std::string &anim, int flags = 0, float speed = 1.0f);
    void attach(const std::string &parentNode, const std::shared_ptr<Model> &model);
    void changeTexture(const std::string &resRef);
    void update(float dt);
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
        float nextSpeed { 1.0f };
        std::string name;
        int flags { 0 };
        float speed { 1.0f };
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
    bool shouldRender(const ModelNode &node) const;
    glm::mat4 getNodeTransform(const ModelNode &node) const;
    ShaderProgram getShaderProgram(const ModelMesh &mesh, bool skeletal) const;
};

} // namespace render

} // namespace reone
