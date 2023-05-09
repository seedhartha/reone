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

#include <boost/test/unit_test.hpp>

#include "reone/graphics/animation.h"
#include "reone/graphics/mesh.h"
#include "reone/scene/node/model.h"
#include "reone/scene/node/modelnode.h"

using namespace std;

using namespace reone;
using namespace reone::graphics;
using namespace reone::scene;

BOOST_AUTO_TEST_SUITE(model_scene_node)

// TODO: un-comment
/*
BOOST_AUTO_TEST_CASE(should_build_from_model) {
    // given
    auto test = FunctionalTest();

    auto rootNode = make_shared<ModelNode>(0, "root_node", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), true, nullptr);

    auto mesh = make_shared<ModelNode::TriangleMesh>();
    auto meshNode = make_shared<ModelNode>(1, "mesh_node", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), true, rootNode.get());
    meshNode->setMesh(mesh);
    rootNode->addChild(meshNode);

    auto light = make_shared<ModelNode::Light>();
    auto lightNode = make_shared<ModelNode>(2, "light_node", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), true, rootNode.get());
    lightNode->setLight(light);
    rootNode->addChild(lightNode);

    auto emitter = make_shared<ModelNode::Emitter>();
    auto emitterNode = make_shared<ModelNode>(3, "emitter_node", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), true, rootNode.get());
    emitterNode->setEmitter(emitter);
    rootNode->addChild(emitterNode);

    auto model = Model("some_model", 0, rootNode, vector<shared_ptr<Animation>>(), nullptr, 1.0f);

    auto &scene = test.sceneMockByName(kSceneMain);
    auto modelSceneNode = scene.newModel(model, ModelUsage::Creature);

    // when
    modelSceneNode->init();

    // then
    BOOST_TEST(1ll == modelSceneNode->children().size());

    auto rootNodeSceneNode = modelSceneNode->getNodeByName("root_node");
    BOOST_REQUIRE(static_cast<bool>(rootNodeSceneNode));
    BOOST_TEST(static_cast<int>(SceneNodeType::Dummy) == static_cast<int>(rootNodeSceneNode->type()));

    BOOST_TEST(3ll == rootNodeSceneNode->children().size());

    auto meshSceneNode = modelSceneNode->getNodeByName("mesh_node");
    BOOST_REQUIRE(static_cast<bool>(meshSceneNode));
    BOOST_TEST(static_cast<int>(SceneNodeType::Mesh) == static_cast<int>(meshSceneNode->type()));

    auto lightSceneNode = modelSceneNode->getNodeByName("light_node");
    BOOST_REQUIRE(static_cast<bool>(lightSceneNode));
    BOOST_TEST(static_cast<int>(SceneNodeType::Light) == static_cast<int>(lightSceneNode->type()));

    auto emitterSceneNode = modelSceneNode->getNodeByName("emitter_node");
    BOOST_REQUIRE(static_cast<bool>(emitterSceneNode));
    BOOST_TEST(static_cast<int>(SceneNodeType::Emitter) == static_cast<int>(emitterSceneNode->type()));
}

BOOST_AUTO_TEST_CASE(should_play_single_fire_forget_animation) {
    // given
    auto test = FunctionalTest();

    auto rootNode = make_shared<ModelNode>(0, "root_node", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), true, nullptr);

    auto animRootNode = make_shared<ModelNode>(0, "root_node", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), false, nullptr);
    animRootNode->position().addFrame(0.0f, glm::vec3(0.0f));
    animRootNode->position().addFrame(1.0f, glm::vec3(1.0f, 2.0f, 3.0f));

    auto animations = vector<shared_ptr<Animation>> {
        make_shared<Animation>("some_animation", 1.0f, 0.5f, "root_node", animRootNode, vector<Animation::Event>())};

    auto model = Model("some_model", 0, rootNode, animations, nullptr, 1.0f);

    auto &scene = test.sceneMockByName(kSceneMain);
    auto modelSceneNode = scene.newModel(model, ModelUsage::Creature);

    // when
    modelSceneNode->init();
    modelSceneNode->playAnimation("some_animation", AnimationProperties::fromFlags(AnimationFlags::fireForget));
    modelSceneNode->update(1.25f);

    // then
    auto &channels = modelSceneNode->animationChannels();
    BOOST_TEST(1ll == channels.size());
    BOOST_TEST(1.0f == channels[0].time);
    BOOST_TEST(channels[0].finished);
    auto rootSceneNode = modelSceneNode->getNodeByName("root_node");
    BOOST_REQUIRE(static_cast<bool>(rootSceneNode));
    auto &rootPosition = rootSceneNode->localTransform()[3];
    BOOST_CHECK_CLOSE(1.0f, rootPosition.x, 1e-5);
    BOOST_CHECK_CLOSE(2.0f, rootPosition.y, 1e-5);
    BOOST_CHECK_CLOSE(3.0f, rootPosition.z, 1e-5);
}

BOOST_AUTO_TEST_CASE(should_play_single_looping_animation) {
    // given
    auto test = FunctionalTest();

    auto rootNode = make_shared<ModelNode>(0, "root_node", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), true, nullptr);

    auto animRootNode = make_shared<ModelNode>(0, "root_node", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), false, nullptr);
    animRootNode->position().addFrame(0.0f, glm::vec3(0.0f));
    animRootNode->position().addFrame(1.0f, glm::vec3(1.0f, 2.0f, 3.0f));

    auto animations = vector<shared_ptr<Animation>> {
        make_shared<Animation>("some_animation", 1.0f, 0.5f, "root_node", animRootNode, vector<Animation::Event>())};

    auto model = Model("some_model", 0, rootNode, animations, nullptr, 1.0f);

    auto &scene = test.sceneMockByName(kSceneMain);
    auto modelSceneNode = scene.newModel(model, ModelUsage::Creature);

    // when
    modelSceneNode->init();
    modelSceneNode->playAnimation("some_animation", AnimationProperties::fromFlags(AnimationFlags::loop));
    modelSceneNode->update(1.25f);

    // then
    auto &channels = modelSceneNode->animationChannels();
    BOOST_TEST(1ll == channels.size());
    BOOST_CHECK_CLOSE(0.0f, channels[0].time, 1e-5);
    BOOST_TEST(!channels[0].finished);
    auto rootSceneNode = modelSceneNode->getNodeByName("root_node");
    BOOST_REQUIRE(static_cast<bool>(rootSceneNode));
    auto &rootPosition = rootSceneNode->localTransform()[3];
    BOOST_CHECK_CLOSE(1.0f, rootPosition.x, 1e-5);
    BOOST_CHECK_CLOSE(2.0f, rootPosition.y, 1e-5);
    BOOST_CHECK_CLOSE(3.0f, rootPosition.z, 1e-5);
}

BOOST_AUTO_TEST_CASE(should_play_two_overlayed_animations) {
    // given
    auto test = FunctionalTest();

    auto rootNode = make_shared<ModelNode>(0, "root_node", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), true, nullptr);
    auto dummyNode = make_shared<ModelNode>(1, "dummy_node", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), true, rootNode.get());
    rootNode->addChild(dummyNode);

    auto anim1RootNode = make_shared<ModelNode>(0, "root_node", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), false, nullptr);
    anim1RootNode->position().addFrame(0.0f, glm::vec3(0.0f));
    anim1RootNode->position().addFrame(1.0f, glm::vec3(1.0f, 2.0f, 3.0f));

    auto anim2RootNode = make_shared<ModelNode>(0, "root_node", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), false, nullptr);
    auto anim2DummyNode = make_shared<ModelNode>(1, "dummy_node", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), false, anim2RootNode.get());
    anim2DummyNode->position().addFrame(0.0f, glm::vec3(0.0f));
    anim2DummyNode->position().addFrame(2.0f, glm::vec3(4.0f, 5.0f, 6.0f));
    anim2RootNode->addChild(anim2DummyNode);

    auto animations = vector<shared_ptr<Animation>> {
        make_shared<Animation>("animation1", 1.0f, 0.5f, "root_node", anim1RootNode, vector<Animation::Event>()),
        make_shared<Animation>("animation2", 2.0f, 0.5f, "root_node", anim2RootNode, vector<Animation::Event>())};

    auto model = Model("some_model", 0, rootNode, animations, nullptr, 1.0f);

    auto &scene = test.sceneMockByName(kSceneMain);
    auto modelSceneNode = scene.newModel(model, ModelUsage::Creature);

    // when
    modelSceneNode->init();
    modelSceneNode->playAnimation("animation1", AnimationProperties::fromFlags(AnimationFlags::loopOverlay));
    modelSceneNode->playAnimation("animation2", AnimationProperties::fromFlags(AnimationFlags::loopOverlay));
    modelSceneNode->update(1.25f);

    // then
    auto &channels = modelSceneNode->animationChannels();
    BOOST_TEST(2ll == channels.size());
    BOOST_CHECK_CLOSE(1.25f, channels[0].time, 1e-5);
    BOOST_CHECK_CLOSE(0.0f, channels[1].time, 1e-5);
    BOOST_TEST(!channels[0].finished);
    BOOST_TEST(!channels[1].finished);
    auto rootSceneNode = modelSceneNode->getNodeByName("root_node");
    BOOST_REQUIRE(static_cast<bool>(rootSceneNode));
    auto &rootPosition = rootSceneNode->localTransform()[3];
    BOOST_CHECK_CLOSE(1.0f, rootPosition.x, 1e-5);
    BOOST_CHECK_CLOSE(2.0f, rootPosition.y, 1e-5);
    BOOST_CHECK_CLOSE(3.0f, rootPosition.z, 1e-5);
    auto dummySceneNode = modelSceneNode->getNodeByName("dummy_node");
    BOOST_REQUIRE(static_cast<bool>(dummySceneNode));
    auto &dummyPosition = dummySceneNode->localTransform()[3];
    BOOST_CHECK_CLOSE(2.5f, dummyPosition.x, 1e-5);
    BOOST_CHECK_CLOSE(3.125f, dummyPosition.y, 1e-5);
    BOOST_CHECK_CLOSE(3.75f, dummyPosition.z, 1e-5);
}

BOOST_AUTO_TEST_CASE(should_transition_between_two_animations) {
    // given
    auto test = FunctionalTest();

    auto rootNode = make_shared<ModelNode>(0, "root_node", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), true, nullptr);

    auto anim1RootNode = make_shared<ModelNode>(0, "root_node", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), false, nullptr);
    anim1RootNode->position().addFrame(0.0f, glm::vec3(0.0f));
    anim1RootNode->position().addFrame(1.0f, glm::vec3(1.0f, 2.0f, 3.0f));

    auto anim2RootNode = make_shared<ModelNode>(0, "root_node", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), false, nullptr);
    anim2RootNode->position().addFrame(0.0f, glm::vec3(0.0f));
    anim2RootNode->position().addFrame(2.0f, glm::vec3(4.0f, 5.0f, 6.0f));

    auto animations = vector<shared_ptr<Animation>> {
        make_shared<Animation>("animation1", 1.0f, 0.5f, "root_node", anim1RootNode, vector<Animation::Event>()),
        make_shared<Animation>("animation2", 2.0f, 0.5f, "root_node", anim2RootNode, vector<Animation::Event>())};

    auto model = Model("some_model", 0, rootNode, animations, nullptr, 1.0f);

    auto &scene = test.sceneMockByName(kSceneMain);
    auto modelSceneNode = scene.newModel(model, ModelUsage::Creature);

    // when
    modelSceneNode->init();
    modelSceneNode->playAnimation("animation1", AnimationProperties::fromFlags(AnimationFlags::loopBlend));
    modelSceneNode->playAnimation("animation2", AnimationProperties::fromFlags(AnimationFlags::loopBlend));
    modelSceneNode->update(1.25f);

    // then
    auto &channels = modelSceneNode->animationChannels();
    BOOST_TEST(2ll == channels.size());
    BOOST_CHECK_CLOSE(1.5f, channels[0].time, 1e-5);
    BOOST_CHECK_CLOSE(0.0f, channels[1].time, 1e-5);
    BOOST_TEST(!channels[0].finished);
    BOOST_TEST(!channels[1].finished);
    auto rootSceneNode = modelSceneNode->getNodeByName("root_node");
    BOOST_REQUIRE(static_cast<bool>(rootSceneNode));
    auto &rootPosition = rootSceneNode->localTransform()[3];
    BOOST_CHECK_CLOSE(3.0f, rootPosition.x, 1e-5);
    BOOST_CHECK_CLOSE(3.75f, rootPosition.y, 1e-5);
    BOOST_CHECK_CLOSE(4.5f, rootPosition.z, 1e-5);
}
*/

BOOST_AUTO_TEST_SUITE_END()
