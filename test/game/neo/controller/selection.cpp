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

#include "../../../fixtures/scene.h"

#include "reone/game/neo/controller/selection.h"
#include "reone/game/neo/object/module.h"
#include "reone/graphics/options.h"

using namespace reone::game::neo;
using namespace reone::graphics;
using namespace reone::scene;

TEST(selection_controller, should_xxx) {
    // given
    GraphicsOptions graphicsOpt;
    MockSceneGraph scene;
    SelectionController subject {graphicsOpt, scene};
    Module module {0, ""};
    subject.setModule(module);

    // when

    // then
}
