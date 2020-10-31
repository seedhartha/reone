/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include <boost/filesystem/path.hpp>
#include <boost/program_options/options_description.hpp>

#include "game/types.h"
#include "mp/types.h"
#include "net/types.h"
#include "resource/types.h"

namespace reone {

/**
 * Encapsulates the program-level logic (i.e., option management). Starts a
 * `Game` instance.
 *
 * @see game::Game
 */
class Program {
public:
    Program(int argc, char **argv);

    int run();

private:
    bool _showHelp { false };
    boost::filesystem::path _gamePath;
    game::Options _gameOpts;
    mp::MultiplayerMode _multiplayerMode { mp::MultiplayerMode::None };

    // Command line arguments

    int _argc { 0 };
    char **_argv { nullptr };

    // END Command line arguments

    // Intermediate options

    boost::program_options::options_description _commonOpts;
    boost::program_options::options_description _cmdLineOpts { "Usage" };

    // END Intermediate options

    Program(const Program &) = delete;
    Program &operator=(const Program &) = delete;

    void initOptions();
    void loadOptions();
    int runGame();
};

} // namespace reone
