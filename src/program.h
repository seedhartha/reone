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

#include <memory>
#include <string>

#include <boost/filesystem/path.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>

#include "game/types.h"
#include "net/types.h"
#include "resources/types.h"
#include "tools/tools.h"

namespace reone {

/**
 * Encapsulates program-level logic (i.e., option management).
 */
class Program {
public:
    Program(int argc, char **argv);

    int run();

private:
    enum class Command {
        None,
        Help,
        List,
        Extract,
        Convert
    };

    int _argc { 0 };
    char **_argv { nullptr };
    Command _command { Command::None };
    boost::program_options::options_description _commonOpts;
    boost::program_options::options_description _cmdLineOpts { "Usage" };
    boost::program_options::variables_map _vars;
    resources::GameVersion _version { resources::GameVersion::KotOR };
    game::MultiplayerMode _multiplayer { game::MultiplayerMode::None };
    std::unique_ptr<tools::Tool> _tool;

    // Parsed options
    game::Options _gameOpts;
    boost::filesystem::path _gamePath;
    std::string _module;
    boost::filesystem::path _keyPath;
    boost::filesystem::path _inputFilePath;
    boost::filesystem::path _destPath;

    Program(const Program &) = delete;
    Program &operator=(const Program &) = delete;

    void loadOptions();
    void initOptions();
    void initGameVersion();
    void initMultiplayerMode();
    void initCommand();
    void initTool();
    int runGame();
};

} // namespace reone
