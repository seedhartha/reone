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
#include "../game/neo/game.h"

#include "di/services.h"
#include "gameprobe.h"
#include "optionsparser.h"

using namespace std;

using namespace reone::engine;
using namespace reone::game;

namespace reone {

namespace engine {

static unique_ptr<Game> newGame(GameID gameId, Options options, ServicesView &services) {
    switch (gameId) {
    case GameID::KotOR:
    case GameID::TSL:
        return make_unique<Game>(gameId == GameID::TSL, options.gamePath, options, services);
    default:
        throw logic_error("Unsupported game ID: " + to_string(static_cast<int>(gameId)));
    }
}

int Engine::run() {
    OptionsParser optionsParser(_argc, _argv);
    Options gameOptions(optionsParser.parse());

    setLogLevel(gameOptions.logLevel);
    setLogToFile(gameOptions.logToFile);
    setLogChannels(gameOptions.logChannels);

    GameProbe gameProbe(gameOptions.gamePath);
    GameID gameId = gameProbe.probe();

    Services services(gameId, gameOptions);
    services.init();

    if (gameOptions.neo) {
        auto game = neo::Game(GameID::KotOR, gameOptions, services.view());
        game.init();
        game.run();
        return 0;
    } else {
        auto game = newGame(gameId, gameOptions, services.view());
        game->init();
        services.view().graphics.window.setEventHandler(game.get());
        return game->run();
    }
}

} // namespace engine

} // namespace reone
