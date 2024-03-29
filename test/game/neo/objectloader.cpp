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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../../fixtures/graphics.h"
#include "../../fixtures/neogame.h"
#include "../../fixtures/resource.h"
#include "../../fixtures/scene.h"
#include "../../fixtures/system.h"

#include "reone/game/neo/game.h"
#include "reone/game/neo/object/area.h"
#include "reone/game/neo/object/camera.h"
#include "reone/game/neo/object/creature.h"
#include "reone/game/neo/object/door.h"
#include "reone/game/neo/object/encounter.h"
#include "reone/game/neo/object/item.h"
#include "reone/game/neo/object/placeable.h"
#include "reone/game/neo/object/sound.h"
#include "reone/game/neo/object/store.h"
#include "reone/game/neo/object/trigger.h"
#include "reone/game/neo/object/waypoint.h"
#include "reone/resource/provider/gffs.h"

using namespace reone;
using namespace reone::audio;
using namespace reone::game;
using namespace reone::game::neo;
using namespace reone::resource;
using namespace reone::scene;

using testing::_;
using testing::Return;
using testing::ReturnRef;
using testing::Test;

class ObjectLoaderFixture : public Test {
protected:
    void SetUp() override {
        _resourceModule.init();

        _area = std::make_unique<Area>(0, "");
        _camera = std::make_unique<Camera>(1, "");
        _creature = std::make_unique<Creature>(2, "");
        _door = std::make_unique<Door>(3, "");
        _encounter = std::make_unique<Encounter>(4, "");
        _item = std::make_unique<Item>(5, "");
        _module = std::make_unique<Module>(6, "");
        _placeable = std::make_unique<Placeable>(7, "");
        _sound = std::make_unique<Sound>(8, "");
        _store = std::make_unique<Store>(9, "");
        _trigger = std::make_unique<Trigger>(10, "");
        _waypoint = std::make_unique<Waypoint>(11, "");

        _subject = std::make_unique<ObjectLoader>(
            _objectFactory,
            _resourceModule.services());
    }

    TestResourceModule _resourceModule;
    MockObjectFactory _objectFactory;
    MockObjectLoader _objectLoader;
    MockActionExecutor _actionExecutor;

    std::unique_ptr<Area> _area;
    std::unique_ptr<Camera> _camera;
    std::unique_ptr<Creature> _creature;
    std::unique_ptr<Door> _door;
    std::unique_ptr<Encounter> _encounter;
    std::unique_ptr<Item> _item;
    std::unique_ptr<Module> _module;
    std::unique_ptr<Placeable> _placeable;
    std::unique_ptr<Sound> _sound;
    std::unique_ptr<Store> _store;
    std::unique_ptr<Trigger> _trigger;
    std::unique_ptr<Waypoint> _waypoint;

    std::unique_ptr<ObjectLoader> _subject;
};

TEST_F(ObjectLoaderFixture, should_load_module) {
    // given
    auto ifo = Gff::Builder().build();

    // expect
    EXPECT_CALL(_resourceModule.gffs(), get(_, _)).WillOnce(Return(ifo));
    EXPECT_CALL(_objectFactory, newModule(_)).WillOnce(ReturnRef(*_module));
    auto &module = _subject->loadModule("end_m01aa");
}

TEST_F(ObjectLoaderFixture, should_load_area) {
    // given
    auto are = Gff::Builder().build();
    auto git = Gff::Builder().build();
    auto layout = std::make_shared<Layout>();

    // expect
    EXPECT_CALL(_resourceModule.gffs(), get(_, ResType::Are)).WillOnce(Return(are));
    EXPECT_CALL(_resourceModule.gffs(), get(_, ResType::Git)).WillOnce(Return(git));
    EXPECT_CALL(_resourceModule.layouts(), get(_)).WillOnce(Return(layout));
    EXPECT_CALL(_objectFactory, newArea(_)).WillOnce(ReturnRef(*_area));
    auto &area = _subject->loadArea("m01aa");
}

TEST_F(ObjectLoaderFixture, should_load_camera) {
    // expect
    EXPECT_CALL(_objectFactory, newCamera(_)).WillOnce(ReturnRef(*_camera));
    auto &camera = _subject->loadCamera();
}

TEST_F(ObjectLoaderFixture, should_load_creature) {
    // given
    auto utc = Gff::Builder().build();
    std::shared_ptr<TwoDA> appearance = TwoDA::Builder()
                                            .columns({"modeltype", "race", "racetex"})
                                            .row({"F", "n_mandalorian", ""})
                                            .build();
    std::shared_ptr<TwoDA> heads = TwoDA::Builder().build();

    // expect
    EXPECT_CALL(_resourceModule.gffs(), get(_, _)).WillOnce(Return(utc));
    EXPECT_CALL(_resourceModule.twoDas(), get("appearance")).WillOnce(Return(appearance));
    EXPECT_CALL(_resourceModule.twoDas(), get("heads")).WillOnce(Return(heads));
    EXPECT_CALL(_objectFactory, newCreature(_)).WillOnce(ReturnRef(*_creature));
    auto &creature = _subject->loadCreature({"tmplt"});
}

TEST_F(ObjectLoaderFixture, should_load_creature_for_portrait) {
    // given
    std::shared_ptr<TwoDA> portraits = TwoDA::Builder()
                                           .columns({"appearancenumber"})
                                           .row({"0"})
                                           .build();
    std::shared_ptr<TwoDA> appearance = TwoDA::Builder()
                                            .columns({"modeltype", "race", "racetex"})
                                            .row({"F", "n_mandalorian", ""})
                                            .build();
    std::shared_ptr<TwoDA> heads = TwoDA::Builder().build();

    // expect
    EXPECT_CALL(_resourceModule.twoDas(), get("portraits")).WillOnce(Return(portraits));
    EXPECT_CALL(_resourceModule.twoDas(), get("appearance")).WillOnce(Return(appearance));
    EXPECT_CALL(_resourceModule.twoDas(), get("heads")).WillOnce(Return(heads));
    EXPECT_CALL(_objectFactory, newCreature(_)).WillOnce(ReturnRef(*_creature));
    auto &creature = _subject->loadCreature("tag", 0);
}

TEST_F(ObjectLoaderFixture, should_load_door) {
    // given
    auto utd = Gff::Builder().build();
    std::shared_ptr<TwoDA> genericDoors = TwoDA::Builder()
                                              .columns({"modelname"})
                                              .row({"dor_lhr01"})
                                              .build();

    // expect
    EXPECT_CALL(_resourceModule.gffs(), get(_, _)).WillOnce(Return(utd));
    EXPECT_CALL(_resourceModule.twoDas(), get("genericdoors")).WillOnce(Return(genericDoors));
    EXPECT_CALL(_objectFactory, newDoor(_)).WillOnce(ReturnRef(*_door));
    auto &door = _subject->loadDoor({"tmplt"});
}

TEST_F(ObjectLoaderFixture, should_load_encounter) {
    // given
    auto ute = Gff::Builder().build();

    // expect
    EXPECT_CALL(_resourceModule.gffs(), get(_, _)).WillOnce(Return(ute));
    EXPECT_CALL(_objectFactory, newEncounter(_)).WillOnce(ReturnRef(*_encounter));
    auto &encounter = _subject->loadEncounter({"tmplt"});
}

TEST_F(ObjectLoaderFixture, should_load_placeable) {
    // given
    auto utp = Gff::Builder().build();

    // expect
    EXPECT_CALL(_resourceModule.gffs(), get(_, _)).WillOnce(Return(utp));
    std::shared_ptr<TwoDA> placeables = TwoDA::Builder()
                                            .columns({"modelname"})
                                            .row({"plc_footlker"})
                                            .build();
    EXPECT_CALL(_resourceModule.twoDas(), get("placeables")).WillOnce(Return(placeables));
    EXPECT_CALL(_objectFactory, newPlaceable(_)).WillOnce(ReturnRef(*_placeable));
    auto &placeable = _subject->loadPlaceable({"tmplt"});
}

TEST_F(ObjectLoaderFixture, should_load_sound) {
    // given
    auto uts = Gff::Builder().build();

    // expect
    EXPECT_CALL(_resourceModule.gffs(), get(_, _)).WillOnce(Return(uts));
    EXPECT_CALL(_objectFactory, newSound(_)).WillOnce(ReturnRef(*_sound));
    auto &sound = _subject->loadSound({"tmplt"});
}

TEST_F(ObjectLoaderFixture, should_load_store) {
    // given
    auto utm = Gff::Builder().build();

    // expect
    EXPECT_CALL(_resourceModule.gffs(), get(_, _)).WillOnce(Return(utm));
    EXPECT_CALL(_objectFactory, newStore(_)).WillOnce(ReturnRef(*_store));
    auto &store = _subject->loadStore({"tmplt"});
}

TEST_F(ObjectLoaderFixture, should_load_trigger) {
    // given
    auto utt = Gff::Builder().build();

    // expect
    EXPECT_CALL(_resourceModule.gffs(), get(_, _)).WillOnce(Return(utt));
    EXPECT_CALL(_objectFactory, newTrigger(_)).WillOnce(ReturnRef(*_trigger));
    auto &trigger = _subject->loadTrigger({"tmplt"});
}

TEST_F(ObjectLoaderFixture, should_load_waypoint) {
    // given
    auto utw = Gff::Builder().build();

    // expect
    EXPECT_CALL(_resourceModule.gffs(), get(_, _)).WillOnce(Return(utw));
    EXPECT_CALL(_objectFactory, newWaypoint(_)).WillOnce(ReturnRef(*_waypoint));
    auto &waypoint = _subject->loadWaypoint({"tmplt"});
}
