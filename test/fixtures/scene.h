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

#include <gmock/gmock.h>

#include "reone/scene/di/services.h"
#include "reone/scene/graph.h"
#include "reone/scene/graphs.h"
#include "reone/scene/render/pipeline.h"

namespace reone {

namespace scene {

class MockSceneGraph : public ISceneGraph, boost::noncopyable {
public:
    MOCK_METHOD(void, update, (float dt), (override));
    MOCK_METHOD(graphics::Texture &, render, (const glm::ivec2 &dim), (override));

    MOCK_METHOD(void, clear, (), (override));

    MOCK_METHOD(void, addRoot, (std::shared_ptr<ModelSceneNode>), (override));
    MOCK_METHOD(void, addRoot, (std::shared_ptr<WalkmeshSceneNode>), (override));
    MOCK_METHOD(void, addRoot, (std::shared_ptr<TriggerSceneNode>), (override));
    MOCK_METHOD(void, addRoot, (std::shared_ptr<GrassSceneNode>), (override));
    MOCK_METHOD(void, addRoot, (std::shared_ptr<SoundSceneNode>), (override));

    MOCK_METHOD(void, removeRoot, (ModelSceneNode &), (override));
    MOCK_METHOD(void, removeRoot, (WalkmeshSceneNode &), (override));
    MOCK_METHOD(void, removeRoot, (TriggerSceneNode &), (override));
    MOCK_METHOD(void, removeRoot, (GrassSceneNode &), (override));
    MOCK_METHOD(void, removeRoot, (SoundSceneNode &), (override));

    MOCK_METHOD(std::optional<std::reference_wrapper<ModelSceneNode>>, modelByExternalId, (void *), (override));
    MOCK_METHOD(std::vector<std::reference_wrapper<WalkmeshSceneNode>>, walkmeshesByExternalId, (void *), (override));

    MOCK_METHOD(bool, testElevation, (const glm::vec2 &, Collision &), (const override));
    MOCK_METHOD(bool, testLineOfSight, (const glm::vec3 &, const glm::vec3 &, Collision &), (const override));
    MOCK_METHOD(bool, testWalk, (const glm::vec3 &, const glm::vec3 &, const IUser *, Collision &), (const override));

    MOCK_METHOD(ModelSceneNode *, pickModelAt, (int, int, IUser *), (const override));
    MOCK_METHOD(std::optional<std::reference_wrapper<ModelSceneNode>>, pickModelRay, (const glm::vec3 &, const glm::vec3 &), (const override));

    MOCK_METHOD(const std::string &, name, (), (const override));
    MOCK_METHOD(std::optional<std::reference_wrapper<CameraSceneNode>>, camera, (), (override));

    MOCK_METHOD(void, setAmbientLightColor, (glm::vec3), (override));
    MOCK_METHOD(void, setFog, (FogProperties fog), (override));

    MOCK_METHOD(void, setWalkableSurfaces, (std::set<uint32_t>), (override));
    MOCK_METHOD(void, setWalkcheckSurfaces, (std::set<uint32_t>), (override));
    MOCK_METHOD(void, setLineOfSightSurfaces, (std::set<uint32_t>), (override));

    MOCK_METHOD(void, setActiveCamera, (CameraSceneNode *), (override));
    MOCK_METHOD(void, setUpdateRoots, (bool), (override));

    MOCK_METHOD(void, setRenderAABB, (bool), (override));
    MOCK_METHOD(void, setRenderWalkmeshes, (bool), (override));
    MOCK_METHOD(void, setRenderTriggers, (bool), (override));

    MOCK_METHOD(std::shared_ptr<CameraSceneNode>, newCamera, (), (override));
    MOCK_METHOD(std::shared_ptr<ModelSceneNode>, newModel, (graphics::Model &, ModelUsage), (override));
    MOCK_METHOD(std::shared_ptr<WalkmeshSceneNode>, newWalkmesh, (graphics::Walkmesh & walkmesh), (override));
    MOCK_METHOD(std::shared_ptr<TriggerSceneNode>, newTrigger, (std::vector<glm::vec3> geometry), (override));
    MOCK_METHOD(std::shared_ptr<SoundSceneNode>, newSound, (), (override));
    MOCK_METHOD(std::shared_ptr<DummySceneNode>, newDummy, (graphics::ModelNode & modelNode), (override));
    MOCK_METHOD(std::shared_ptr<MeshSceneNode>, newMesh, (ModelSceneNode & model, graphics::ModelNode &modelNode), (override));
    MOCK_METHOD(std::shared_ptr<LightSceneNode>, newLight, (ModelSceneNode & model, graphics::ModelNode &modelNode), (override));
    MOCK_METHOD(std::shared_ptr<EmitterSceneNode>, newEmitter, (graphics::ModelNode & modelNode), (override));
    MOCK_METHOD(std::shared_ptr<ParticleSceneNode>, newParticle, (EmitterSceneNode & emitter), (override));
    MOCK_METHOD(std::shared_ptr<GrassSceneNode>, newGrass, (GrassProperties properties, graphics::ModelNode &aabbNode), (override));
    MOCK_METHOD(std::shared_ptr<GrassClusterSceneNode>, newGrassCluster, (GrassSceneNode & grass), (override));

    MOCK_METHOD(std::shared_ptr<graphics::Camera>, camera, (), (const override));
    MOCK_METHOD(const glm::vec3 &, ambientLightColor, (), (const override));

    MOCK_METHOD(bool, isFogEnabled, (), (const override));

    MOCK_METHOD(float, fogNear, (), (const override));
    MOCK_METHOD(float, fogFar, (), (const override));
    MOCK_METHOD(const glm::vec3 &, fogColor, (), (const override));

    MOCK_METHOD(bool, hasShadowLight, (), (const override));
    MOCK_METHOD(bool, isShadowLightDirectional, (), (const override));

    MOCK_METHOD(glm::vec3, shadowLightPosition, (), (const override));
    MOCK_METHOD(float, shadowStrength, (), (const override));
    MOCK_METHOD(float, shadowRadius, (), (const override));
};

class MockSceneGraphs : public ISceneGraphs, boost::noncopyable {
public:
    MOCK_METHOD(void, reserve, (std::string name), (override));
    MOCK_METHOD(ISceneGraph &, get, (const std::string &name), (override));
    MOCK_METHOD(std::set<std::string>, sceneNames, (), (const override));
};

class MockRenderPipeline : public IRenderPipeline, boost::noncopyable {
public:
    MOCK_METHOD(void, init, (), (override));

    MOCK_METHOD(void, reset, (), (override));
    MOCK_METHOD(void, inRenderPass, (RenderPassName, std::function<void(IRenderPass &)>), (override));

    MOCK_METHOD(graphics::Texture &, render, (), (override));
};

class MockRenderPipelineFactory : public IRenderPipelineFactory, boost::noncopyable {
public:
    MOCK_METHOD(std::unique_ptr<IRenderPipeline>, create, (RendererType, glm::ivec2), (override));
};

class TestSceneModule : boost::noncopyable {
public:
    void init() {
        _graphs = std::make_unique<MockSceneGraphs>();
        _renderPipelineFactory = std::make_unique<MockRenderPipelineFactory>();

        _services = std::make_unique<SceneServices>(*_graphs, *_renderPipelineFactory);
    }

    MockSceneGraphs &graphs() {
        return *_graphs;
    }

    MockRenderPipelineFactory &renderPipelineFactory() {
        return *_renderPipelineFactory;
    }

    SceneServices &services() {
        return *_services;
    }

private:
    std::unique_ptr<MockSceneGraphs> _graphs;
    std::unique_ptr<MockRenderPipelineFactory> _renderPipelineFactory;

    std::unique_ptr<SceneServices> _services;
};

} // namespace scene

} // namespace reone
