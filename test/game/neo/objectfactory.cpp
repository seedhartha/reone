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

#include "../../fixtures/neogame.h"

#include "reone/game/neo/object/area.h"
#include "reone/game/neo/object/camera.h"
#include "reone/game/neo/object/creature.h"
#include "reone/game/neo/object/door.h"
#include "reone/game/neo/object/encounter.h"
#include "reone/game/neo/object/item.h"
#include "reone/game/neo/object/module.h"
#include "reone/game/neo/object/placeable.h"
#include "reone/game/neo/object/sound.h"
#include "reone/game/neo/object/store.h"
#include "reone/game/neo/object/trigger.h"
#include "reone/game/neo/object/waypoint.h"
#include "reone/game/neo/objectfactory.h"

using namespace reone::game::neo;

TEST(object_factory, should_instantiate_objects_with_incrementing_id) {
    // given
    ObjectFactory subject;

    // when
    auto &area = subject.newArea("");
    auto &camera = subject.newCamera("");
    auto &creature = subject.newCreature("");
    auto &door = subject.newDoor("");
    auto &encounter = subject.newEncounter("");
    auto &item = subject.newItem("");
    auto &module = subject.newModule("");
    auto &placeable = subject.newPlaceable("");
    auto &sound = subject.newSound("");
    auto &store = subject.newStore("");
    auto &trigger = subject.newTrigger("");
    auto &waypoint = subject.newWaypoint("");

    // then
    EXPECT_EQ(area.id(), 2);
    EXPECT_EQ(camera.id(), 3);
    EXPECT_EQ(creature.id(), 4);
    EXPECT_EQ(door.id(), 5);
    EXPECT_EQ(encounter.id(), 6);
    EXPECT_EQ(item.id(), 7);
    EXPECT_EQ(module.id(), 8);
    EXPECT_EQ(placeable.id(), 9);
    EXPECT_EQ(sound.id(), 10);
    EXPECT_EQ(store.id(), 11);
    EXPECT_EQ(trigger.id(), 12);
    EXPECT_EQ(waypoint.id(), 13);
}