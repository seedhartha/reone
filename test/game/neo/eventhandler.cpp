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

#include "../../fixtures/audio.h"
#include "../../fixtures/graphics.h"
#include "../../fixtures/resource.h"
#include "../../fixtures/scene.h"

#include "reone/game/neo/event.h"
#include "reone/game/neo/eventhandler.h"
#include "reone/game/neo/object/area.h"
#include "reone/game/neo/object/creature.h"
#include "reone/game/neo/object/door.h"
#include "reone/game/neo/object/module.h"
#include "reone/game/neo/object/placeable.h"
#include "reone/graphics/animation.h"
#include "reone/graphics/model.h"
#include "reone/graphics/options.h"
#include "reone/graphics/walkmesh.h"

using namespace reone::audio;
using namespace reone::game::neo;
using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

using testing::_;
using testing::Return;
using testing::ReturnRef;
using testing::Test;

class EventHandlerFixture : public Test {
protected:
    void SetUp() override {
        _graphicsModule.init();
        _audioModule.init();
        _resourceModule.init();
        _sceneModule.init();
        _subject = std::make_unique<EventHandler>(
            _sceneLock,
            _graphicsOpt,
            _resourceModule.services(),
            _sceneModule.services());

        AreaRoom room;
        room.model = "m01aa_01a";
        room.position = glm::vec3 {1.0f, 2.0f, 3.0f};
        _area.add(std::move(room));

        _module.setArea(_area);
        _subject->setModule(_module);
    }

    std::mutex _sceneMutex;
    std::unique_lock<std::mutex> _sceneLock {_sceneMutex, std::defer_lock};
    GraphicsOptions _graphicsOpt;
    TestGraphicsModule _graphicsModule;
    TestAudioModule _audioModule;
    TestResourceModule _resourceModule;
    TestSceneModule _sceneModule;
    MockSceneGraph _sceneGraph;

    Module _module {0, ""};
    Area _area {1, ""};

    std::unique_ptr<EventHandler> _subject;
};

TEST_F(EventHandlerFixture, should_handle_area_loaded_event) {
    // given
    Event event;
    event.type = EventType::ObjectStateChanged;
    event.object.objectId = 1;
    event.object.state = ObjectState::Loaded;
    auto model = std::make_shared<Model>(
        "",
        0,
        std::shared_ptr<ModelNode>(),
        std::vector<std::shared_ptr<Animation>>(),
        "",
        1.0f);
    auto walkmesh = std::make_shared<Walkmesh>();
    auto modelSceneNode = std::make_shared<ModelSceneNode>(
        *model,
        ModelUsage::Room,
        _sceneGraph,
        _graphicsModule.services(),
        _audioModule.services(),
        _resourceModule.services());
    auto walkmeshSceneNode = std::make_shared<WalkmeshSceneNode>(
        *walkmesh,
        _sceneGraph,
        _graphicsModule.services(),
        _audioModule.services(),
        _resourceModule.services());

    // expect
    EXPECT_CALL(_sceneModule.graphs(), get(_)).WillOnce(ReturnRef(_sceneGraph));
    EXPECT_CALL(_resourceModule.models(), get(_)).WillOnce(Return(model));
    EXPECT_CALL(_sceneGraph, newModel(_, ModelUsage::Room)).WillOnce(Return(modelSceneNode));
    EXPECT_CALL(_sceneGraph, addRoot(modelSceneNode));
    EXPECT_CALL(_resourceModule.walkmeshes(), get(_, ResType::Wok)).WillOnce(Return(walkmesh));
    EXPECT_CALL(_sceneGraph, newWalkmesh(_)).WillOnce(Return(walkmeshSceneNode));
    EXPECT_CALL(_sceneGraph, addRoot(walkmeshSceneNode));
    _subject->handle(event);
    EXPECT_EQ(modelSceneNode->localTransform(), glm::translate(_area.rooms().at(0).position));
    EXPECT_EQ(walkmeshSceneNode->localTransform(), glm::identity<glm::mat4>());
}

TEST_F(EventHandlerFixture, should_handle_creature_loaded_event) {
    // given
    Creature creature {2, ""};
    creature.setPosition({1.0f, 2.0f, 3.0f});
    creature.setFacing(glm::radians(90.0f));
    _area.add(creature);
    Event event;
    event.type = EventType::ObjectStateChanged;
    event.object.objectId = 2;
    event.object.state = ObjectState::Loaded;
    auto model = std::make_shared<Model>(
        "",
        0,
        std::shared_ptr<ModelNode>(),
        std::vector<std::shared_ptr<Animation>>(),
        "",
        1.0f);
    auto modelSceneNode = std::make_shared<ModelSceneNode>(
        *model,
        ModelUsage::Creature,
        _sceneGraph,
        _graphicsModule.services(),
        _audioModule.services(),
        _resourceModule.services());

    // expect
    EXPECT_CALL(_sceneModule.graphs(), get(_)).WillOnce(ReturnRef(_sceneGraph));
    EXPECT_CALL(_resourceModule.models(), get(_)).WillOnce(Return(model));
    EXPECT_CALL(_sceneGraph, newModel(_, ModelUsage::Creature)).WillOnce(Return(modelSceneNode));
    EXPECT_CALL(_sceneGraph, addRoot(modelSceneNode));
    _subject->handle(event);

    auto transform = glm::translate(creature.position());
    transform *= glm::eulerAngleZ(creature.facing());
    EXPECT_EQ(modelSceneNode->localTransform(), transform);
}

TEST_F(EventHandlerFixture, should_handle_door_loaded_event) {
    // given
    Door door {2, ""};
    door.setPosition({1.0f, 2.0f, 3.0f});
    door.setFacing(glm::radians(90.0f));
    _area.add(door);
    Event event;
    event.type = EventType::ObjectStateChanged;
    event.object.objectId = 2;
    event.object.state = ObjectState::Loaded;
    auto model = std::make_shared<Model>(
        "",
        0,
        std::shared_ptr<ModelNode>(),
        std::vector<std::shared_ptr<Animation>>(),
        "",
        1.0f);
    auto modelSceneNode = std::make_shared<ModelSceneNode>(
        *model,
        ModelUsage::Door,
        _sceneGraph,
        _graphicsModule.services(),
        _audioModule.services(),
        _resourceModule.services());
    auto walkmesh = std::make_shared<Walkmesh>();
    auto walkmeshSceneNode = std::make_shared<WalkmeshSceneNode>(
        *walkmesh,
        _sceneGraph,
        _graphicsModule.services(),
        _audioModule.services(),
        _resourceModule.services());

    // expect
    EXPECT_CALL(_sceneModule.graphs(), get(_)).WillOnce(ReturnRef(_sceneGraph));
    EXPECT_CALL(_resourceModule.models(), get(_)).WillOnce(Return(model));
    EXPECT_CALL(_sceneGraph, newModel(_, ModelUsage::Door)).WillOnce(Return(modelSceneNode));
    EXPECT_CALL(_sceneGraph, addRoot(modelSceneNode));
    EXPECT_CALL(_resourceModule.walkmeshes(), get(_, ResType::Dwk)).WillRepeatedly(Return(walkmesh));
    EXPECT_CALL(_sceneGraph, newWalkmesh(_)).WillRepeatedly(Return(walkmeshSceneNode));
    EXPECT_CALL(_sceneGraph, addRoot(walkmeshSceneNode)).Times(3);
    _subject->handle(event);

    auto transform = glm::translate(door.position());
    transform *= glm::eulerAngleZ(door.facing());
    EXPECT_EQ(modelSceneNode->localTransform(), transform);
    EXPECT_EQ(walkmeshSceneNode->localTransform(), transform);
}

TEST_F(EventHandlerFixture, should_handle_placeable_loaded_event) {
    // given
    Placeable placeable {2, ""};
    placeable.setPosition({1.0f, 2.0f, 3.0f});
    placeable.setFacing(glm::radians(90.0f));
    _area.add(placeable);
    Event event;
    event.type = EventType::ObjectStateChanged;
    event.object.objectId = 2;
    event.object.state = ObjectState::Loaded;
    auto model = std::make_shared<Model>(
        "",
        0,
        std::shared_ptr<ModelNode>(),
        std::vector<std::shared_ptr<Animation>>(),
        "",
        1.0f);
    auto modelSceneNode = std::make_shared<ModelSceneNode>(
        *model,
        ModelUsage::Placeable,
        _sceneGraph,
        _graphicsModule.services(),
        _audioModule.services(),
        _resourceModule.services());
    auto walkmesh = std::make_shared<Walkmesh>();
    auto walkmeshSceneNode = std::make_shared<WalkmeshSceneNode>(
        *walkmesh,
        _sceneGraph,
        _graphicsModule.services(),
        _audioModule.services(),
        _resourceModule.services());

    // expect
    EXPECT_CALL(_sceneModule.graphs(), get(_)).WillOnce(ReturnRef(_sceneGraph));
    EXPECT_CALL(_resourceModule.models(), get(_)).WillOnce(Return(model));
    EXPECT_CALL(_sceneGraph, newModel(_, ModelUsage::Placeable)).WillOnce(Return(modelSceneNode));
    EXPECT_CALL(_sceneGraph, addRoot(modelSceneNode));
    EXPECT_CALL(_resourceModule.walkmeshes(), get(_, ResType::Pwk)).WillRepeatedly(Return(walkmesh));
    EXPECT_CALL(_sceneGraph, newWalkmesh(_)).WillRepeatedly(Return(walkmeshSceneNode));
    EXPECT_CALL(_sceneGraph, addRoot(walkmeshSceneNode));
    _subject->handle(event);

    auto transform = glm::translate(placeable.position());
    transform *= glm::eulerAngleZ(placeable.facing());
    EXPECT_EQ(modelSceneNode->localTransform(), transform);
    EXPECT_EQ(walkmeshSceneNode->localTransform(), transform);
}

TEST_F(EventHandlerFixture, should_handle_object_location_changed_event) {
    // given
    Creature creature {2, ""};
    creature.setPosition({1.0f, 2.0f, 3.0f});
    creature.setFacing(glm::radians(90.0f));
    _area.add(creature);
    Event event;
    event.type = EventType::ObjectLocationChanged;
    event.object.objectId = 2;
    auto model = std::make_shared<Model>(
        "",
        0,
        std::shared_ptr<ModelNode>(),
        std::vector<std::shared_ptr<Animation>>(),
        "",
        1.0f);
    auto modelSceneNode = std::make_shared<ModelSceneNode>(
        *model,
        ModelUsage::Creature,
        _sceneGraph,
        _graphicsModule.services(),
        _audioModule.services(),
        _resourceModule.services());

    // expect
    EXPECT_CALL(_sceneModule.graphs(), get(_)).WillOnce(ReturnRef(_sceneGraph));
    EXPECT_CALL(_sceneGraph, modelByExternalId(_)).WillOnce(Return(std::ref(*modelSceneNode)));
    _subject->handle(event);

    auto transform = glm::rotate(
        glm::translate(creature.position()),
        creature.facing(),
        glm::vec3 {0.0f, 0.0f, 1.0f});
    EXPECT_EQ(modelSceneNode->localTransform(), transform);
}

TEST_F(EventHandlerFixture, should_handle_object_animation_reset_event) {
    // given
    Creature creature {2, ""};
    _area.add(creature);
    Event event;
    event.type = EventType::ObjectAnimationReset;
    event.animation.objectId = 2;
    event.animation.name = "pause1";
    auto animation = std::make_shared<Animation>(
        "pause1",
        1.0f,
        0.25f,
        "",
        std::shared_ptr<ModelNode>(),
        std::vector<Animation::Event>());
    auto model = std::make_shared<Model>(
        "",
        0,
        std::shared_ptr<ModelNode>(),
        std::vector<std::shared_ptr<Animation>> {animation},
        "",
        1.0f);
    auto modelSceneNode = std::make_shared<ModelSceneNode>(
        *model,
        ModelUsage::Creature,
        _sceneGraph,
        _graphicsModule.services(),
        _audioModule.services(),
        _resourceModule.services());

    // expect
    EXPECT_CALL(_sceneModule.graphs(), get(_)).WillOnce(ReturnRef(_sceneGraph));
    EXPECT_CALL(_sceneGraph, modelByExternalId(_)).WillOnce(Return(std::ref(*modelSceneNode)));
    _subject->handle(event);

    EXPECT_EQ(modelSceneNode->activeAnimationName(), "pause1");
    int flags = modelSceneNode->animationChannels().front().properties.flags;
    EXPECT_TRUE((flags & AnimationFlags::loop) != 0);
}

TEST_F(EventHandlerFixture, should_handle_object_fire_forget_animation_fired_event) {
    // given
    Creature creature {2, ""};
    _area.add(creature);
    Event event;
    event.type = EventType::ObjectFireForgetAnimationFired;
    event.animation.objectId = 2;
    event.animation.name = "pause1";
    auto animation = std::make_shared<Animation>(
        "pause1",
        1.0f,
        0.25f,
        "",
        std::shared_ptr<ModelNode>(),
        std::vector<Animation::Event>());
    auto model = std::make_shared<Model>(
        "",
        0,
        std::shared_ptr<ModelNode>(),
        std::vector<std::shared_ptr<Animation>> {animation},
        "",
        1.0f);
    auto modelSceneNode = std::make_shared<ModelSceneNode>(
        *model,
        ModelUsage::Creature,
        _sceneGraph,
        _graphicsModule.services(),
        _audioModule.services(),
        _resourceModule.services());

    // expect
    EXPECT_CALL(_sceneModule.graphs(), get(_)).WillOnce(ReturnRef(_sceneGraph));
    EXPECT_CALL(_sceneGraph, modelByExternalId(_)).WillOnce(Return(std::ref(*modelSceneNode)));
    _subject->handle(event);

    EXPECT_EQ(modelSceneNode->activeAnimationName(), "pause1");
    int flags = modelSceneNode->animationChannels().front().properties.flags;
    EXPECT_TRUE((flags & AnimationFlags::loop) == 0);
}

TEST_F(EventHandlerFixture, should_handle_door_state_changed_event) {
    // given
    Door door {2, ""};
    _area.add(door);
    Event event;
    event.type = EventType::DoorStateChanged;
    event.door.objectId = 2;
    event.door.state = DoorState::Open;
    auto walkmesh = std::make_shared<Walkmesh>();
    auto closedSceneNode = std::make_shared<WalkmeshSceneNode>(
        *walkmesh,
        _sceneGraph,
        _graphicsModule.services(),
        _audioModule.services(),
        _resourceModule.services());
    closedSceneNode->attributes().add("doorState", static_cast<int>(DoorState::Closed));
    auto openSceneNode = std::make_shared<WalkmeshSceneNode>(
        *walkmesh,
        _sceneGraph,
        _graphicsModule.services(),
        _audioModule.services(),
        _resourceModule.services());
    openSceneNode->attributes().add("doorState", static_cast<int>(DoorState::Open));
    std::vector<std::reference_wrapper<WalkmeshSceneNode>> walkmeshSceneNodes {*closedSceneNode, *openSceneNode};

    // expect
    EXPECT_CALL(_sceneModule.graphs(), get(_)).WillOnce(ReturnRef(_sceneGraph));
    EXPECT_CALL(_sceneGraph, walkmeshesByExternalId(_)).WillOnce(Return(walkmeshSceneNodes));
    _subject->handle(event);

    EXPECT_FALSE(closedSceneNode->isEnabled());
    EXPECT_TRUE(openSceneNode->isEnabled());
}
