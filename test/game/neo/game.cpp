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

class GameFixture : public Test {
protected:
    void SetUp() override {
        _graphicsModule.init();
        _resourceModule.init();
        _sceneModule.init();
        _game = std::make_unique<Game>(
            _options,
            _systemModule.services(),
            _graphicsModule.services(),
            _resourceModule.services(),
            _sceneModule.services(),
            _console,
            _profiler);
    }

    TestOptions _options;
    TestSystemModule _systemModule;
    TestGraphicsModule _graphicsModule;
    TestResourceModule _resourceModule;
    TestSceneModule _sceneModule;
    MockConsole _console;
    MockProfiler _profiler;

    std::unique_ptr<Game> _game;
};

TEST_F(GameFixture, should_do_something) {
}
