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

#include "../../graphics/modelnode.h"

#include "modelnode.h"

namespace reone {

namespace graphics {

class Texture;
class Textures;

} // namespace graphics

namespace scene {

class ModelSceneNode;

class MeshSceneNode : public ModelNodeSceneNode {
public:
    MeshSceneNode(
        ModelSceneNode &model,
        std::shared_ptr<graphics::ModelNode> modelNode,
        SceneGraph &sceneGraph,
        graphics::GraphicsContext &graphicsContext,
        graphics::Meshes &meshes,
        graphics::Shaders &shaders,
        graphics::Textures &textures,
        graphics::UniformBuffers &uniformBuffers) :
        ModelNodeSceneNode(
            modelNode,
            SceneNodeType::Mesh,
            sceneGraph,
            graphicsContext,
            meshes,
            shaders,
            textures,
            uniformBuffers),
        _model(model) {

        init();
    }

    void init();

    void update(float dt) override;

    void draw();
    void drawShadow();

    bool shouldRender() const;
    bool shouldCastShadows() const;

    bool isTransparent() const;

    ModelSceneNode &model() { return _model; }
    const ModelSceneNode &model() const { return _model; }

    void setDiffuseMap(std::shared_ptr<graphics::Texture> texture) override;
    void setEnvironmentMap(std::shared_ptr<graphics::Texture> texture) override;
    void setAlpha(float alpha) { _alpha = alpha; }
    void setSelfIllumColor(glm::vec3 color) { _selfIllumColor = std::move(color); }

private:
    struct NodeTextures {
        std::shared_ptr<graphics::Texture> diffuse;
        std::shared_ptr<graphics::Texture> lightmap;
        std::shared_ptr<graphics::Texture> envmap;
        std::shared_ptr<graphics::Texture> bumpmap;
    } _nodeTextures;

    ModelSceneNode &_model;

    glm::vec2 _uvOffset {0.0f};
    float _bumpmapCycleTime {0.0f};
    int _bumpmapCycleFrame {0};
    float _alpha {1.0f};
    glm::vec3 _selfIllumColor {0.0f};

    void initTextures();

    void refreshAdditionalTextures();

    bool isLightingEnabled() const;

    // Animation

    void updateUVAnimation(float dt, const graphics::ModelNode::TriangleMesh &mesh);
    void updateBumpmapAnimation(float dt, const graphics::ModelNode::TriangleMesh &mesh);

    // END Animation
};

} // namespace scene

} // namespace reone
