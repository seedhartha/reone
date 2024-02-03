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

#include <gtest/gtest.h>

#include "reone/game/types.h"
#include "reone/graphics/animation.h"
#include "reone/graphics/mesh.h"
#include "reone/graphics/options.h"
#include "reone/scene/graphs.h"
#include "reone/scene/node/model.h"
#include "reone/scene/node/modelnode.h"

#include "../fixtures/audio.h"
#include "../fixtures/graphics.h"
#include "../fixtures/resource.h"
#include "../fixtures/scene.h"

using namespace reone;
using namespace reone::audio;
using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

using testing::_;
using testing::ReturnRef;

TEST(ModelSceneNode, should_build_from_model) {
    // given
    auto graphicsOpt = GraphicsOptions();
    auto pipelineFactory = MockRenderPipelineFactory();

    auto graphicsModule = TestGraphicsModule();
    graphicsModule.init();

    auto audioModule = TestAudioModule();
    audioModule.init();

    auto resourceModule = TestResourceModule();
    resourceModule.init();

    auto scene = std::make_unique<SceneGraph>("test", pipelineFactory, graphicsOpt, graphicsModule.services(), audioModule.services(), resourceModule.services());

    auto rootNode = std::make_shared<ModelNode>(0, "root_node", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), true, nullptr);

    auto mesh = std::make_shared<ModelNode::TriangleMesh>();
    auto meshNode = std::make_shared<ModelNode>(1, "mesh_node", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), true, rootNode.get());
    meshNode->setMesh(mesh);
    rootNode->addChild(meshNode);

    auto light = std::make_shared<ModelNode::Light>();
    auto lightNode = std::make_shared<ModelNode>(2, "light_node", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), true, rootNode.get());
    lightNode->setLight(light);
    rootNode->addChild(lightNode);

    auto emitter = std::make_shared<ModelNode::Emitter>();
    auto emitterNode = std::make_shared<ModelNode>(3, "emitter_node", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), true, rootNode.get());
    emitterNode->setEmitter(emitter);
    rootNode->addChild(emitterNode);

    auto model = Model("some_model", 0, rootNode, std::vector<std::shared_ptr<Animation>>(), "", 1.0f);
    auto modelSceneNode = std::make_shared<ModelSceneNode>(
        model,
        ModelUsage::Creature,
        *scene,
        graphicsModule.services(),
        audioModule.services(),
        resourceModule.services());

    // when
    modelSceneNode->init();

    // then
    EXPECT_EQ(1ll, modelSceneNode->children().size());

    auto rootNodeSceneNode = modelSceneNode->getNodeByName("root_node");
    EXPECT_TRUE(static_cast<bool>(rootNodeSceneNode));
    EXPECT_EQ(static_cast<int>(SceneNodeType::Dummy), static_cast<int>(rootNodeSceneNode->type()));

    EXPECT_EQ(3ll, rootNodeSceneNode->children().size());

    auto meshSceneNode = modelSceneNode->getNodeByName("mesh_node");
    EXPECT_TRUE(static_cast<bool>(meshSceneNode));
    EXPECT_EQ(static_cast<int>(SceneNodeType::Mesh), static_cast<int>(meshSceneNode->type()));

    auto lightSceneNode = modelSceneNode->getNodeByName("light_node");
    EXPECT_TRUE(static_cast<bool>(lightSceneNode));
    EXPECT_EQ(static_cast<int>(SceneNodeType::Light), static_cast<int>(lightSceneNode->type()));

    auto emitterSceneNode = modelSceneNode->getNodeByName("emitter_node");
    EXPECT_TRUE(static_cast<bool>(emitterSceneNode));
    EXPECT_EQ(static_cast<int>(SceneNodeType::Emitter), static_cast<int>(emitterSceneNode->type()));
}

TEST(ModelSceneNode, should_play_single_fire_forget_animation) {
    // given
    auto graphicsOpt = GraphicsOptions();
    auto pipelineFactory = MockRenderPipelineFactory();

    auto graphicsModule = TestGraphicsModule();
    graphicsModule.init();

    auto audioModule = TestAudioModule();
    audioModule.init();

    auto resourceModule = TestResourceModule();
    resourceModule.init();

    auto scene = std::make_unique<SceneGraph>("test", pipelineFactory, graphicsOpt, graphicsModule.services(), audioModule.services(), resourceModule.services());

    auto rootNode = std::make_shared<ModelNode>(0, "root_node", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), true, nullptr);

    auto animRootNode = std::make_shared<ModelNode>(0, "root_node", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), false, nullptr);
    animRootNode->vectorTracks()[ControllerTypes::position].add(0.0f, glm::vec3(0.0f));
    animRootNode->vectorTracks()[ControllerTypes::position].add(1.0f, glm::vec3(1.0f, 2.0f, 3.0f));

    auto animations = std::vector<std::shared_ptr<Animation>> {
        std::make_shared<Animation>("some_animation", 1.0f, 0.5f, "root_node", animRootNode, std::vector<Animation::Event>())};

    auto model = Model("some_model", 0, rootNode, animations, "", 1.0f);

    auto modelSceneNode = std::make_shared<ModelSceneNode>(
        model,
        ModelUsage::Creature,
        *scene,
        graphicsModule.services(),
        audioModule.services(),
        resourceModule.services());

    // when
    modelSceneNode->init();
    modelSceneNode->playAnimation("some_animation", nullptr, AnimationProperties::fromFlags(AnimationFlags::fireForget));
    modelSceneNode->update(1.25f);

    // then
    auto &channels = modelSceneNode->animationChannels();
    EXPECT_EQ(1ll, channels.size());
    EXPECT_EQ(1.0f, channels[0].time);
    EXPECT_TRUE(channels[0].finished);
    auto rootSceneNode = modelSceneNode->getNodeByName("root_node");
    EXPECT_TRUE(static_cast<bool>(rootSceneNode));
    auto &rootPosition = rootSceneNode->localTransform()[3];
    EXPECT_NEAR(1.0f, rootPosition.x, 1e-5);
    EXPECT_NEAR(2.0f, rootPosition.y, 1e-5);
    EXPECT_NEAR(3.0f, rootPosition.z, 1e-5);
}

TEST(ModelSceneNode, should_play_single_looping_animation) {
    // given
    auto graphicsOpt = GraphicsOptions();
    auto pipelineFactory = MockRenderPipelineFactory();

    auto graphicsModule = TestGraphicsModule();
    graphicsModule.init();

    auto audioModule = TestAudioModule();
    audioModule.init();

    auto resourceModule = TestResourceModule();
    resourceModule.init();

    auto scene = std::make_unique<SceneGraph>("test", pipelineFactory, graphicsOpt, graphicsModule.services(), audioModule.services(), resourceModule.services());

    auto rootNode = std::make_shared<ModelNode>(0, "root_node", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), true, nullptr);

    auto animRootNode = std::make_shared<ModelNode>(0, "root_node", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), false, nullptr);
    animRootNode->vectorTracks()[ControllerTypes::position].add(0.0f, glm::vec3(0.0f));
    animRootNode->vectorTracks()[ControllerTypes::position].add(1.0f, glm::vec3(1.0f, 2.0f, 3.0f));

    auto animations = std::vector<std::shared_ptr<Animation>> {
        std::make_shared<Animation>("some_animation", 1.0f, 0.5f, "root_node", animRootNode, std::vector<Animation::Event>())};

    auto model = Model("some_model", 0, rootNode, animations, "", 1.0f);
    auto modelSceneNode = std::make_shared<ModelSceneNode>(
        model,
        ModelUsage::Creature,
        *scene,
        graphicsModule.services(),
        audioModule.services(),
        resourceModule.services());

    // when
    modelSceneNode->init();
    modelSceneNode->playAnimation("some_animation", nullptr, AnimationProperties::fromFlags(AnimationFlags::loop));
    modelSceneNode->update(1.25f);

    // then
    auto &channels = modelSceneNode->animationChannels();
    EXPECT_EQ(1ll, channels.size());
    EXPECT_NEAR(0.0f, channels[0].time, 1e-5);
    EXPECT_TRUE(!channels[0].finished);
    auto rootSceneNode = modelSceneNode->getNodeByName("root_node");
    EXPECT_TRUE(static_cast<bool>(rootSceneNode));
    auto &rootPosition = rootSceneNode->localTransform()[3];
    EXPECT_NEAR(1.0f, rootPosition.x, 1e-5);
    EXPECT_NEAR(2.0f, rootPosition.y, 1e-5);
    EXPECT_NEAR(3.0f, rootPosition.z, 1e-5);
}

TEST(ModelSceneNode, should_play_two_overlayed_animations) {
    // given
    auto graphicsOpt = GraphicsOptions();
    auto pipelineFactory = MockRenderPipelineFactory();

    auto graphicsModule = TestGraphicsModule();
    graphicsModule.init();

    auto audioModule = TestAudioModule();
    audioModule.init();

    auto resourceModule = TestResourceModule();
    resourceModule.init();

    auto scene = std::make_unique<SceneGraph>("test", pipelineFactory, graphicsOpt, graphicsModule.services(), audioModule.services(), resourceModule.services());

    auto rootNode = std::make_shared<ModelNode>(0, "root_node", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), true, nullptr);
    auto dummyNode = std::make_shared<ModelNode>(1, "dummy_node", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), true, rootNode.get());
    rootNode->addChild(dummyNode);

    auto anim1RootNode = std::make_shared<ModelNode>(0, "root_node", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), false, nullptr);
    anim1RootNode->vectorTracks()[ControllerTypes::position].add(0.0f, glm::vec3(0.0f));
    anim1RootNode->vectorTracks()[ControllerTypes::position].add(1.0f, glm::vec3(1.0f, 2.0f, 3.0f));

    auto anim2RootNode = std::make_shared<ModelNode>(0, "root_node", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), false, nullptr);
    auto anim2DummyNode = std::make_shared<ModelNode>(1, "dummy_node", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), false, anim2RootNode.get());
    anim2DummyNode->vectorTracks()[ControllerTypes::position].add(0.0f, glm::vec3(0.0f));
    anim2DummyNode->vectorTracks()[ControllerTypes::position].add(2.0f, glm::vec3(4.0f, 5.0f, 6.0f));
    anim2RootNode->addChild(anim2DummyNode);

    auto animations = std::vector<std::shared_ptr<Animation>> {
        std::make_shared<Animation>("animation1", 1.0f, 0.5f, "root_node", anim1RootNode, std::vector<Animation::Event>()),
        std::make_shared<Animation>("animation2", 2.0f, 0.5f, "root_node", anim2RootNode, std::vector<Animation::Event>())};

    auto model = Model("some_model", 0, rootNode, animations, "", 1.0f);
    auto modelSceneNode = std::make_shared<ModelSceneNode>(
        model,
        ModelUsage::Creature,
        *scene,
        graphicsModule.services(),
        audioModule.services(),
        resourceModule.services());

    // when
    modelSceneNode->init();
    modelSceneNode->playAnimation("animation1", nullptr, AnimationProperties::fromFlags(AnimationFlags::loopOverlay));
    modelSceneNode->playAnimation("animation2", nullptr, AnimationProperties::fromFlags(AnimationFlags::loopOverlay));
    modelSceneNode->update(1.25f);

    // then
    auto &channels = modelSceneNode->animationChannels();
    EXPECT_EQ(2ll, channels.size());
    EXPECT_NEAR(1.25f, channels[0].time, 1e-5);
    EXPECT_NEAR(0.0f, channels[1].time, 1e-5);
    EXPECT_TRUE(!channels[0].finished);
    EXPECT_TRUE(!channels[1].finished);
    auto rootSceneNode = modelSceneNode->getNodeByName("root_node");
    EXPECT_TRUE(static_cast<bool>(rootSceneNode));
    auto &rootPosition = rootSceneNode->localTransform()[3];
    EXPECT_NEAR(1.0f, rootPosition.x, 1e-5);
    EXPECT_NEAR(2.0f, rootPosition.y, 1e-5);
    EXPECT_NEAR(3.0f, rootPosition.z, 1e-5);
    auto dummySceneNode = modelSceneNode->getNodeByName("dummy_node");
    EXPECT_TRUE(static_cast<bool>(dummySceneNode));
    auto &dummyPosition = dummySceneNode->localTransform()[3];
    EXPECT_NEAR(2.5f, dummyPosition.x, 1e-5);
    EXPECT_NEAR(3.125f, dummyPosition.y, 1e-5);
    EXPECT_NEAR(3.75f, dummyPosition.z, 1e-5);
}

TEST(ModelSceneNode, hould_transition_between_two_animations) {
    // given
    auto graphicsOpt = GraphicsOptions();
    auto pipelineFactory = MockRenderPipelineFactory();

    auto graphicsModule = TestGraphicsModule();
    graphicsModule.init();

    auto audioModule = TestAudioModule();
    audioModule.init();

    auto resourceModule = TestResourceModule();
    resourceModule.init();

    auto scene = std::make_unique<SceneGraph>("test", pipelineFactory, graphicsOpt, graphicsModule.services(), audioModule.services(), resourceModule.services());

    auto rootNode = std::make_shared<ModelNode>(0, "root_node", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), true, nullptr);

    auto anim1RootNode = std::make_shared<ModelNode>(0, "root_node", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), false, nullptr);
    anim1RootNode->vectorTracks()[ControllerTypes::position].add(0.0f, glm::vec3(0.0f));
    anim1RootNode->vectorTracks()[ControllerTypes::position].add(1.0f, glm::vec3(1.0f, 2.0f, 3.0f));

    auto anim2RootNode = std::make_shared<ModelNode>(0, "root_node", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), false, nullptr);
    anim2RootNode->vectorTracks()[ControllerTypes::position].add(0.0f, glm::vec3(0.0f));
    anim2RootNode->vectorTracks()[ControllerTypes::position].add(2.0f, glm::vec3(4.0f, 5.0f, 6.0f));

    auto animations = std::vector<std::shared_ptr<Animation>> {
        std::make_shared<Animation>("animation1", 1.0f, 0.5f, "root_node", anim1RootNode, std::vector<Animation::Event>()),
        std::make_shared<Animation>("animation2", 2.0f, 0.5f, "root_node", anim2RootNode, std::vector<Animation::Event>())};

    auto model = Model("some_model", 0, rootNode, animations, "", 1.0f);
    auto modelSceneNode = std::make_shared<ModelSceneNode>(
        model,
        ModelUsage::Creature,
        *scene,
        graphicsModule.services(),
        audioModule.services(),
        resourceModule.services());

    // when
    modelSceneNode->init();
    modelSceneNode->playAnimation("animation1", nullptr, AnimationProperties::fromFlags(AnimationFlags::loopBlend));
    modelSceneNode->playAnimation("animation2", nullptr, AnimationProperties::fromFlags(AnimationFlags::loopBlend));
    modelSceneNode->update(1.25f);

    // then
    auto &channels = modelSceneNode->animationChannels();
    EXPECT_EQ(2ll, channels.size());
    EXPECT_NEAR(1.5f, channels[0].time, 1e-5);
    EXPECT_NEAR(0.0f, channels[1].time, 1e-5);
    EXPECT_TRUE(!channels[0].finished);
    EXPECT_TRUE(!channels[1].finished);
    auto rootSceneNode = modelSceneNode->getNodeByName("root_node");
    EXPECT_TRUE(static_cast<bool>(rootSceneNode));
    auto &rootPosition = rootSceneNode->localTransform()[3];
    EXPECT_NEAR(3.0f, rootPosition.x, 1e-5);
    EXPECT_NEAR(3.75f, rootPosition.y, 1e-5);
    EXPECT_NEAR(4.5f, rootPosition.z, 1e-5);
}
