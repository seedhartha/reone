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

#include "program.h"

#include <iostream>

#include <boost/filesystem/operations.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>

#include "src/core/pathutil.h"

using namespace std;

using namespace reone::resources;

namespace fs = boost::filesystem;
namespace po = boost::program_options;

namespace reone {

namespace tools {

Program::Program(int argc, char **argv) : _argc(argc), _argv(argv) {
}

int Program::run() {
    loadOptions();

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

void Program::loadOptions() {
    _cmdLineOpts.add_options()
        ("help", "print this message")
        ("list", "list file contents")
        ("extract", "extract file contents")
        ("convert", "convert 2DA or GFF file to JSON")
        ("game", po::value<std::string>(), "path to game directory")
        ("dest", po::value<std::string>(), "path to destination directory")
        ("input-file", po::value<std::string>(), "path to input file");

    po::positional_options_description positional;
    positional.add("input-file", 1);

    po::parsed_options parsedCmdLineOpts = po::command_line_parser(_argc, _argv)
        .options(_cmdLineOpts)
        .positional(positional)
        .run();

    po::store(parsedCmdLineOpts, _vars);
    po::notify(_vars);

    _gamePath = _vars.count("game") ? _vars["game"].as<std::string>() : fs::current_path();
    _destPath = _vars.count("dest") ? _vars["dest"].as<std::string>() : fs::current_path();
    _inputFilePath = _vars.count("input-file") ? _vars["input-file"].as<std::string>() : "";

    initKeyPath();
    initGameVersion();
    initCommand();
    initTool();
}

void Program::initKeyPath() {
    _keyPath = getPathIgnoreCase(_gamePath, "chitin.key");
}

void Program::initGameVersion() {
    fs::path exePath = getPathIgnoreCase(_gamePath, "swkotor2.exe");
    _version = exePath.empty() ? GameVersion::KotOR : GameVersion::TheSithLords;
}

void Program::initCommand() {
    if (_vars.count("help")) {
        _command = Command::Help;
    } else if (_vars.count("list")) {
        _command = Command::List;
    } else if (_vars.count("extract")) {
        _command = Command::Extract;
    } else if (_vars.count("convert")) {
        _command = Command::Convert;
    }
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
