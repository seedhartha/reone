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

#include "types.h"

namespace reone {

namespace tools {

class ITool;

}

class Program : boost::noncopyable {
public:
    Program(int argc, char **argv) :
        _argc(argc), _argv(argv) {
    }

    int run();

private:
    int _argc;
    char **_argv;

    boost::program_options::options_description _optsCmdLine {"Usage"};
    boost::program_options::variables_map _variables;

    boost::filesystem::path _gamePath;
    boost::filesystem::path _destPath;
    std::string _target;
    bool _tsl {false};
    tools::Operation _operation {tools::Operation::None};
    std::vector<std::shared_ptr<tools::ITool>> _tools;

    void initOptions();
    void parseOptions();
    void loadOptions();

    void loadTools();

    std::shared_ptr<tools::ITool> getTool() const;
};

} // namespace reone
