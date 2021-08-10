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

#include <boost/program_options.hpp>

#include "common/guardutil.h"
#include "common/log.h"
#include "common/types.h"
#include "engine.h"
#include "game/options.h"

using namespace std;

using namespace reone::game;
using namespace reone::graphics;

namespace fs = boost::filesystem;
namespace po = boost::program_options;

namespace reone {

static constexpr char kConfigFilename[] = "reone.cfg";

static constexpr int kDefaultShadowResolution = 2;
static constexpr int kDefaultMusicVolume = 85;
static constexpr int kDefaultVoiceVolume = 85;
static constexpr int kDefaultSoundVolume = 85;
static constexpr int kDefaultMovieVolume = 85;

/**
 * Encapsulates option management.
 */
class Program : boost::noncopyable {
public:
    Program(int argc, char **argv) : _argc(argc), _argv(argv) {
        ensureNotNull(argv, "argv");
    }

    /**
     * Loads options from command line and configuration file, and starts an instance of Engine.
     *
     * @return exit code
     */
    int run() {
        initOptions();
        loadOptions();

        if (_showHelp) {
            cout << _optsCmdLine << endl;
            return 0;
        }

        return runEngine(_gamePath, _options);
    }

private:
    int _argc;
    char **_argv;

    boost::program_options::options_description _optsCommon;
    boost::program_options::options_description _optsCmdLine { "Usage" };

    bool _showHelp { false };
    boost::filesystem::path _gamePath;
    game::Options _options;

    void initOptions() {
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
            ("debug", po::value<int>()->default_value(0), "debug log level (0-3)")
            ("debugch", po::value<int>(), "debug channel mask")
            ("logfile", po::value<bool>()->default_value(false), "log to file");

        _optsCmdLine.add(_optsCommon).add_options()
            ("help", "print this message");
    }

    void loadOptions() {
        po::parsed_options parsedCmdLineOpts = po::command_line_parser(_argc, _argv)
            .options(_optsCmdLine)
            .run();

        po::variables_map vars;
        po::store(parsedCmdLineOpts, vars);
        if (fs::exists(kConfigFilename)) {
            po::store(po::parse_config_file<char>(kConfigFilename, _optsCommon), vars);
        }
        po::notify(vars);

        _showHelp = vars.count("help") > 0;
        _gamePath = vars.count("game") > 0 ? vars["game"].as<string>() : fs::current_path();
        _options.developer = vars["dev"].as<bool>();
        _options.module = vars.count("module") > 0 ? vars["module"].as<string>() : "";
        _options.graphics.width = vars["width"].as<int>();
        _options.graphics.height = vars["height"].as<int>();
        _options.graphics.shadowResolution = vars["shadowres"].as<int>();
        _options.graphics.fullscreen = vars["fullscreen"].as<bool>();
        _options.graphics.pbr = vars["pbr"].as<bool>();
        _options.audio.musicVolume = vars["musicvol"].as<int>();
        _options.audio.voiceVolume = vars["voicevol"].as<int>();
        _options.audio.soundVolume = vars["soundvol"].as<int>();
        _options.audio.movieVolume = vars["movievol"].as<int>();

        setDebugLogLevel(vars["debug"].as<int>());
        setLogToFile(vars["logfile"].as<bool>());

        if (vars.count("debugch") > 0) {
            setDebugChannels(vars["debugch"].as<int>());
        }
    }
};

int runProgram(int argc, char **argv) {
    return Program(argc, argv).run();
}

} // namespace reone
