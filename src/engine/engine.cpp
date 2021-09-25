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

#include "engine.h"

#include "common/logutil.h"
#include "common/pathutil.h"
#include "di/services/audio.h"
#include "di/services/common.h"
#include "di/services/graphics.h"
#include "di/services/resource.h"
#include "di/services/scene.h"
#include "di/services/script.h"
#include "game/kotor.h"
#include "game/tsl.h"

using namespace std;

using namespace reone::audio;
using namespace reone::di;
using namespace reone::game;
using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;
using namespace reone::script;

namespace fs = boost::filesystem;
namespace po = boost::program_options;

namespace reone {

static constexpr char kConfigFilename[] = "reone.cfg";

static constexpr int kDefaultShadowResolution = 2;
static constexpr int kDefaultMusicVolume = 85;
static constexpr int kDefaultVoiceVolume = 85;
static constexpr int kDefaultSoundVolume = 85;
static constexpr int kDefaultMovieVolume = 85;

int Engine::run() {
    initOptions();
    parseOptions();
    loadOptions();

    if (_showHelp) {
        cout << _optsCmdLine << endl;
        return 0;
    }

    return runGame();
}

void Engine::initOptions() {
    _optsCommon.add_options()
        ("game", po::value<string>(), "path to game directory")
        ("dev", po::value<bool>()->default_value(false), "enable developer mode")
        ("module", po::value<string>(), "name of a module to load")
        ("width", po::value<int>()->default_value(800), "window width")
        ("height", po::value<int>()->default_value(600), "window height")
        ("fullscreen", po::value<bool>()->default_value(false), "enable fullscreen")
        ("pbr", po::value<bool>()->default_value(false), "enable enhanced graphics mode")
        ("shadowres", po::value<int>()->default_value(kDefaultShadowResolution), "shadow map resolution")
        ("musicvol", po::value<int>()->default_value(kDefaultMusicVolume), "music volume in percents")
        ("voicevol", po::value<int>()->default_value(kDefaultVoiceVolume), "voice volume in percents")
        ("soundvol", po::value<int>()->default_value(kDefaultSoundVolume), "sound volume in percents")
        ("movievol", po::value<int>()->default_value(kDefaultMovieVolume), "movie volume in percents")
        ("logch", po::value<int>(), "log channel mask")
        ("logfile", po::value<bool>()->default_value(false), "log to file");

    _optsCmdLine.add(_optsCommon).add_options()
        ("help", "print this message");
}

void Engine::parseOptions() {
    po::store(po::parse_command_line(_argc, _argv, _optsCmdLine), _variables);

    if (fs::exists(kConfigFilename)) {
        po::store(po::parse_config_file<char>(kConfigFilename, _optsCommon, true), _variables);
    }

    po::notify(_variables);
}

void Engine::loadOptions() {
    _showHelp = _variables.count("help") > 0;
    _gamePath = _variables.count("game") > 0 ? _variables["game"].as<string>() : fs::current_path();
    _gameOptions.developer = _variables["dev"].as<bool>();
    _gameOptions.module = _variables.count("module") > 0 ? _variables["module"].as<string>() : "";
    _gameOptions.graphics.width = _variables["width"].as<int>();
    _gameOptions.graphics.height = _variables["height"].as<int>();
    _gameOptions.graphics.shadowResolution = _variables["shadowres"].as<int>();
    _gameOptions.graphics.fullscreen = _variables["fullscreen"].as<bool>();
    _gameOptions.graphics.pbr = _variables["pbr"].as<bool>();
    _gameOptions.audio.musicVolume = _variables["musicvol"].as<int>();
    _gameOptions.audio.voiceVolume = _variables["voicevol"].as<int>();
    _gameOptions.audio.soundVolume = _variables["soundvol"].as<int>();
    _gameOptions.audio.movieVolume = _variables["movievol"].as<int>();

    setLogToFile(_variables["logfile"].as<bool>());

    if (_variables.count("logch") > 0) {
        setLogChannels(_variables["logch"].as<int>());
    }
}

int Engine::runGame() {
    GameID gameId = determineGameID();

    CommonServices common;
    common.init();

    ResourceServices resource(_gamePath);
    resource.init();

    GraphicsServices graphics(_gameOptions.graphics, resource);
    graphics.init();

    AudioServices audio(_gameOptions.audio, resource);
    audio.init();

    SceneServices scene(_gameOptions.graphics, graphics);
    scene.init();

    ScriptServices script(resource);
    script.init();

    unique_ptr<Game> game(newGame(gameId, resource, graphics, audio, scene, script));

    return game->run();
}

GameID Engine::determineGameID() {
    // If there is a KotOR executable then game is KotOR
    fs::path exePathK1(getPathIgnoreCase(_gamePath, "swkotor.exe", false));
    if (!exePathK1.empty()) return GameID::KotOR;

    // If there is a TSL executable then game is TSL
    fs::path exePathK2(getPathIgnoreCase(_gamePath, "swkotor2.exe", false));
    if (!exePathK2.empty()) return GameID::TSL;

    throw logic_error("Unable to determine game ID: " + _gamePath.string());
}

unique_ptr<Game> Engine::newGame(
    GameID gameId,
    ResourceServices &resource,
    GraphicsServices &graphics,
    AudioServices &audio,
    SceneServices &scene,
    ScriptServices &script
) {
    switch (gameId) {
        case GameID::KotOR:
            return make_unique<KotOR>(_gamePath, _gameOptions, resource, graphics, audio, scene, script);
        case GameID::TSL:
            return make_unique<TSL>(_gamePath, _gameOptions, resource, graphics, audio, scene, script);
        default:
            throw logic_error("Unsupported game ID: " + to_string(static_cast<int>(gameId)));
    }
}

} // namespace reone
