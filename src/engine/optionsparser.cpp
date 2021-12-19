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

#include "optionsparser.h"

#include "../common/types.h"

using namespace std;

using namespace reone::game;
using namespace reone::graphics;

namespace fs = boost::filesystem;
namespace po = boost::program_options;

namespace reone {

static constexpr char kConfigFilename[] = "reone.cfg";

Options OptionsParser::parse() {
    // Initialize options description

    po::options_description descCommon;
    descCommon.add_options()                                                                         //
        ("game", po::value<string>(), "path to game directory")                                      //
        ("dev", po::value<bool>()->default_value(false), "enable developer mode")                    //
        ("width", po::value<int>()->default_value(800), "window width")                              //
        ("height", po::value<int>()->default_value(600), "window height")                            //
        ("fullscreen", po::value<bool>()->default_value(false), "enable fullscreen")                 //
        ("texquality", po::value<int>()->default_value(0), "texture quality")                        //
        ("aasamples", po::value<int>()->default_value(0), "anti-aliasing samples")                   //
        ("shadowres", po::value<int>()->default_value(0), "shadow map resolution")                   //
        ("musicvol", po::value<int>()->default_value(85), "music volume in percents")                //
        ("voicevol", po::value<int>()->default_value(85), "voice volume in percents")                //
        ("soundvol", po::value<int>()->default_value(85), "sound volume in percents")                //
        ("movievol", po::value<int>()->default_value(85), "movie volume in percents")                //
        ("loglevel", po::value<int>()->default_value(static_cast<int>(LogLevel::Info)), "log level") //
        ("logch", po::value<int>()->default_value(LogChannels::general), "log channel mask")         //
        ("logfile", po::value<bool>()->default_value(false), "log to file");

    po::options_description descCmdLine {"Usage"};
    descCmdLine.add(descCommon);

    // Parse command line and configuration file

    po::variables_map vars;
    po::store(po::parse_command_line(_argc, _argv, descCmdLine), vars);
    if (fs::exists(kConfigFilename)) {
        po::store(po::parse_config_file<char>(kConfigFilename, descCommon, true), vars);
    }
    po::notify(vars);

    // Convert Boost options to game options

    Options options;
    options.gamePath = vars.count("game") > 0 ? vars["game"].as<string>() : fs::current_path();
    options.graphics.width = vars["width"].as<int>();
    options.graphics.height = vars["height"].as<int>();
    options.graphics.fullscreen = vars["fullscreen"].as<bool>();
    options.graphics.textureQuality = static_cast<TextureQuality>(vars["texquality"].as<int>());
    options.graphics.aaSamples = 1 << vars["aasamples"].as<int>();
    options.graphics.shadowResolution = 1 << (10 + vars["shadowres"].as<int>());
    options.audio.musicVolume = vars["musicvol"].as<int>();
    options.audio.voiceVolume = vars["voicevol"].as<int>();
    options.audio.soundVolume = vars["soundvol"].as<int>();
    options.audio.movieVolume = vars["movievol"].as<int>();
    options.developer = vars["dev"].as<bool>();
    options.logLevel = static_cast<LogLevel>(vars["loglevel"].as<int>());
    options.logChannels = vars["logch"].as<int>();
    options.logToFile = vars["logfile"].as<bool>();

    return move(options);
}

} // namespace reone
