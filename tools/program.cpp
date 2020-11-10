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

#include "program.h"

#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include "../src/common/pathutil.h"

using namespace std;

using namespace reone::resource;

namespace fs = boost::filesystem;
namespace po = boost::program_options;

namespace reone {

namespace tools {

Program::Program(int argc, char **argv) : _argc(argc), _argv(argv) {
}

int Program::run() {
    initOptions();
    loadOptions();
    initGameVersion();
    initTool();

    switch (_command) {
        case Command::List:
            _tool->list(_inputFilePath, _keyPath);
            break;
        case Command::Extract:
            _tool->extract(_inputFilePath, _keyPath, _destPath);
            break;
        case Command::Convert:
            _tool->convert(_inputFilePath, _destPath);
            break;
        default:
            cout << _cmdLineOpts << endl;
            break;
    }

    return 0;
}

void Program::initOptions() {
    _cmdLineOpts.add_options()
        ("help", "print this message")
        ("list", "list file contents")
        ("extract", "extract file contents")
        ("convert", "convert 2DA or GFF file to JSON")
        ("game", po::value<string>(), "path to game directory")
        ("dest", po::value<string>(), "path to destination directory")
        ("input-file", po::value<string>(), "path to input file");
}

void Program::loadOptions() {
    po::positional_options_description positional;
    positional.add("input-file", 1);

    po::parsed_options parsedCmdLineOpts = po::command_line_parser(_argc, _argv)
        .options(_cmdLineOpts)
        .positional(positional)
        .run();

    po::variables_map vars;
    po::store(parsedCmdLineOpts, vars);
    po::notify(vars);

    _gamePath = vars.count("game") > 0 ? vars["game"].as<string>() : fs::current_path();
    _destPath = vars.count("dest") > 0 ? vars["dest"].as<string>() : fs::current_path();
    _inputFilePath = vars.count("input-file") > 0 ? vars["input-file"].as<string>() : "";
    _keyPath = getPathIgnoreCase(_gamePath, "chitin.key");

    if (vars.count("help")) {
        _command = Command::Help;
    } else if (vars.count("list")) {
        _command = Command::List;
    } else if (vars.count("extract")) {
        _command = Command::Extract;
    } else if (vars.count("convert")) {
        _command = Command::Convert;
    }
}

void Program::initGameVersion() {
    fs::path exePath = getPathIgnoreCase(_gamePath, "swkotor2.exe");
    _version = exePath.empty() ? GameVersion::KotOR : GameVersion::TheSithLords;
}

void Program::initTool() {
    switch (_command) {
        case Command::List:
        case Command::Extract:
        case Command::Convert:
            if (!fs::exists(_inputFilePath)) {
                throw runtime_error("Input file does not exist: " + _inputFilePath.string());
            }
            _tool = getToolByPath(_version, _inputFilePath);
            break;
        default:
            break;
    }
}

} // namespace tools

} // namespace reone
