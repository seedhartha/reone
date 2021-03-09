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

#include <iostream>
#include <stdexcept>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include "common/log.h"
#include "common/types.h"
#include "game/game.h"

using namespace std;

using namespace reone::game;

namespace fs = boost::filesystem;
namespace po = boost::program_options;

namespace reone {

static const char kConfigFilename[] = "reone.cfg";

static constexpr int kDefaultNumLights = 8;
static constexpr int kDefaultShadowResolution = 2;
static constexpr int kDefaultMusicVolume = 85;
static constexpr int kDefaultVoiceVolume = 85;
static constexpr int kDefaultSoundVolume = 85;
static constexpr int kDefaultMovieVolume = 85;

Program::Program(int argc, char **argv) : _argc(argc), _argv(argv) {
    if (!argv) {
        throw invalid_argument("argv must not be null");
    }
}

int Program::run() {
    initOptions();
    loadOptions();

    if (_showHelp) {
        cout << _cmdLineOpts << endl;
        return 0;
    }

    return runGame();
}

void Program::initOptions() {
    _commonOpts.add_options()
        ("game", po::value<string>(), "path to game directory")
        ("dev", po::value<bool>()->default_value(false), "enable developer mode")
        ("module", po::value<string>(), "name of a module to load")
        ("width", po::value<int>()->default_value(800), "window width")
        ("height", po::value<int>()->default_value(600), "window height")
        ("fullscreen", po::value<bool>()->default_value(false), "enable fullscreen")
        ("numlights", po::value<int>()->default_value(kDefaultNumLights), "maximum number of lights")
        ("shadowres", po::value<int>()->default_value(kDefaultShadowResolution), "shadow map resolution")
        ("musicvol", po::value<int>()->default_value(kDefaultMusicVolume), "music volume in percents")
        ("voicevol", po::value<int>()->default_value(kDefaultVoiceVolume), "voice volume in percents")
        ("soundvol", po::value<int>()->default_value(kDefaultSoundVolume), "sound volume in percents")
        ("movievol", po::value<int>()->default_value(kDefaultMovieVolume), "movie volume in percents")
        ("debug", po::value<int>()->default_value(0), "debug log level (0-3)")
        ("debugch", po::value<int>()->default_value(DebugChannels::all), "debug channel mask")
        ("logfile", po::value<bool>()->default_value(false), "log to file");

    _cmdLineOpts.add(_commonOpts).add_options()
        ("help", "print this message");
}

void Program::loadOptions() {
    po::parsed_options parsedCmdLineOpts = po::command_line_parser(_argc, _argv)
        .options(_cmdLineOpts)
        .run();

    po::variables_map vars;
    po::store(parsedCmdLineOpts, vars);
    if (fs::exists(kConfigFilename)) {
        po::store(po::parse_config_file<char>(kConfigFilename, _commonOpts), vars);
    }
    po::notify(vars);

    _showHelp = vars.count("help") > 0;
    _gamePath = vars.count("game") > 0 ? vars["game"].as<string>() : fs::current_path();
    _gameOpts.developer = vars["dev"].as<bool>();
    _gameOpts.module = vars.count("module") > 0 ? vars["module"].as<string>() : "";
    _gameOpts.graphics.width = vars["width"].as<int>();
    _gameOpts.graphics.height = vars["height"].as<int>();
    _gameOpts.graphics.fullscreen = vars["fullscreen"].as<bool>();
    _gameOpts.graphics.numLights = vars["numlights"].as<int>();
    _gameOpts.graphics.shadowResolution = vars["shadowres"].as<int>();
    _gameOpts.audio.musicVolume = vars["musicvol"].as<int>();
    _gameOpts.audio.voiceVolume = vars["voicevol"].as<int>();
    _gameOpts.audio.soundVolume = vars["soundvol"].as<int>();
    _gameOpts.audio.movieVolume = vars["movievol"].as<int>();

    setDebugLogLevel(vars["debug"].as<int>());
    setDebugChannels(vars["debugch"].as<int>());
    setLogToFile(vars["logfile"].as<bool>());
}

int Program::runGame() {
    return Game(_gamePath, _gameOpts).run();
}

} // namespace reone
