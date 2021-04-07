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

#include <boost/filesystem/path.hpp>
#include <boost/noncopyable.hpp>
#include <boost/program_options/options_description.hpp>

#include "game/options.h"

namespace reone {

/**
 * Encapsulates option management. Starts a Game instance.
 *
 * @see game::Game
 */
class Program : boost::noncopyable {
public:
    Program(int argc, char **argv);

    /**
     * Process command line arguments and start an appropriate Game instance.
     *
     * @return the exit code
     */
    int run();

private:
    int _argc;
    char **_argv;

    boost::program_options::options_description _optsCommon;
    boost::program_options::options_description _optsCmdLine { "Usage" };

    bool _showHelp { false };
    boost::filesystem::path _gamePath;
    game::Options _options;

    void initOptions();
    void loadOptions();
    int runGame();
};

} // namespace reone
