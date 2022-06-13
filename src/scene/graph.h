/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "../graphics/scene.h"

#include "node/camera.h"
#include "node/dummy.h"
#include "node/emitter.h"
#include "node/grass.h"
#include "node/light.h"
#include "node/mesh.h"
#include "node/model.h"
#include "node/sound.h"
#include "node/trigger.h"
#include "node/walkmesh.h"
#include "user.h"

namespace reone {

namespace graphics {

struct GraphicsOptions;
struct GraphicsServices;

class Walkmesh;

} // namespace graphics

namespace audio {

struct AudioServices;

}

namespace scene {

struct Collision;

class IAnimationEventListener;

class ISceneGraph {
public:
    virtual ModelSceneNode *pickModelAt(int x, int y, IUser *except = nullptr) const = 0;
};

class SceneGraph : public ISceneGraph, public graphics::IScene, boost::noncopyable {
public:
    SceneGraph(
        std::string name,
        graphics::GraphicsOptions &graphicsOpt,
        graphics::GraphicsServices &graphicsSvc,
        audio::AudioServices &audioSvc) :
        _name(std::move(name)),
        _graphicsOpt(graphicsOpt),
        _graphicsSvc(graphicsSvc),
        _audioSvc(audioSvc) {
    }

    void update(float dt);

    void drawShadows() override;
    void drawOpaque() override;
    void drawTransparent() override;
    void drawLensFlares() override;

    const std::string &name() const {
        return _name;
    }

    CameraSceneNode *activeCamera() const {
        return _activeCamera;
    }

    std::shared_ptr<graphics::Camera> camera() const override {
        return _activeCamera ? _activeCamera->camera() : nullptr;
    }

    void setActiveCamera(CameraSceneNode *camera) { _activeCamera = camera; }
    void setUpdateRoots(bool update) { _updateRoots = update; }

    void setDrawAABB(bool draw) { _drawAABB = draw; }
    void setDrawWalkmeshes(bool draw) { _drawWalkmeshes = draw; }
    void setDrawTriggers(bool draw) { _drawTriggers = draw; }

    // Roots

    void clear();

    void addRoot(ModelSceneNode &node);
    void addRoot(WalkmeshSceneNode &node);
    void addRoot(TriggerSceneNode &node);
    void addRoot(GrassSceneNode &node);
    void addRoot(SoundSceneNode &node);

    void removeRoot(ModelSceneNode &node);
    void removeRoot(WalkmeshSceneNode &node);
    void removeRoot(TriggerSceneNode &node);
    void removeRoot(GrassSceneNode &node);
    void removeRoot(SoundSceneNode &node);

    // END Roots

    // Lighting

    void fillLightingUniforms() override;

    const glm::vec3 &ambientLightColor() const override { return _ambientLightColor; }

    void setAmbientLightColor(glm::vec3 color) { _ambientLightColor = std::move(color); }

    // END Lighting

    // Fog

    bool isFogEnabled() const override { return _fogEnabled; }

    float fogNear() const override { return _fogNear; }
    float fogFar() const override { return _fogFar; }
    const glm::vec3 &fogColor() const override { return _fogColor; }

    void setFogEnabled(bool enabled) { _fogEnabled = enabled; }
    void setFogNear(float near) { _fogNear = near; }
    void setFogFar(float far) { _fogFar = far; }
    void setFogColor(glm::vec3 color) { _fogColor = std::move(color); }

    // END Fog

    // Shadows

    bool hasShadowLight() const override { return _shadowLight; }
    bool isShadowLightDirectional() const override { return _shadowLight->isDirectional(); }

    glm::vec3 shadowLightPosition() const override { return _shadowLight->getOrigin(); }
    float shadowStrength() const override { return _shadowStrength; }
    float shadowRadius() const override { return _shadowLight->radius(); }

    // END Shadows

    // Collision detection and object picking

    bool testElevation(const glm::vec2 &position, Collision &outCollision) const;
    bool testLineOfSight(const glm::vec3 &origin, const glm::vec3 &dest, Collision &outCollision) const;
    bool testWalk(const glm::vec3 &origin, const glm::vec3 &dest, const IUser *excludeUser, Collision &outCollision) const;

    ModelSceneNode *pickModelAt(int x, int y, IUser *except = nullptr) const override;

    void setWalkableSurfaces(std::set<uint32_t> surfaces) { _walkableSurfaces = std::move(surfaces); }
    void setWalkcheckSurfaces(std::set<uint32_t> surfaces) { _walkcheckSurfaces = std::move(surfaces); }
    void setLineOfSightSurfaces(std::set<uint32_t> surfaces) { _lineOfSightSurfaces = std::move(surfaces); }

    // END Collision detection and object picking

    // Factory methods

    std::shared_ptr<CameraSceneNode> newCamera();
    std::shared_ptr<ModelSceneNode> newModel(graphics::Model &model, ModelUsage usage);
    std::shared_ptr<WalkmeshSceneNode> newWalkmesh(graphics::Walkmesh &walkmesh);
    std::shared_ptr<TriggerSceneNode> newTrigger(std::vector<glm::vec3> geometry);
    std::shared_ptr<SoundSceneNode> newSound();

    std::shared_ptr<DummySceneNode> newDummy(graphics::ModelNode &modelNode);
    std::shared_ptr<MeshSceneNode> newMesh(ModelSceneNode &model, graphics::ModelNode &modelNode);
    std::shared_ptr<LightSceneNode> newLight(ModelSceneNode &model, graphics::ModelNode &modelNode);

    std::shared_ptr<EmitterSceneNode> newEmitter(graphics::ModelNode &modelNode);
    std::shared_ptr<ParticleSceneNode> newParticle(EmitterSceneNode &emitter);

    std::shared_ptr<GrassSceneNode> newGrass(GrassProperties properties, graphics::ModelNode &aabbNode);
    std::shared_ptr<GrassClusterSceneNode> newGrassCluster(GrassSceneNode &grass);

    // END Factory methods

private:
    std::string _name;
    graphics::GraphicsOptions &_graphicsOpt;
    graphics::GraphicsServices &_graphicsSvc;
    audio::AudioServices &_audioSvc;

    bool _updateRoots {true};

    bool _drawAABB {false};
    bool _drawWalkmeshes {false};
    bool _drawTriggers {false};

    std::set<std::shared_ptr<SceneNode>> _nodes;

    CameraSceneNode *_activeCamera {nullptr};
    std::vector<LightSceneNode *> _flareLights;

    // Roots

    std::set<ModelSceneNode *> _modelRoots;
    std::list<WalkmeshSceneNode *> _walkmeshRoots;
    std::set<TriggerSceneNode *> _triggerRoots;
    std::set<GrassSceneNode *> _grassRoots;
    std::set<SoundSceneNode *> _soundRoots;

    // END Roots

    // Leafs

    std::vector<MeshSceneNode *> _opaqueMeshes;
    std::vector<MeshSceneNode *> _transparentMeshes;
    std::vector<MeshSceneNode *> _shadowMeshes;
    std::vector<LightSceneNode *> _lights;
    std::vector<EmitterSceneNode *> _emitters;

    std::vector<std::pair<SceneNode *, std::vector<SceneNode *>>> _opaqueLeafs;
    std::vector<std::pair<SceneNode *, std::vector<SceneNode *>>> _transparentLeafs;

    // END Leafs

    // Lighting

    glm::vec3 _ambientLightColor {0.5f};

    std::vector<LightSceneNode *> _activeLights;

    // END Lighting

    // Shadows

    bool _shadowActive {false};
    float _shadowStrength {0.0f};

    LightSceneNode *_shadowLight {nullptr};

    // END Shadows

    // Fog

    bool _fogEnabled {false};
    float _fogNear {0.0f};
    float _fogFar {0.0f};
    glm::vec3 _fogColor {0.0f};

    // END Fog

    // Surfaces

    std::set<uint32_t> _walkableSurfaces;
    std::set<uint32_t> _walkcheckSurfaces;
    std::set<uint32_t> _lineOfSightSurfaces;

    // END Surfaces

    void cullRoots();

    void refresh();
    void refreshFromNode(SceneNode &node);

    void updateLighting();
    void updateShadowLight(float dt);
    void updateFlareLights();
    void updateSounds();

    void prepareOpaqueLeafs();
    void prepareTransparentLeafs();

    std::vector<LightSceneNode *> computeClosestLights(int count, const std::function<bool(const LightSceneNode &, float)> &pred) const;

    template <class T, class... Params>
    std::shared_ptr<T> newSceneNode(Params... params) {
        auto node = std::make_shared<T>(params..., *this, _graphicsSvc, _audioSvc);
        _nodes.insert(node);
        return std::move(node);
    }
};

} // namespace scene

} // namespace reone
