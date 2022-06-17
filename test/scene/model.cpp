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

#include "../../src/graphics/animation.h"
#include "../../src/graphics/mesh.h"
#include "../../src/scene/node/model.h"
#include "../../src/scene/node/modelnode.h"

#include "../fixtures/functional.h"

using namespace std;

using namespace reone;
using namespace reone::game;
using namespace reone::graphics;
using namespace reone::scene;

BOOST_AUTO_TEST_SUITE(model_scene_node)

BOOST_AUTO_TEST_CASE(should_build_from_model) {
    // given
    auto test = FunctionalTest();

    auto rootNode = make_shared<ModelNode>(0, "root_node", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), false, nullptr);

    auto mesh = make_shared<ModelNode::TriangleMesh>();
    auto meshNode = make_shared<ModelNode>(1, "mesh_node", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), false, rootNode.get());
    meshNode->setMesh(mesh);
    rootNode->addChild(meshNode);

    auto light = make_shared<ModelNode::Light>();
    auto lightNode = make_shared<ModelNode>(2, "light_node", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), false, rootNode.get());
    lightNode->setLight(light);
    rootNode->addChild(lightNode);

    auto emitter = make_shared<ModelNode::Emitter>();
    auto emitterNode = make_shared<ModelNode>(3, "emitter_node", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), false, rootNode.get());
    emitterNode->setEmitter(emitter);
    rootNode->addChild(emitterNode);

    auto model = Model("some_model", 0, rootNode, vector<shared_ptr<Animation>>(), nullptr, 1.0f);

    auto &scene = test.sceneMockByName(kSceneMain);
    auto modelSceneNode = scene.newModel(model, ModelUsage::Creature);

    // when
    modelSceneNode->init();

    // then
    BOOST_CHECK_EQUAL(1ll, modelSceneNode->children().size());

    auto rootNodeSceneNode = modelSceneNode->getNodeByName("root_node");
    BOOST_REQUIRE(static_cast<bool>(rootNodeSceneNode));
    BOOST_CHECK_EQUAL(static_cast<int>(SceneNodeType::Dummy), static_cast<int>(rootNodeSceneNode->type()));

    BOOST_CHECK_EQUAL(3ll, rootNodeSceneNode->children().size());

    auto meshSceneNode = modelSceneNode->getNodeByName("mesh_node");
    BOOST_REQUIRE(static_cast<bool>(meshSceneNode));
    BOOST_CHECK_EQUAL(static_cast<int>(SceneNodeType::Mesh), static_cast<int>(meshSceneNode->type()));

    auto lightSceneNode = modelSceneNode->getNodeByName("light_node");
    BOOST_REQUIRE(static_cast<bool>(lightSceneNode));
    BOOST_CHECK_EQUAL(static_cast<int>(SceneNodeType::Light), static_cast<int>(lightSceneNode->type()));

    auto emitterSceneNode = modelSceneNode->getNodeByName("emitter_node");
    BOOST_REQUIRE(static_cast<bool>(emitterSceneNode));
    BOOST_CHECK_EQUAL(static_cast<int>(SceneNodeType::Emitter), static_cast<int>(emitterSceneNode->type()));
}

BOOST_AUTO_TEST_CASE(should_update_animations) {
    // given
    auto test = FunctionalTest();

    auto rootNode = make_shared<ModelNode>(0, "some_model", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), false, nullptr);

    auto animRootNode = make_shared<ModelNode>(0, "some_model", glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), true, nullptr);
    auto animations = vector<shared_ptr<Animation>> {
        make_shared<Animation>("some_animation", 1.0f, 0.5f, "some_model", animRootNode, vector<Animation::Event>())};

    auto model = Model("some_model", 0, rootNode, animations, nullptr, 1.0f);

    auto &scene = test.sceneMockByName(kSceneMain);
    auto sceneNode = scene.newModel(model, ModelUsage::Creature);

    // when
    sceneNode->init();

    // then
}

BOOST_AUTO_TEST_SUITE_END()
