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
        Convert
    };

    boost::filesystem::path _gamePath;
    boost::filesystem::path _destPath;
    boost::filesystem::path _inputFilePath;
    boost::filesystem::path _keyPath;
    resources::GameVersion _version { resources::GameVersion::KotOR };
    Command _command { Command::None };
    std::unique_ptr<Tool> _tool;

    // Command line arguments

    int _argc { 0 };
    char **_argv { nullptr };

    // END Command line arguments

    boost::program_options::options_description _cmdLineOpts { "Usage" };

    Program(const Program &) = delete;
    Program &operator=(const Program &) = delete;

    void initOptions();
    void loadOptions();
    void initGameVersion();
    void initTool();
};

} // namespace tools

} // namespace reone
