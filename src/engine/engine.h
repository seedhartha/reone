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

#pragma once

#include <boost/program_options.hpp>

#include "game/options.h"
#include "game/types.h"

namespace reone {

namespace game {

class Game;

}

namespace di {

class AudioServices;
class GraphicsServices;
class ResourceServices;
class SceneServices;
class ScriptServices;

}

/**
 * Encapsulates option loading and service initialization.
 */
class Engine : boost::noncopyable {
public:
    Engine(int argc, char **argv) : _argc(argc), _argv(argv) {
    }

    /**
     * Loads options from command line and configuration file, initializes
     * services and starts an instance of Game.
     *
     * @return exit code
     */
    int run();

private:
    int _argc;
    char **_argv;

    boost::program_options::options_description _optsCommon;
    boost::program_options::options_description _optsCmdLine { "Usage" };
    boost::program_options::variables_map _variables;

    bool _showHelp { false };
    boost::filesystem::path _gamePath;
    game::Options _gameOptions;

    void initOptions();
    void parseOptions();
    void loadOptions();

    int runGame();

    game::GameID determineGameID();
};

} // namespace reone
