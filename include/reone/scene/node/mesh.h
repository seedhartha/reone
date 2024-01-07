/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "modelnode.h"

namespace reone {

namespace scene {

class ModelSceneNode;

class MeshSceneNode : public ModelNodeSceneNode {
public:
    MeshSceneNode(
        ModelSceneNode &model,
        graphics::ModelNode &modelNode,
        SceneGraph &sceneGraph,
        graphics::GraphicsServices &graphicsSvc,
        audio::AudioServices &audioSvc,
        resource::ResourceServices &resourceSvc) :
        ModelNodeSceneNode(
            modelNode,
            SceneNodeType::Mesh,
            sceneGraph,
            graphicsSvc,
            audioSvc,
            resourceSvc),
        _model(model) {

        init();
    }

    void init();

    void update(float dt) override;

    void render(graphics::IRenderPass &pass);
    void renderShadow(graphics::IRenderPass &pass);

    bool shouldRender() const;
    bool shouldCastShadows() const;

    bool isTransparent() const;

    ModelSceneNode &model() { return _model; }
    const ModelSceneNode &model() const { return _model; }

    void setDiffuseMap(graphics::Texture *texture) override;
    void setEnvironmentMap(graphics::Texture *texture) override;
    void setAlpha(float alpha) { _alpha = alpha; }
    void setSelfIllumColor(glm::vec3 color) { _selfIllumColor = std::move(color); }

private:
    struct NodeTextures {
        graphics::Texture *diffuse {nullptr};
        graphics::Texture *lightmap {nullptr};
        graphics::Texture *envmap {nullptr};
        graphics::Texture *bumpmap {nullptr};
    } _nodeTextures;

    struct DanglyVertex {
        glm::vec3 position {0.0f};
        glm::vec3 displacement {0.0f};
        glm::vec3 velocity {0.0f};
    };

    struct DanglyMesh {
        std::vector<DanglyVertex> vertices;
        glm::vec3 prevWorldPos {0.0f};
    } _dangly;

    ModelSceneNode &_model;

    glm::vec2 _uvOffset {0.0f};
    float _bumpmapCycleTime {0.0f};
    int _bumpmapCycleFrame {0};
    float _alpha {1.0f};
    glm::vec3 _selfIllumColor {0.0f};

    float _windTime {0.0f};

    void initTextures();
    void initDanglyMesh();

    void refreshAdditionalTextures();

    bool isLightingEnabled() const;

    // Animation

    void updateUVAnimation(float dt, const graphics::ModelNode::TriangleMesh &mesh);
    void updateBumpmapAnimation(float dt, const graphics::ModelNode::TriangleMesh &mesh);
    void updateDanglyAnimation(float dt, const graphics::ModelNode::Danglymesh &mesh);

    // END Animation
};

} // namespace scene

} // namespace reone
