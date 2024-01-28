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

TEST(area, should_load_are_and_git) {
    // given
    MockObjectLoader objectLoader;
    Area area {0, ""};
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
    Camera camera {1, ""};
    Creature creature {2, ""};
    Door door {3, ""};
    Encounter encounter {4, ""};
    Placeable placeable {5, ""};
    Sound sound {6, ""};
    Store store {7, ""};
    Trigger trigger {8, ""};
    Waypoint waypoint {9, ""};

    // expect
    EXPECT_CALL(objectLoader, loadCamera()).WillOnce(ReturnRef(camera));
    EXPECT_CALL(objectLoader, loadCreature(_)).WillOnce(ReturnRef(creature));
    EXPECT_CALL(objectLoader, loadDoor(_)).WillOnce(ReturnRef(door));
    EXPECT_CALL(objectLoader, loadEncounter(_)).WillOnce(ReturnRef(encounter));
    EXPECT_CALL(objectLoader, loadPlaceable(_)).WillOnce(ReturnRef(placeable));
    EXPECT_CALL(objectLoader, loadSound(_)).WillOnce(ReturnRef(sound));
    EXPECT_CALL(objectLoader, loadStore(_)).WillOnce(ReturnRef(store));
    EXPECT_CALL(objectLoader, loadTrigger(_)).WillOnce(ReturnRef(trigger));
    EXPECT_CALL(objectLoader, loadWaypoint(_)).WillOnce(ReturnRef(waypoint));
    area.load(objectLoader, are, git, lyt, vis, pth);
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
    Area area {0, ""};
    Camera camera {1, ""};
    Creature creature {2, ""};
    Door door {3, ""};
    Encounter encounter {4, ""};
    Placeable placeable {5, ""};
    Sound sound {6, ""};
    Store store {7, ""};
    Trigger trigger {8, ""};
    Waypoint waypoint {9, ""};

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

class MockCreature : public Creature {
public:
    MockCreature(ObjectId objectId,
                 ObjectTag tag) :
        Creature(
            objectId,
            std::move(tag)) {
    }

    MOCK_METHOD(void, update, (IActionExecutor &, float), (override));
};

TEST(area, should_update_objects_on_update) {
    // given
    MockActionExecutor actionExecutor;
    Area area {0, ""};
    MockCreature creature {1, ""};
    area.add(creature);

    // expect
    EXPECT_CALL(creature, update(_, _));
    area.update(actionExecutor, 1.0f);
}

TEST(area, should_find_object_by_id) {
    // given
    Creature creature {1, ""};
    Area area {0, ""};
    area.add(creature);

    // expect
    std::optional<std::reference_wrapper<Object>> found;

    found = area.objectById(2);
    EXPECT_FALSE(found.has_value());

    found = area.objectById(0);
    EXPECT_FALSE(found.has_value());

    found = area.objectById(1);
    EXPECT_TRUE(found.has_value());
    EXPECT_EQ(found->get(), creature);
}

TEST(area, should_find_object_by_tag) {
    // given
    Creature creature1 {1, "tag1"};
    Creature creature2 {2, "tag2"};
    Creature creature3 {3, "tag1"};
    Area area {0, ""};
    area.add(creature1);
    area.add(creature2);
    area.add(creature3);

    // expect
    std::optional<std::reference_wrapper<Object>> found;

    found = area.objectByTag("tag1", 0);
    EXPECT_TRUE(found.has_value());
    EXPECT_EQ(found->get(), creature1);

    found = area.objectByTag("tag1", 1);
    EXPECT_TRUE(found.has_value());
    EXPECT_EQ(found->get(), creature3);

    found = area.objectByTag("tag1", 2);
    EXPECT_FALSE(found.has_value());
}
