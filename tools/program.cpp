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

#include "program.h"

#include <algorithm>
#include <iostream>
#include <unordered_map>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include "../src/common/pathutil.h"
#include "../src/resource/gameidutil.h"

#include "tools.h"
#include "types.h"

using namespace std;

using namespace reone::resource;

namespace fs = boost::filesystem;
namespace po = boost::program_options;

namespace reone {

namespace tools {

static const char kConfigFilename[] = "reone-tools.cfg";

static const unordered_map<string, Operation> g_operations {
    { "list", Operation::List },
    { "extract", Operation::Extract },
    { "to-json", Operation::ToJSON },
    { "to-tga", Operation::ToTGA },
    { "to-2da", Operation::To2DA },
    { "to-gff", Operation::ToGFF },
    { "to-rim", Operation::ToRIM },
};

Program::Program(int argc, char **argv) : _argc(argc), _argv(argv) {
}

int Program::run() {
    initOptions();
    loadOptions();

    _gameId = determineGameID(_gamePath);

    loadTools();

    switch (_operation) {
        case Operation::None:
            cout << _cmdLineOpts << endl;
            break;
        default: {
            auto tool = getTool();
            if (tool) {
                tool->invoke(_operation, _target, _gamePath, _destPath);
            } else {
                cout << "Unable to choose a tool for the specified operation" << endl;
            }
            break;
        }
    }

    return 0;
}

void Program::initOptions() {
    _commonOpts.add_options()
        ("game", po::value<string>(), "path to game directory")
        ("dest", po::value<string>(), "path to destination directory");

    _cmdLineOpts.add(_commonOpts).add_options()
        ("list", "list file contents")
        ("extract", "extract file contents")
        ("to-json", "convert 2DA, GFF or TLK file to JSON")
        ("to-tga", "convert TPC image to TGA")
        ("to-2da", "convert JSON to 2DA")
        ("to-gff", "convert JSON to GFF")
        ("to-rim", "create RIM archive from directory")
        ("target", po::value<string>(), "target name or path to input file");
}

static fs::path getDestination(const po::variables_map &vars) {
    fs::path result;
    if (vars.count("dest") > 0) {
        result = vars["dest"].as<string>();
    } else if (vars.count("target") > 0) {
        result = fs::path(vars["target"].as<string>()).parent_path();
    } else {
        result = fs::current_path();
    }
    return move(result);
}

void Program::loadOptions() {
    po::positional_options_description positional;
    positional.add("target", 1);

    po::parsed_options parsedCmdLineOpts = po::command_line_parser(_argc, _argv)
        .options(_cmdLineOpts)
        .positional(positional)
        .run();

    po::variables_map vars;
    po::store(parsedCmdLineOpts, vars);
    if (fs::exists(kConfigFilename)) {
        po::store(po::parse_config_file<char>(kConfigFilename, _commonOpts), vars);
    }
    po::notify(vars);

    _gamePath = vars.count("game") > 0 ? vars["game"].as<string>() : fs::current_path();
    _destPath = getDestination(vars);
    _target = vars.count("target") > 0 ? vars["target"].as<string>() : "";

    // Determine operation from program options
    for (auto &operation : g_operations) {
        if (vars.count(operation.first)) {
            _operation = operation.second;
            break;
        }
    }
}

void Program::loadTools() {
    // Tools are queried in the order of addition, whether they support a
    // particular operation on a particular file, or not. The first tool
    // to return true gets chosen.

    _tools.push_back(make_shared<KeyBifTool>());
    _tools.push_back(make_shared<ErfTool>());
    _tools.push_back(make_shared<RimTool>());
    _tools.push_back(make_shared<TwoDaTool>());
    _tools.push_back(make_shared<TlkTool>());
    _tools.push_back(make_shared<GffTool>());
    _tools.push_back(make_shared<TpcTool>());
}

shared_ptr<ITool> Program::getTool() const {
    for (auto &tool : _tools) {
        if (tool->supports(_operation, _target)) return tool;
    }
    return nullptr;
}

} // namespace tools

} // namespace reone
