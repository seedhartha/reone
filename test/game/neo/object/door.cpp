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

#include "../../../fixtures/neogame.h"

#include "reone/game/neo/object/door.h"
#include "reone/resource/2da.h"
#include "reone/resource/parser/2da/genericdoors.h"
#include "reone/resource/parser/gff/utd.h"

using namespace reone::game;
using namespace reone::game::neo;
using namespace reone::resource;
using namespace reone::resource::generated;

TEST(door, should_load_utd) {
    // given
    MockEventCollector eventCollector;
    Door door {0, "", eventCollector};
    UTD utd;
    utd.GenericType = 0;
    GenericdoorsTwoDARow genericDoorsRow;
    genericDoorsRow.modelname = "dor_lhr01";
    GenericdoorsTwoDA genericDoors;
    genericDoors.rows.push_back(std::move(genericDoorsRow));

    // when
    door.load(utd, genericDoors);

    // then
    EXPECT_TRUE(door.is(ObjectState::Loaded));
    EXPECT_EQ(door.modelName().value(), "dor_lhr01");
}
