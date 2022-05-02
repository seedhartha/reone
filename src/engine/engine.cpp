/*
 * Copyright (c) 2020-2021 The reone project contributors
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

#include "engine.h"

#include "../common/logutil.h"
#include "../game/game.h"

#include "di/services.h"
#include "gameprobe.h"
#include "optionsparser.h"

using namespace std;

using namespace reone::engine;
using namespace reone::game;

namespace reone {

namespace engine {

int Engine::run() {
    auto optionsParser = OptionsParser(_argc, _argv);
    auto options = optionsParser.parse();

    setLogLevel(options.logging.level);
    setLogChannels(options.logging.channels);
    setLogToFile(options.logging.logToFile);

    auto gameProbe = GameProbe(options.game.path);
    auto gameId = gameProbe.probe();

    auto optionsView = options.toView();

    auto services = Services(gameId, optionsView);
    services.init();

    auto game = Game(GameID::KotOR, optionsView, services.view());
    game.init();
    game.run();

    return 0;
}

} // namespace engine

} // namespace reone
