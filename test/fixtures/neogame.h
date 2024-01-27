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

#pragma once

#include <gmock/gmock.h>

#include "reone/game/console.h"
#include "reone/game/neo/object/area.h"
#include "reone/game/neo/object/camera.h"
#include "reone/game/neo/object/creature.h"
#include "reone/game/neo/object/door.h"
#include "reone/game/neo/object/encounter.h"
#include "reone/game/neo/object/module.h"
#include "reone/game/neo/object/placeable.h"
#include "reone/game/neo/object/sound.h"
#include "reone/game/neo/object/store.h"
#include "reone/game/neo/object/trigger.h"
#include "reone/game/neo/object/waypoint.h"
#include "reone/game/profiler.h"

using namespace reone::resource;

namespace reone {

namespace game {

namespace neo {

class MockGame : public IAreaLoader,
                 public IAreaObjectLoader,
                 public IActionExecutor,
                 public IEventCollector {
public:
    MOCK_METHOD(Area &, loadArea, (const std::string &), (override));

    MOCK_METHOD(Camera &, loadCamera, (), (override));
    MOCK_METHOD(Creature &, loadCreature, (const ResRef &), (override));
    MOCK_METHOD(Door &, loadDoor, (const ResRef &), (override));
    MOCK_METHOD(Encounter &, loadEncounter, (const ResRef &), (override));
    MOCK_METHOD(Placeable &, loadPlaceable, (const ResRef &), (override));
    MOCK_METHOD(Sound &, loadSound, (const ResRef &), (override));
    MOCK_METHOD(Store &, loadStore, (const ResRef &), (override));
    MOCK_METHOD(Trigger &, loadTrigger, (const ResRef &), (override));
    MOCK_METHOD(Waypoint &, loadWaypoint, (const ResRef &), (override));

    MOCK_METHOD(bool, executeAction, (Object &, const Action &, float), (override));

    MOCK_METHOD(void, collectEvent, (Event), (override));
};

class MockConsole : public IConsole, boost::noncopyable {
public:
    MOCK_METHOD(void, registerCommand, (std::string, std::string, IConsole::CommandHandler), (override));
    MOCK_METHOD(void, printLine, (const std::string &), (override));
};

class MockProfiler : public IProfiler, boost::noncopyable {
public:
    MOCK_METHOD(void, reserveThread, (std::string, std::vector<glm::vec3>), (override));
    MOCK_METHOD(void, measure, (const std::string &, int, const std::function<void()> &), (override));
};

} // namespace neo

} // namespace game

} // namespace reone
