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

#include "reone/scene/render/pipeline.h"

#include "fogproperties.h"
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

class IRenderPass;
class Walkmesh;

} // namespace graphics

namespace audio {

struct AudioServices;

}

namespace resource {

struct ResourceServices;

}

namespace scene {

struct Collision;

class IAnimationEventListener;
class IRenderPipelineFactory;

class ISceneGraph {
public:
    virtual ~ISceneGraph() = default;

    virtual void update(float dt) = 0;
    virtual graphics::Texture &render(const glm::ivec2 &dim) = 0;

    virtual void clear() = 0;

    virtual void addRoot(std::shared_ptr<ModelSceneNode> node) = 0;
    virtual void addRoot(std::shared_ptr<WalkmeshSceneNode> node) = 0;
    virtual void addRoot(std::shared_ptr<TriggerSceneNode> node) = 0;
    virtual void addRoot(std::shared_ptr<GrassSceneNode> node) = 0;
    virtual void addRoot(std::shared_ptr<SoundSceneNode> node) = 0;

    virtual void removeRoot(ModelSceneNode &node) = 0;
    virtual void removeRoot(WalkmeshSceneNode &node) = 0;
    virtual void removeRoot(TriggerSceneNode &node) = 0;
    virtual void removeRoot(GrassSceneNode &node) = 0;
    virtual void removeRoot(SoundSceneNode &node) = 0;

    virtual bool testElevation(const glm::vec2 &position, Collision &outCollision) const = 0;
    virtual bool testLineOfSight(const glm::vec3 &origin, const glm::vec3 &dest, Collision &outCollision) const = 0;
    virtual bool testWalk(const glm::vec3 &origin, const glm::vec3 &dest, const IUser *excludeUser, Collision &outCollision) const = 0;

    virtual ModelSceneNode *pickModelAt(int x, int y, IUser *except = nullptr) const = 0;

    virtual const std::string &name() const = 0;

    virtual void setAmbientLightColor(glm::vec3 color) = 0;
    virtual void setFog(FogProperties fog) = 0;

    virtual void setWalkableSurfaces(std::set<uint32_t> surfaces) = 0;
    virtual void setWalkcheckSurfaces(std::set<uint32_t> surfaces) = 0;
    virtual void setLineOfSightSurfaces(std::set<uint32_t> surfaces) = 0;

    virtual void setActiveCamera(CameraSceneNode *camera) = 0;
    virtual void setUpdateRoots(bool update) = 0;

    virtual void setRenderAABB(bool render) = 0;
    virtual void setRenderWalkmeshes(bool render) = 0;
    virtual void setRenderTriggers(bool render) = 0;

    virtual std::shared_ptr<CameraSceneNode> newCamera() = 0;
    virtual std::shared_ptr<ModelSceneNode> newModel(graphics::Model &model, ModelUsage usage) = 0;
    virtual std::shared_ptr<WalkmeshSceneNode> newWalkmesh(graphics::Walkmesh &walkmesh) = 0;
    virtual std::shared_ptr<TriggerSceneNode> newTrigger(std::vector<glm::vec3> geometry) = 0;
    virtual std::shared_ptr<SoundSceneNode> newSound() = 0;
    virtual std::shared_ptr<DummySceneNode> newDummy(graphics::ModelNode &modelNode) = 0;
    virtual std::shared_ptr<MeshSceneNode> newMesh(ModelSceneNode &model, graphics::ModelNode &modelNode) = 0;
    virtual std::shared_ptr<LightSceneNode> newLight(ModelSceneNode &model, graphics::ModelNode &modelNode) = 0;
    virtual std::shared_ptr<EmitterSceneNode> newEmitter(graphics::ModelNode &modelNode) = 0;
    virtual std::shared_ptr<ParticleSceneNode> newParticle(EmitterSceneNode &emitter) = 0;
    virtual std::shared_ptr<GrassSceneNode> newGrass(GrassProperties properties, graphics::ModelNode &aabbNode) = 0;
    virtual std::shared_ptr<GrassClusterSceneNode> newGrassCluster(GrassSceneNode &grass) = 0;
};

class SceneGraph : public ISceneGraph, boost::noncopyable {
public:
    SceneGraph(
        std::string name,
        IRenderPipelineFactory &renderPipelineFactory,
        graphics::GraphicsOptions &graphicsOpt,
        graphics::GraphicsServices &graphicsSvc,
        audio::AudioServices &audioSvc,
        resource::ResourceServices &resourceSvc) :
        _name(std::move(name)),
        _renderPipelineFactory(renderPipelineFactory),
        _graphicsOpt(graphicsOpt),
        _graphicsSvc(graphicsSvc),
        _audioSvc(audioSvc),
        _resourceSvc(resourceSvc) {
    }

    void update(float dt) override;
    graphics::Texture &render(const glm::ivec2 &dim) override;

    void renderShadows(graphics::IRenderPass &pass);
    void renderOpaque(graphics::IRenderPass &pass);
    void renderTransparent(graphics::IRenderPass &pass);
    void renderLensFlares(graphics::IRenderPass &pass);

    const std::string &name() const override {
        return _name;
    }

    CameraSceneNode *activeCamera() const {
        return _activeCamera;
    }

    std::shared_ptr<graphics::Camera> camera() const {
        return _activeCamera ? _activeCamera->camera() : nullptr;
    }

    void setActiveCamera(CameraSceneNode *camera) override { _activeCamera = camera; }
    void setUpdateRoots(bool update) override { _updateRoots = update; }

    void setRenderAABB(bool render) override { _renderAABB = render; }
    void setRenderWalkmeshes(bool render) override { _renderWalkmeshes = render; }
    void setRenderTriggers(bool render) override { _renderTriggers = render; }

    // Roots

    void clear() override;

    void addRoot(std::shared_ptr<ModelSceneNode> node) override;
    void addRoot(std::shared_ptr<WalkmeshSceneNode> node) override;
    void addRoot(std::shared_ptr<TriggerSceneNode> node) override;
    void addRoot(std::shared_ptr<GrassSceneNode> node) override;
    void addRoot(std::shared_ptr<SoundSceneNode> node) override;

    void removeRoot(ModelSceneNode &node) override;
    void removeRoot(WalkmeshSceneNode &node) override;
    void removeRoot(TriggerSceneNode &node) override;
    void removeRoot(GrassSceneNode &node) override;
    void removeRoot(SoundSceneNode &node) override;

    // END Roots

    // Lighting

    const glm::vec3 &ambientLightColor() const { return _ambientLightColor; }

    void setAmbientLightColor(glm::vec3 color) override { _ambientLightColor = std::move(color); }

    // END Lighting

    // Fog

    bool isFogEnabled() const {
        return _fog.enabled;
    }

    float fogNear() const {
        return _fog.nearPlane;
    }

    float fogFar() const {
        return _fog.farPlane;
    }

    const glm::vec3 &fogColor() const {
        return _fog.color;
    }

    void setFog(FogProperties fog) override {
        _fog = std::move(fog);
    }

    // END Fog

    // Shadows

    bool hasShadowLight() const { return _shadowLight; }
    bool isShadowLightDirectional() const { return _shadowLight->isDirectional(); }

    glm::vec3 shadowLightPosition() const { return _shadowLight->getOrigin(); }
    float shadowStrength() const { return _shadowStrength; }
    float shadowRadius() const { return _shadowLight->radius(); }

    // END Shadows

    // Collision detection and object picking

    bool testElevation(const glm::vec2 &position, Collision &outCollision) const override;
    bool testLineOfSight(const glm::vec3 &origin, const glm::vec3 &dest, Collision &outCollision) const override;
    bool testWalk(const glm::vec3 &origin, const glm::vec3 &dest, const IUser *excludeUser, Collision &outCollision) const override;

    ModelSceneNode *pickModelAt(int x, int y, IUser *except = nullptr) const override;

    void setWalkableSurfaces(std::set<uint32_t> surfaces) override { _walkableSurfaces = std::move(surfaces); }
    void setWalkcheckSurfaces(std::set<uint32_t> surfaces) override { _walkcheckSurfaces = std::move(surfaces); }
    void setLineOfSightSurfaces(std::set<uint32_t> surfaces) override { _lineOfSightSurfaces = std::move(surfaces); }

    // END Collision detection and object picking

    // Factory methods

    std::shared_ptr<CameraSceneNode> newCamera() override;
    std::shared_ptr<ModelSceneNode> newModel(graphics::Model &model, ModelUsage usage) override;
    std::shared_ptr<WalkmeshSceneNode> newWalkmesh(graphics::Walkmesh &walkmesh) override;
    std::shared_ptr<TriggerSceneNode> newTrigger(std::vector<glm::vec3> geometry) override;
    std::shared_ptr<SoundSceneNode> newSound() override;

    std::shared_ptr<DummySceneNode> newDummy(graphics::ModelNode &modelNode) override;
    std::shared_ptr<MeshSceneNode> newMesh(ModelSceneNode &model, graphics::ModelNode &modelNode) override;
    std::shared_ptr<LightSceneNode> newLight(ModelSceneNode &model, graphics::ModelNode &modelNode) override;

    std::shared_ptr<EmitterSceneNode> newEmitter(graphics::ModelNode &modelNode) override;
    std::shared_ptr<ParticleSceneNode> newParticle(EmitterSceneNode &emitter) override;

    std::shared_ptr<GrassSceneNode> newGrass(GrassProperties properties, graphics::ModelNode &aabbNode) override;
    std::shared_ptr<GrassClusterSceneNode> newGrassCluster(GrassSceneNode &grass) override;

    // END Factory methods

private:
    std::string _name;
    IRenderPipelineFactory &_renderPipelineFactory;
    graphics::GraphicsOptions &_graphicsOpt;
    graphics::GraphicsServices &_graphicsSvc;
    audio::AudioServices &_audioSvc;
    resource::ResourceServices &_resourceSvc;

    std::unique_ptr<IRenderPipeline> _renderPipeline;

    bool _updateRoots {true};

    bool _renderAABB {false};
    bool _renderWalkmeshes {false};
    bool _renderTriggers {false};

    std::set<std::shared_ptr<SceneNode>> _nodes;

    CameraSceneNode *_activeCamera {nullptr};
    std::vector<LightSceneNode *> _flareLights;

    // Roots

    std::list<std::shared_ptr<ModelSceneNode>> _modelRoots;
    std::list<std::shared_ptr<WalkmeshSceneNode>> _walkmeshRoots;
    std::list<std::shared_ptr<TriggerSceneNode>> _triggerRoots;
    std::list<std::shared_ptr<GrassSceneNode>> _grassRoots;
    std::list<std::shared_ptr<SoundSceneNode>> _soundRoots;

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

    glm::mat4 _shadowLightSpace[graphics::kNumShadowLightSpace] {glm::mat4(1.0f)};
    glm::vec4 _shadowCascadeFarPlanes {glm::vec4(0.0f)};

    // END Shadows

    // Fog

    FogProperties _fog;

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

    void computeLightSpaceMatrices();

    std::vector<LightSceneNode *> computeClosestLights(int count, const std::function<bool(const LightSceneNode &, float)> &pred) const;

    template <class T, class... Params>
    std::shared_ptr<T> newSceneNode(Params... params) {
        auto node = std::make_shared<T>(params..., *this, _graphicsSvc, _audioSvc, _resourceSvc);
        _nodes.insert(node);
        return node;
    }
};

} // namespace scene

} // namespace reone
