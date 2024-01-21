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

#include "reone/game/neo/object/placeable.h"
#include "reone/resource/2da.h"
#include "reone/resource/template/generated/utp.h"

using namespace reone::game;
using namespace reone::game::neo;
using namespace reone::resource;
using namespace reone::resource::generated;

TEST(placeable, should_load_utp) {
    // given
    Placeable placeable {0, ""};
    UTP utp;
    utp.Appearance = 0;
    TwoDA placeables {
        {"modelname"},
        {TwoDA::newRow({"plc_footlker"})}};

    // when
    placeable.load(utp, placeables);

    // then
    EXPECT_TRUE(placeable.is(ObjectState::Loaded));
    EXPECT_EQ(placeable.modelName().value(), "plc_footlker");
}
