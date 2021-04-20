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

#include <functional>
#include <memory>
#include <set>
#include <vector>

#include <boost/noncopyable.hpp>

#include "glm/vec3.hpp"

#include "../render/shaders.h"
#include "../render/types.h"

#include "grasscluster.h"

namespace reone {

namespace scene {

constexpr float kDefaultExposure = 1.0f;

class CameraSceneNode;
class EmitterSceneNode;
class GrassSceneNode;
class LightSceneNode;
class ModelNodeSceneNode;
class Particle;
class SceneNode;

class SceneGraph : boost::noncopyable {
public:
    SceneGraph(const render::GraphicsOptions &opts);

    void update(float dt);
    void draw(bool shadowPass = false);

    void clear();

    void addRoot(const std::shared_ptr<SceneNode> &node);
    void removeRoot(const std::shared_ptr<SceneNode> &node);

    void setGrass(std::shared_ptr<GrassSceneNode> node);

    /**
     * Prepares this scene graph for rendering the next frame. Meshes, lights
     * and particles are extracted from the root scene nodes and sorted by
     * distance to camera. Lighting sources are selected for each model.
     */
    void prepareFrame();

    const render::GraphicsOptions &options() const { return _opts; }
    std::shared_ptr<CameraSceneNode> activeCamera() const { return _activeCamera; }
    render::ShaderUniforms uniformsPrototype() const { return _uniformsPrototype; }
    float exposure() const { return _exposure; }

    void setActiveCamera(const std::shared_ptr<CameraSceneNode> &camera);
    void setShadowReference(const std::shared_ptr<SceneNode> &reference);
    void setUpdate(bool update);
    void setUniformsPrototype(render::ShaderUniforms &&uniforms);
    void setExposure(float exposure);

    // Lights and shadows

    /**
     * Fills lights vector with up to count lights, sorted by priority and
     * proximity to the reference node.
     */
    void getLightsAt(
        const SceneNode &reference,
        std::vector<LightSceneNode *> &lights,
        int count = render::kMaxLights,
        std::function<bool(const LightSceneNode &)> predicate = [](auto &light) { return true; }) const;

    const glm::vec3 &ambientLightColor() const { return _ambientLightColor; }
    const LightSceneNode *shadowLight() const { return _shadowLight; }
    float shadowStrength() const { return _shadowStrength; }

    void setAmbientLightColor(const glm::vec3 &color);

    // END Lights and shadows

    // Fog

    bool isFogEnabled() const { return _fogEnabled; }

    float fogNear() const { return _fogNear; }
    float fogFar() const { return _fogFar; }
    const glm::vec3 &fogColor() const { return _fogColor; }

    void setFogEnabled(bool enabled) { _fogEnabled = enabled; }
    void setFogNear(float near) { _fogNear = near; }
    void setFogFar(float far) { _fogFar = far; }
    void setFogColor(glm::vec3 color) { _fogColor = std::move(color); }

    // END Fog

private:
    render::GraphicsOptions _opts;

    std::vector<std::shared_ptr<SceneNode>> _roots;
    std::shared_ptr<GrassSceneNode> _grass;
    std::shared_ptr<CameraSceneNode> _activeCamera;

    std::vector<ModelNodeSceneNode *> _opaqueMeshes;
    std::vector<ModelNodeSceneNode *> _transparentMeshes;
    std::vector<ModelNodeSceneNode *> _shadowMeshes;
    std::vector<LightSceneNode *> _lights;
    std::vector<EmitterSceneNode *> _emitters;
    std::vector<std::pair<EmitterSceneNode *, std::vector<Particle *>>> _particles;
    std::vector<GrassCluster> _grassClusters;

    glm::vec3 _ambientLightColor { 0.5f };
    uint32_t _textureId { 0 };
    bool _update { true };
    render::ShaderUniforms _uniformsPrototype;
    float _exposure { kDefaultExposure };

    // Shadows

    const LightSceneNode *_shadowLight { nullptr };
    std::shared_ptr<SceneNode> _shadowReference;
    float _shadowStrength { 1.0f };
    bool _shadowFading { false };

    // END Shadows

    // Fog

    bool _fogEnabled { false };
    float _fogNear { 0.0f };
    float _fogFar { 0.0f };
    glm::vec3 _fogColor { 0.0f };

    // END Fog

    void refreshNodeLists();
    void refreshFromSceneNode(const std::shared_ptr<SceneNode> &node);
    void refreshShadowLight();

    void prepareOpaqueMeshes();
    void prepareTransparentMeshes();
    void prepareParticles();
    void prepareGrass();

    inline void flushEmitterParticles(EmitterSceneNode *emitter, std::vector<Particle *> &particles);
};

} // namespace scene

} // namespace reone
