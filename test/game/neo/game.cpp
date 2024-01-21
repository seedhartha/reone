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
#include "../../fixtures/resource.h"
#include "../../fixtures/scene.h"

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

using namespace reone::audio;
using namespace reone::game;
using namespace reone::game::neo;
using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

using testing::_;
using testing::Return;
using testing::Test;

class TestOptions : public OptionsView {
public:
    TestOptions() :
        OptionsView(_game, _graphics, _audio) {
    }

private:
    GameOptions _game;
    GraphicsOptions _graphics;
    AudioOptions _audio;
};

class MockModule : public Module {
public:
    MockModule(IAreaLoader &areaLoader) :
        Module(0, "", areaLoader) {
    }

    MOCK_METHOD(void, update, (float), (override));
};

class GameFixture : public Test {
protected:
    void SetUp() override {
        _graphicsModule.init();
        _resourceModule.init();
        _sceneModule.init();
        _game = std::make_unique<Game>(
            _options,
            _graphicsModule.services(),
            _resourceModule.services(),
            _sceneModule.services());
    }

    TestOptions _options;
    TestGraphicsModule _graphicsModule;
    TestResourceModule _resourceModule;
    TestSceneModule _sceneModule;

    std::unique_ptr<Game> _game;
};

TEST_F(GameFixture, should_return_nullopt_for_current_module) {
    // when
    auto module = _game->module();

    // then
    EXPECT_FALSE(module.has_value());
}

TEST_F(GameFixture, should_start_module) {
    // given
    auto ifo = Gff::Builder().build();

    // when
    EXPECT_CALL(_resourceModule.gffs(), get(_, _)).WillOnce(Return(ifo));
    _game->startModule("end_m01aa");

    // then
    auto currentModule = _game->module();
    EXPECT_TRUE(currentModule.has_value());
}

TEST_F(GameFixture, should_load_camera) {
    // when
    auto &camera = _game->loadCamera();
}

TEST_F(GameFixture, should_load_creature) {
    // given
    auto utc = Gff::Builder().build();
    auto appearance2da = std::make_shared<TwoDA>(
        std::vector<std::string> {
            "modeltype", "race", "racetex"},
        std::vector<TwoDA::Row> {
            TwoDA::Row {std::vector<std::string> {"F", "n_mandalorian", ""}}});
    auto heads2da = std::make_shared<TwoDA>(std::vector<std::string> {}, std::vector<TwoDA::Row> {});

    // expect
    EXPECT_CALL(_resourceModule.gffs(), get(_, _)).WillOnce(Return(utc));
    EXPECT_CALL(_resourceModule.twoDas(), get("appearance")).WillOnce(Return(appearance2da));
    EXPECT_CALL(_resourceModule.twoDas(), get("heads")).WillOnce(Return(heads2da));
    auto &creature = _game->loadCreature({"tmplt"});
}

TEST_F(GameFixture, should_load_door) {
    // given
    auto utd = Gff::Builder().build();

    // expect
    EXPECT_CALL(_resourceModule.gffs(), get(_, _)).WillOnce(Return(utd));
    auto &door = _game->loadDoor({"tmplt"});
}

TEST_F(GameFixture, should_load_encounter) {
    // given
    auto ute = Gff::Builder().build();

    // expect
    EXPECT_CALL(_resourceModule.gffs(), get(_, _)).WillOnce(Return(ute));
    auto &encounter = _game->loadEncounter({"tmplt"});
}

TEST_F(GameFixture, should_load_placeable) {
    // given
    auto utp = Gff::Builder().build();

    // expect
    EXPECT_CALL(_resourceModule.gffs(), get(_, _)).WillOnce(Return(utp));
    auto &placeable = _game->loadPlaceable({"tmplt"});
}

TEST_F(GameFixture, should_load_sound) {
    // given
    auto uts = Gff::Builder().build();

    // expect
    EXPECT_CALL(_resourceModule.gffs(), get(_, _)).WillOnce(Return(uts));
    auto &sound = _game->loadSound({"tmplt"});
}

TEST_F(GameFixture, should_load_store) {
    // given
    auto utm = Gff::Builder().build();

    // expect
    EXPECT_CALL(_resourceModule.gffs(), get(_, _)).WillOnce(Return(utm));
    auto &store = _game->loadStore({"tmplt"});
}

TEST_F(GameFixture, should_load_trigger) {
    // given
    auto utt = Gff::Builder().build();

    // expect
    EXPECT_CALL(_resourceModule.gffs(), get(_, _)).WillOnce(Return(utt));
    auto &trigger = _game->loadTrigger({"tmplt"});
}

TEST_F(GameFixture, should_load_waypoint) {
    // given
    auto utw = Gff::Builder().build();

    // expect
    EXPECT_CALL(_resourceModule.gffs(), get(_, _)).WillOnce(Return(utw));
    auto &waypoint = _game->loadWaypoint({"tmplt"});
}

TEST_F(GameFixture, should_instantiate_objects_with_incrementing_id) {
    // when
    auto &area = _game->newArea("");
    auto &camera = _game->newCamera("");
    auto &creature = _game->newCreature("");
    auto &door = _game->newDoor("");
    auto &encounter = _game->newEncounter("");
    auto &item = _game->newItem("");
    auto &placeable = _game->newPlaceable("");
    auto &sound = _game->newSound("");
    auto &store = _game->newStore("");
    auto &trigger = _game->newTrigger("");
    auto &waypoint = _game->newWaypoint("");

    // then
    EXPECT_EQ(area.id(), 2);
    EXPECT_EQ(camera.id(), 3);
    EXPECT_EQ(creature.id(), 4);
    EXPECT_EQ(door.id(), 5);
    EXPECT_EQ(encounter.id(), 6);
    EXPECT_EQ(item.id(), 7);
    EXPECT_EQ(placeable.id(), 8);
    EXPECT_EQ(sound.id(), 9);
    EXPECT_EQ(store.id(), 10);
    EXPECT_EQ(trigger.id(), 11);
    EXPECT_EQ(waypoint.id(), 12);
}