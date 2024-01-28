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

#include "reone/game/neo/object/area.h"
#include "reone/game/neo/object/module.h"
#include "reone/resource/parser/gff/ifo.h"

using namespace reone::game;
using namespace reone::game::neo;
using namespace reone::resource::generated;

using testing::_;
using testing::ReturnRef;

TEST(module, should_load_ifo) {
    // given
    MockObjectLoader objectLoader;
    Module module {0, ""};
    IFO ifo;
    ifo.Mod_Entry_Area = "m01aa";
    ifo.Mod_Entry_X = 1.0f;
    ifo.Mod_Entry_Y = 2.0f;
    ifo.Mod_Entry_Z = 3.0f;
    ifo.Mod_Entry_Dir_X = 0.70710677f;
    ifo.Mod_Entry_Dir_Y = -0.70710677f;
    ifo.Mod_Area_list.push_back({"m01aa"});
    Area area {1, ""};

    // expect
    EXPECT_CALL(objectLoader, loadArea(_)).WillOnce(ReturnRef(area));
    module.load(objectLoader, ifo);
    EXPECT_TRUE(module.is(ObjectState::Loaded));
    EXPECT_EQ(module.area(), area);
    EXPECT_EQ(module.entryPosition(), (glm::vec3 {1.0f, 2.0f, 3.0f}));
    EXPECT_EQ(module.entryFacing(), glm::radians(-135.0f));
}

TEST(module, should_throw_for_area_when_has_no_areas) {
    // given
    Module module {0, ""};

    // expect
    EXPECT_THROW(module.area(), std::logic_error);
}

class MockArea : public Area {
public:
    MockArea(ObjectId objectId,
             ObjectTag tag) :
        Area(
            objectId,
            std::move(tag)) {
    }

    MOCK_METHOD(void, update, (IActionExecutor & actionExecutor, float), (override));
};
TEST(module, should_update_current_area_on_update) {
    // given
    MockObjectLoader objectLoader;
    MockActionExecutor actionExecutor;
    Module module {0, ""};
    MockArea area {1, ""};
    EXPECT_CALL(objectLoader, loadArea(_)).WillOnce(ReturnRef(area));
    IFO ifo;
    ifo.Mod_Area_list.push_back({"m01aa"});
    ifo.Mod_Entry_Area = "m01aa";
    module.load(objectLoader, ifo);

    // expect
    EXPECT_CALL(area, update(_, _));
    module.update(actionExecutor, 1.0f);
}
