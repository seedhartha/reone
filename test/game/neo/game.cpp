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

class TestModule : public Module {
public:
    TestModule(IAreaLoader &areaLoader) :
        Module(0, "", areaLoader) {
    }

    MOCK_METHOD(void, update, (float), (override));
};

TEST(game, should_return_nullopt_for_current_module) {
    // given
    TestOptions options;
    TestResourceModule resourceModule;
    resourceModule.init();
    TestSceneModule sceneModule;
    sceneModule.init();
    Game game {options, resourceModule.services(), sceneModule.services()};

    // when
    auto module = game.module();

    // then
    EXPECT_FALSE(module.has_value());
}

TEST(game, should_start_module) {
    // given
    TestOptions options;
    TestResourceModule resourceModule;
    resourceModule.init();
    TestSceneModule sceneModule;
    sceneModule.init();
    Game game {options, resourceModule.services(), sceneModule.services()};
    auto ifo = Gff::Builder().build();

    // when
    EXPECT_CALL(resourceModule.gffs(), get(_, _)).WillOnce(Return(ifo));
    game.startModule("end_m01aa");

    // then
    auto currentModule = game.module();
    EXPECT_TRUE(currentModule.has_value());
}

TEST(game, should_load_camera) {
    // given
    TestOptions options;
    TestResourceModule resourceModule;
    resourceModule.init();
    TestSceneModule sceneModule;
    sceneModule.init();
    Game game {options, resourceModule.services(), sceneModule.services()};

    // when
    auto &camera = game.loadCamera();
}

TEST(game, should_load_creature) {
    // given
    TestOptions options;
    TestResourceModule resourceModule;
    resourceModule.init();
    TestSceneModule sceneModule;
    sceneModule.init();
    Game game {options, resourceModule.services(), sceneModule.services()};
    auto utc = Gff::Builder().build();
    auto appearance2da = std::make_shared<TwoDA>(
        std::vector<std::string> {
            "modeltype", "race", "racetex"},
        std::vector<TwoDA::Row> {
            TwoDA::Row {std::vector<std::string> {"F", "n_mandalorian", ""}}});
    auto heads2da = std::make_shared<TwoDA>(std::vector<std::string> {}, std::vector<TwoDA::Row> {});

    // expect
    EXPECT_CALL(resourceModule.gffs(), get(_, _)).WillOnce(Return(utc));
    EXPECT_CALL(resourceModule.twoDas(), get("appearance")).WillOnce(Return(appearance2da));
    EXPECT_CALL(resourceModule.twoDas(), get("heads")).WillOnce(Return(heads2da));
    auto &creature = game.loadCreature({"tmplt"});
}

TEST(game, should_load_door) {
    // given
    TestOptions options;
    TestResourceModule resourceModule;
    resourceModule.init();
    TestSceneModule sceneModule;
    sceneModule.init();
    Game game {options, resourceModule.services(), sceneModule.services()};
    auto utd = Gff::Builder().build();

    // expect
    EXPECT_CALL(resourceModule.gffs(), get(_, _)).WillOnce(Return(utd));
    auto &door = game.loadDoor({"tmplt"});
}

TEST(game, should_load_encounter) {
    // given
    TestOptions options;
    TestResourceModule resourceModule;
    resourceModule.init();
    TestSceneModule sceneModule;
    sceneModule.init();
    Game game {options, resourceModule.services(), sceneModule.services()};
    auto ute = Gff::Builder().build();

    // expect
    EXPECT_CALL(resourceModule.gffs(), get(_, _)).WillOnce(Return(ute));
    auto &encounter = game.loadEncounter({"tmplt"});
}

TEST(game, should_load_placeable) {
    // given
    TestOptions options;
    TestResourceModule resourceModule;
    resourceModule.init();
    TestSceneModule sceneModule;
    sceneModule.init();
    Game game {options, resourceModule.services(), sceneModule.services()};
    auto utp = Gff::Builder().build();

    // expect
    EXPECT_CALL(resourceModule.gffs(), get(_, _)).WillOnce(Return(utp));
    auto &placeable = game.loadPlaceable({"tmplt"});
}

TEST(game, should_load_sound) {
    // given
    TestOptions options;
    TestResourceModule resourceModule;
    resourceModule.init();
    TestSceneModule sceneModule;
    sceneModule.init();
    Game game {options, resourceModule.services(), sceneModule.services()};
    auto uts = Gff::Builder().build();

    // expect
    EXPECT_CALL(resourceModule.gffs(), get(_, _)).WillOnce(Return(uts));
    auto &sound = game.loadSound({"tmplt"});
}

TEST(game, should_load_store) {
    // given
    TestOptions options;
    TestResourceModule resourceModule;
    resourceModule.init();
    TestSceneModule sceneModule;
    sceneModule.init();
    Game game {options, resourceModule.services(), sceneModule.services()};
    auto utm = Gff::Builder().build();

    // expect
    EXPECT_CALL(resourceModule.gffs(), get(_, _)).WillOnce(Return(utm));
    auto &store = game.loadStore({"tmplt"});
}

TEST(game, should_load_trigger) {
    // given
    TestOptions options;
    TestResourceModule resourceModule;
    resourceModule.init();
    TestSceneModule sceneModule;
    sceneModule.init();
    Game game {options, resourceModule.services(), sceneModule.services()};
    auto utt = Gff::Builder().build();

    // expect
    EXPECT_CALL(resourceModule.gffs(), get(_, _)).WillOnce(Return(utt));
    auto &trigger = game.loadTrigger({"tmplt"});
}

TEST(game, should_load_waypoint) {
    // given
    TestOptions options;
    TestResourceModule resourceModule;
    resourceModule.init();
    TestSceneModule sceneModule;
    sceneModule.init();
    Game game {options, resourceModule.services(), sceneModule.services()};
    auto utw = Gff::Builder().build();

    // expect
    EXPECT_CALL(resourceModule.gffs(), get(_, _)).WillOnce(Return(utw));
    auto &waypoint = game.loadWaypoint({"tmplt"});
}

TEST(game, should_instantiate_objects_with_incrementing_id) {
    // given
    TestOptions options;
    TestResourceModule resourceModule;
    resourceModule.init();
    TestSceneModule sceneModule;
    sceneModule.init();
    Game game {options, resourceModule.services(), sceneModule.services()};

    // when
    auto &area = game.newArea("");
    auto &camera = game.newCamera("");
    auto &creature = game.newCreature("");
    auto &door = game.newDoor("");
    auto &encounter = game.newEncounter("");
    auto &item = game.newItem("");
    auto &placeable = game.newPlaceable("");
    auto &sound = game.newSound("");
    auto &store = game.newStore("");
    auto &trigger = game.newTrigger("");
    auto &waypoint = game.newWaypoint("");

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