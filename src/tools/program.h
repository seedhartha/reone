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

#include <boost/program_options/options_description.hpp>

#include "../engine/resource/types.h"

#include "tools.h"
#include "types.h"

namespace reone {

namespace tools {

class Program : boost::noncopyable {
public:
    Program(int argc, char **argv);

    int run();

private:
    int _argc;
    char **_argv;

    boost::program_options::options_description _optsCmdLine { "Usage" };

    boost::filesystem::path _gamePath;
    boost::filesystem::path _destPath;
    std::string _target;
    Operation _operation { Operation::None };
    std::vector<std::shared_ptr<ITool>> _tools;

    void initOptions();
    void loadOptions();
    void loadTools();

    std::shared_ptr<ITool> getTool() const;
};

} // namespace tools

} // namespace reone
