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

#include <boost/program_options.hpp>

#include "game/multiplayer/game.h"

using namespace std;

using namespace reone::game;
using namespace reone::net;
using namespace reone::resources;
using namespace reone::tools;

namespace fs = boost::filesystem;
namespace po = boost::program_options;

namespace reone {

static const char *kConfigFilename = "reone.cfg";
static const int kDefaultMultiplayerPort = 2003;

Program::Program(int argc, char **argv) : _argc(argc), _argv(argv) {
}

int Program::run() {
    loadOptions();

    if (_command == Command::Help) {
        cout << _cmdLineOpts << endl;
        return 0;
    }

    int code = 0;

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
            code = runGame();
            break;
    }

    return code;
}

void Program::loadOptions() {
    initOptions();

    po::positional_options_description positional;
    positional.add("input-file", 1);

    po::parsed_options parsedCmdLineOpts = po::command_line_parser(_argc, _argv)
        .options(_cmdLineOpts)
        .positional(positional)
        .run();

    po::store(parsedCmdLineOpts, _vars);
    if (fs::exists(kConfigFilename)) {
        po::store(po::parse_config_file(kConfigFilename, _commonOpts), _vars);
    }
    po::notify(_vars);

    _gamePath = _vars.count("game") ? _vars["game"].as<std::string>() : fs::current_path();

    if (_vars.count("key")) {
        _keyPath = _vars["key"].as<std::string>();
    } else {
        _keyPath = _gamePath;
        _keyPath.append("chitin.key");
    }

    _destPath = _vars.count("dest") ? _vars["dest"].as<std::string>() : fs::current_path();
    _module = _vars.count("module") ? _vars["module"].as<std::string>() : "";
    _inputFilePath = _vars.count("input-file") ? _vars["input-file"].as<std::string>() : "";
    _gameOpts.graphics.width = _vars["width"].as<int>();
    _gameOpts.graphics.height = _vars["height"].as<int>();
    _gameOpts.graphics.fullscreen = _vars["fullscreen"].as<bool>();
    _gameOpts.audio.volume = _vars["volume"].as<int>();
    _gameOpts.network.host = _vars.count("join") ? _vars["join"].as<std::string>() : "";
    _gameOpts.network.port = _vars["port"].as<int>();

    initGameVersion();
    initMultiplayerMode();
    initCommand();
    initTool();
}

void Program::initOptions() {
    _commonOpts.add_options()
        ("game", po::value<std::string>(), "path to game directory")
        ("key", po::value<std::string>(), "path to Key file")
        ("dest", po::value<std::string>(), "path to destination directory")
        ("module", po::value<std::string>(), "starting module name")
        ("width", po::value<int>()->default_value(800), "window width")
        ("height", po::value<int>()->default_value(600), "window height")
        ("fullscreen", po::value<bool>()->default_value(false), "enable fullscreen")
        ("volume", po::value<int>()->default_value(100), "audio volume in percents");

    _cmdLineOpts.add(_commonOpts).add_options()
        ("help", "print this message")
        ("list", "list file contents")
        ("extract", "extract file contents")
        ("convert", "convert 2DA or GFF file to JSON")
        ("input-file", po::value<std::string>(), "path to input file")
        ("serve", "start multiplayer game")
        ("join", po::value<std::string>()->implicit_value("127.0.0.1"), "join multiplayer game at specified IP address")
        ("port", po::value<int>()->default_value(kDefaultMultiplayerPort), "multiplayer port number");
}

void Program::initGameVersion() {
    fs::path path(_gamePath);
    path.append("swkotor2.exe");
    _version = fs::exists(path) ? GameVersion::TheSithLords : GameVersion::KotOR;
}

void Program::initMultiplayerMode() {
    if (_vars.count("serve")) {
        _multiplayer = MultiplayerMode::Server;
    } else if (_vars.count("join")) {
        _multiplayer = MultiplayerMode::Client;
    }
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
                throw std::runtime_error("Input file does not exist: " + _inputFilePath.string());
            }
            _tool = getToolByPath(_version, _inputFilePath);
            break;
        default:
            break;
    }
}

int Program::runGame() {
    std::unique_ptr<Game> game;

    switch (_multiplayer) {
        case MultiplayerMode::Client:
        case MultiplayerMode::Server:
            game.reset(new MultiplayerGame(_multiplayer, _version, _gamePath, _module, _gameOpts));
            break;

        default:
            game.reset(new Game(_version, _gamePath, _module, _gameOpts));
            break;
    }

    return game->run();
}

} // namespace reone
