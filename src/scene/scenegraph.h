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

#include <map>
#include <memory>
#include <vector>

#include "glm/vec3.hpp"

#include "../render/types.h"

namespace reone {

namespace scene {

class CameraSceneNode;
class EmitterSceneNode;
class LightSceneNode;
class ModelNodeSceneNode;
class SceneNode;

class SceneGraph {
public:
    SceneGraph(const render::GraphicsOptions &opts);

    void render() const;
    void renderNoGlobalUniforms(bool shadowPass) const;

    void clear();

    void addRoot(const std::shared_ptr<SceneNode> &node);
    void removeRoot(const std::shared_ptr<SceneNode> &node);

    void build();
    void prepareFrame();

    std::shared_ptr<CameraSceneNode> activeCamera() const;

    void setActiveCamera(const std::shared_ptr<CameraSceneNode> &camera);
    void setReferenceNode(const std::shared_ptr<SceneNode> &node);

    // Lights

    bool isShadowLightPresent() const;

    void getLightsAt(const glm::vec3 &position, std::vector<LightSceneNode *> &lights) const;

    const glm::vec3 &ambientLightColor() const;
    const glm::vec3 &shadowLightPosition() const;

    void setAmbientLightColor(const glm::vec3 &color);

    // END Lights

private:
    render::GraphicsOptions _opts;
    std::vector<std::shared_ptr<SceneNode>> _roots;
    std::vector<ModelNodeSceneNode *> _opaqueMeshes;
    std::vector<ModelNodeSceneNode *> _transparentMeshes;
    std::vector<ModelNodeSceneNode *> _shadowMeshes;
    std::vector<LightSceneNode *> _lights;
    std::vector<EmitterSceneNode *> _emitters;
    std::shared_ptr<CameraSceneNode> _activeCamera;
    glm::vec3 _ambientLightColor { 0.5f };
    uint32_t _textureId { 0 };
    bool _shadowLightPresent { false };
    glm::vec3 _shadowLightPosition { 0.0f };
    std::shared_ptr<SceneNode> _refNode;

    SceneGraph(const SceneGraph &) = delete;
    SceneGraph &operator=(const SceneGraph &) = delete;

    void refreshMeshesLightsAndEmitters();
    void refreshShadowLight();
};

} // namespace scene

} // namespace reone
