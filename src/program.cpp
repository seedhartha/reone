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

#include "core/debug.h"
#include "core/pathutil.h"
#include "game/multiplayer/game.h"

using namespace std;

using namespace reone::game;
using namespace reone::net;
using namespace reone::resources;

namespace fs = boost::filesystem;
namespace po = boost::program_options;

namespace reone {

static const char *kConfigFilename = "reone.cfg";
static const int kDefaultMusicVolume = 85;
static const int kDefaultSoundVolume = 85;
static const int kDefaultMultiplayerPort = 2003;

Program::Program(int argc, char **argv) : _argc(argc), _argv(argv) {
}

int Program::run() {
    loadOptions();

    if (_help) {
        cout << _cmdLineOpts << endl;
        return 0;
    }

    return runGame();
}

void Program::loadOptions() {
    _commonOpts.add_options()
        ("game", po::value<string>(), "path to game directory")
        ("width", po::value<int>()->default_value(800), "window width")
        ("height", po::value<int>()->default_value(600), "window height")
        ("fullscreen", po::value<bool>()->default_value(false), "enable fullscreen")
        ("musicvol", po::value<int>()->default_value(kDefaultMusicVolume), "music volume in percents")
        ("soundvol", po::value<int>()->default_value(kDefaultSoundVolume), "sound volume in percents")
        ("port", po::value<int>()->default_value(kDefaultMultiplayerPort), "multiplayer port number")
        ("debug", po::value<int>()->default_value(0), "debug level (0-3)");

    _cmdLineOpts.add(_commonOpts).add_options()
        ("help", "print this message")
        ("serve", "start multiplayer game")
        ("join", po::value<string>()->implicit_value("127.0.0.1"), "join multiplayer game at specified IP address");

    po::parsed_options parsedCmdLineOpts = po::command_line_parser(_argc, _argv)
        .options(_cmdLineOpts)
        .run();

    po::store(parsedCmdLineOpts, _vars);

    if (fs::exists(kConfigFilename)) {
        po::store(po::parse_config_file(kConfigFilename, _commonOpts), _vars);
    }
    po::notify(_vars);

    _help = _vars.count("help") > 0;
    _gamePath = _vars.count("game") ? _vars["game"].as<string>() : fs::current_path();
    _gameOpts.graphics.width = _vars["width"].as<int>();
    _gameOpts.graphics.height = _vars["height"].as<int>();
    _gameOpts.graphics.fullscreen = _vars["fullscreen"].as<bool>();
    _gameOpts.audio.musicVolume = _vars["musicvol"].as<int>();
    _gameOpts.audio.soundVolume = _vars["soundvol"].as<int>();
    _gameOpts.network.host = _vars.count("join") ? _vars["join"].as<string>() : "";
    _gameOpts.network.port = _vars["port"].as<int>();
    _gameOpts.debug = _vars["debug"].as<int>();

    setDebugLevel(_gameOpts.debug);
    initGameVersion();
    initMultiplayerMode();
}

void Program::initGameVersion() {
    fs::path exePath = getPathIgnoreCase(_gamePath, "swkotor2.exe");
    _gameVersion = exePath.empty() ? GameVersion::KotOR : GameVersion::TheSithLords;
}

void Program::initMultiplayerMode() {
    if (_vars.count("serve") > 0) {
        _multiplayerMode = MultiplayerMode::Server;
    } else if (_vars.count("join") > 0) {
        _multiplayerMode = MultiplayerMode::Client;
    }
}

int Program::runGame() {
    unique_ptr<Game> game;
    switch (_multiplayerMode) {
        case MultiplayerMode::Client:
        case MultiplayerMode::Server:
            game.reset(new MultiplayerGame(_multiplayerMode, _gameVersion, _gamePath, _gameOpts));
            break;
        default:
            game.reset(new Game(_gameVersion, _gamePath, _gameOpts));
            break;
    }

    return game->run();
}

} // namespace reone
