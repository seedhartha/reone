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

#include "../graphics/options.h"
#include "../graphics/shader/shaders.h"

#include "node/camera.h"
#include "node/dummy.h"
#include "node/emitter.h"
#include "node/grass.h"
#include "node/light.h"
#include "node/mesh.h"

namespace reone {

namespace graphics {

class Context;
class Features;
class Materials;
class Meshes;
class PBRIBL;
class Shaders;
class Textures;
class Walkmesh;

} // namespace graphics

namespace scene {

class IAnimationEventListener;
class ModelSceneNode;
class WalkmeshSceneNode;

/**
 * Responsible for managing drawable objects and their relations.
 *
 * @see SceneNode
 */
class SceneGraph : boost::noncopyable {
public:
    SceneGraph(
        std::string name,
        graphics::GraphicsOptions options,
        graphics::Context &context,
        graphics::Features &features,
        graphics::Materials &materials,
        graphics::Meshes &meshes,
        graphics::PBRIBL &pbrIbl,
        graphics::Shaders &shaders,
        graphics::Textures &textures) :
        _name(std::move(name)),
        _options(std::move(options)),
        _context(context),
        _features(features),
        _materials(materials),
        _meshes(meshes),
        _pbrIbl(pbrIbl),
        _shaders(shaders),
        _textures(textures) {
    }

    /**
     * Recursively update the state of this scene graph. Called prior to rendering a frame.
     * This extracts drawable nodes from roots, culls and sorts objects, updates animation, lighting, shadows and etc.
     */
    void update(float dt);

    void draw(bool shadowPass = false);

    const std::string &name() const { return _name; }
    const graphics::GraphicsOptions &options() const { return _options; }
    std::shared_ptr<CameraSceneNode> activeCamera() const { return _activeCamera; }
    graphics::ShaderUniforms uniformsPrototype() const { return _uniformsPrototype; }

    void setUpdateRoots(bool update) { _updateRoots = update; }
    void setActiveCamera(std::shared_ptr<CameraSceneNode> camera) { _activeCamera = std::move(camera); }
    void setUniformsPrototype(graphics::ShaderUniforms &&uniforms) { _uniformsPrototype = uniforms; }

    // Roots

    void clear();
    void addRoot(std::shared_ptr<SceneNode> node);
    void removeRoot(const std::shared_ptr<SceneNode> &node);

    // END Roots

    // Lighting and shadows

    /**
     * Get up to count lights, sorted by priority and proximity to the reference node.
     */
    std::vector<LightSceneNode *> getLightsAt(
        int count = graphics::kMaxLights,
        std::function<bool(const LightSceneNode &)> predicate = [](auto &light) { return true; }) const;

    const glm::vec3 &ambientLightColor() const { return _ambientLightColor; }
    const std::vector<LightSceneNode *> closestLights() const { return _closestLights; }
    const LightSceneNode *shadowLight() const { return _shadowLight; }

    void setAmbientLightColor(glm::vec3 color) { _ambientLightColor = std::move(color); }
    void setLightingRefNode(std::shared_ptr<SceneNode> node) { _lightingRefNode = std::move(node); }

    // END Lighting and shadows

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

    // Factory methods

    std::unique_ptr<DummySceneNode> newDummy(const graphics::ModelNode &modelNode);
    std::unique_ptr<CameraSceneNode> newCamera(std::string name, glm::mat4 projection);

    std::unique_ptr<ModelSceneNode> newModel(
        const graphics::Model &model,
        ModelUsage usage,
        IAnimationEventListener *animEventListener = nullptr);

    std::unique_ptr<WalkmeshSceneNode> newWalkmesh(std::string name, const graphics::Walkmesh &walkmesh);

    // END Factory methods

private:
    std::string _name;
    graphics::GraphicsOptions _options;

    std::vector<std::shared_ptr<SceneNode>> _roots;
    std::shared_ptr<CameraSceneNode> _activeCamera;

    std::vector<MeshSceneNode *> _opaqueMeshes;
    std::vector<MeshSceneNode *> _transparentMeshes;
    std::vector<MeshSceneNode *> _shadowMeshes;
    std::vector<LightSceneNode *> _lights;
    std::vector<EmitterSceneNode *> _emitters;
    std::vector<GrassSceneNode *> _grass;

    std::vector<std::pair<SceneNode *, std::vector<SceneNode *>>> _elements; // particles and grass clusters

    uint32_t _textureId {0};
    bool _updateRoots {true};
    graphics::ShaderUniforms _uniformsPrototype;

    // Services

    graphics::Context &_context;
    graphics::Features &_features;
    graphics::Materials &_materials;
    graphics::Meshes &_meshes;
    graphics::PBRIBL &_pbrIbl;
    graphics::Shaders &_shaders;
    graphics::Textures &_textures;

    // END Services

    // Lighting and shadows

    glm::vec3 _ambientLightColor {0.5f};
    std::shared_ptr<SceneNode> _lightingRefNode; /**< reference node to use when selecting closest light sources */
    std::vector<LightSceneNode *> _closestLights;
    const LightSceneNode *_shadowLight {nullptr};

    // END Lighting and shadows

    // Fog

    bool _fogEnabled {false};
    float _fogNear {0.0f};
    float _fogFar {0.0f};
    glm::vec3 _fogColor {0.0f};

    // END Fog

    void cullRoots();
    void updateLighting();

    void refreshNodeLists();
    void refreshFromSceneNode(const std::shared_ptr<SceneNode> &node);

    void prepareTransparentMeshes();
    void prepareLeafs();
};

} // namespace scene

} // namespace reone
