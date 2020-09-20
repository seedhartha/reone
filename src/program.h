/*
 * Copyright © 2020 Vsevolod Kremianskii
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
#include <boost/program_options/variables_map.hpp>

#include "game/types.h"
#include "net/types.h"
#include "resources/types.h"

namespace reone {

/**
 * Encapsulates program-level logic (i.e., option management).
 */
class Program {
public:
    Program(int argc, char **argv);

    int run();

private:
    int _argc { 0 };
    char **_argv { nullptr };
    boost::program_options::options_description _commonOpts;
    boost::program_options::options_description _cmdLineOpts { "Usage" };
    boost::program_options::variables_map _vars;
    bool _help { false };
    boost::filesystem::path _gamePath;
    game::Options _gameOpts;
    resources::GameVersion _gameVersion { resources::GameVersion::KotOR };
    game::MultiplayerMode _multiplayerMode { game::MultiplayerMode::None };

    Program(const Program &) = delete;
    Program &operator=(const Program &) = delete;

    void loadOptions();
    void initGameVersion();
    void initMultiplayerMode();
    int runGame();
};

} // namespace reone
