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

#include "../../../fixtures/neogame.h"

#include "reone/resource/layout.h"
#include "reone/resource/parser/gff/are.h"
#include "reone/resource/parser/gff/git.h"
#include "reone/resource/path.h"

using namespace reone::game;
using namespace reone::game::neo;
using namespace reone::resource;
using namespace reone::resource::generated;

using testing::_;
using testing::ReturnRef;

class MockCreature : public Creature {
public:
    MockCreature(MockGame &game) :
        Creature(1, "", game, game) {
    }

    MOCK_METHOD(void, update, (float), (override));
};

TEST(area, should_load_are_and_git) {
    // given
    MockGame game;
    Area area {0, "", game, game, game};
    ARE_Rooms room;
    room.RoomName = "m01aa_01a";
    ARE_Rooms room2;
    room2.RoomName = "m01aa_02a";
    ARE are;
    are.Rooms.push_back(room);
    are.Rooms.push_back(room2);
    GIT git;
    GIT_CameraList gitCamera;
    GIT_Creature_List gitCreature;
    GIT_Door_List gitDoor;
    GIT_Encounter_List gitEncounter;
    GIT_Placeable_List gitPlaceable;
    GIT_SoundList gitSound;
    GIT_StoreList gitStore;
    GIT_TriggerList gitTrigger;
    GIT_WaypointList gitWaypoint;
    git.CameraList.push_back(gitCamera);
    git.Creature_List.push_back(gitCreature);
    git.Door_List.push_back(gitDoor);
    git.Encounter_List.push_back(gitEncounter);
    git.Placeable_List.push_back(gitPlaceable);
    git.SoundList.push_back(gitSound);
    git.StoreList.push_back(gitStore);
    git.TriggerList.push_back(gitTrigger);
    git.WaypointList.push_back(gitWaypoint);
    Layout lyt;
    lyt.rooms.push_back({"m01aa_01a", glm::vec3 {0.0f}});
    lyt.rooms.push_back({"m01aa_02a", glm::vec3 {1.0f}});
    Visibility vis;
    vis.insert({"m01aa_01a", "m01aa_02a"});
    vis.insert({"m01aa_02a", "m01aa_01a"});
    Path pth;
    Camera camera {0, "", game, game};
    Creature creature {1, "", game, game};
    Door door {2, "", game, game};
    Encounter encounter {3, "", game, game};
    Placeable placeable {4, "", game, game};
    Sound sound {5, "", game, game};
    Store store {6, "", game, game};
    Trigger trigger {7, "", game, game};
    Waypoint waypoint {8, "", game, game};

    // expect
    EXPECT_CALL(game, loadCamera()).WillOnce(ReturnRef(camera));
    EXPECT_CALL(game, loadCreature(_)).WillOnce(ReturnRef(creature));
    EXPECT_CALL(game, loadDoor(_)).WillOnce(ReturnRef(door));
    EXPECT_CALL(game, loadEncounter(_)).WillOnce(ReturnRef(encounter));
    EXPECT_CALL(game, loadPlaceable(_)).WillOnce(ReturnRef(placeable));
    EXPECT_CALL(game, loadSound(_)).WillOnce(ReturnRef(sound));
    EXPECT_CALL(game, loadStore(_)).WillOnce(ReturnRef(store));
    EXPECT_CALL(game, loadTrigger(_)).WillOnce(ReturnRef(trigger));
    EXPECT_CALL(game, loadWaypoint(_)).WillOnce(ReturnRef(waypoint));
    area.load(are, git, lyt, vis, pth);
    EXPECT_TRUE(area.is(ObjectState::Loaded));
    EXPECT_EQ(area.rooms().size(), 2);
    EXPECT_EQ(area.rooms().at(0).model, "m01aa_01a");
    EXPECT_EQ(area.rooms().at(0).position, (glm::vec3 {0.0f}));
    EXPECT_EQ(area.rooms().at(0).visibleRooms, (std::set {std::string {"m01aa_02a"}}));
    EXPECT_EQ(area.rooms().at(1).model, "m01aa_02a");
    EXPECT_EQ(area.rooms().at(1).position, (glm::vec3 {1.0f}));
    EXPECT_EQ(area.rooms().at(1).visibleRooms, (std::set {std::string {"m01aa_01a"}}));
}

TEST(area, should_add_objects) {
    // given
    MockGame game;
    Area area {0, "", game, game, game};
    Camera camera {1, "", game, game};
    Creature creature {2, "", game, game};
    Door door {3, "", game, game};
    Encounter encounter {4, "", game, game};
    Placeable placeable {5, "", game, game};
    Sound sound {6, "", game, game};
    Store store {7, "", game, game};
    Trigger trigger {8, "", game, game};
    Waypoint waypoint {9, "", game, game};

    // when
    area.add(camera);
    area.add(creature);
    area.add(door);
    area.add(encounter);
    area.add(placeable);
    area.add(sound);
    area.add(store);
    area.add(trigger);
    area.add(waypoint);

    // then
    auto &cameras = area.cameras();
    auto &creatures = area.creatures();
    auto &doors = area.doors();
    auto &encounters = area.encounters();
    auto &placeables = area.placeables();
    auto &sounds = area.sounds();
    auto &stores = area.stores();
    auto &triggers = area.triggers();
    auto &waypoints = area.waypoints();
    auto &objects = area.objects();
    EXPECT_EQ(cameras.size(), 1);
    EXPECT_EQ(creatures.size(), 1);
    EXPECT_EQ(doors.size(), 1);
    EXPECT_EQ(encounters.size(), 1);
    EXPECT_EQ(placeables.size(), 1);
    EXPECT_EQ(sounds.size(), 1);
    EXPECT_EQ(stores.size(), 1);
    EXPECT_EQ(triggers.size(), 1);
    EXPECT_EQ(waypoints.size(), 1);
    EXPECT_EQ(objects.size(), 9);
    EXPECT_EQ(cameras.front().get(), camera);
    EXPECT_EQ(creatures.front().get(), creature);
    EXPECT_EQ(doors.front().get(), door);
    EXPECT_EQ(encounters.front().get(), encounter);
    EXPECT_EQ(placeables.front().get(), placeable);
    EXPECT_EQ(sounds.front().get(), sound);
    EXPECT_EQ(stores.front().get(), store);
    EXPECT_EQ(triggers.front().get(), trigger);
    EXPECT_EQ(waypoints.front().get(), waypoint);
}

TEST(area, should_update_objects_on_update) {
    // given
    MockGame game;
    Area area {0, "", game, game, game};
    MockCreature creature {game};
    area.add(creature);

    // expect
    EXPECT_CALL(creature, update(_));
    area.update(1.0f);
}
