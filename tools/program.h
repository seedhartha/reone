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

#include <string>
#include <memory>

#include <boost/filesystem/path.hpp>
#include <boost/program_options/options_description.hpp>

#include "tools.h"

namespace reone {

namespace tools {

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
        ToJson,
        ToTga,
        ToTwoDa,
        ToGFF,
        ModuleProbe
    };

    boost::filesystem::path _gamePath;
    boost::filesystem::path _destPath;
    std::string _target;
    boost::filesystem::path _keyPath;
    resource::GameID _gameId { resource::GameID::KotOR };
    Command _command { Command::None };
    std::unique_ptr<FileTool> _tool;

    // Command line arguments

    int _argc { 0 };
    char **_argv { nullptr };

    // END Command line arguments

    boost::program_options::options_description _commonOpts;
    boost::program_options::options_description _cmdLineOpts { "Usage" };

    Program(const Program &) = delete;
    Program &operator=(const Program &) = delete;

    void initOptions();
    void loadOptions();
    void determineGameID();
    void initFileTool();
};

} // namespace tools

} // namespace reone
