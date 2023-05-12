/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "reone/common/types.h"

using namespace std;

using namespace boost::program_options;

using namespace reone::engine;
using namespace reone::game;
using namespace reone::graphics;

namespace reone {

namespace engine {

static constexpr char kConfigFilename[] = "reone.cfg";

Options OptionsParser::parse() {
    Options options;

    // Initialize options description

    options_description descCommon;
    descCommon.add_options()                                                                                                   //
        ("game", value<string>(), "path to game directory")                                                                    //
        ("dev", value<bool>()->default_value(options.game.developer), "enable developer mode")                                 //
        ("neo", value<bool>()->default_value(false), "use new game logic (experimental)")                                      //
        ("width", value<int>()->default_value(options.graphics.width), "window width")                                         //
        ("height", value<int>()->default_value(options.graphics.height), "window height")                                      //
        ("fullscreen", value<bool>()->default_value(options.graphics.fullscreen), "enable fullscreen")                         //
        ("vsync", value<bool>()->default_value(options.graphics.vsync), "enable v-sync")                                       //
        ("grass", value<bool>()->default_value(options.graphics.grass), "enable grass")                                        //
        ("ssao", value<bool>()->default_value(options.graphics.ssao), "enable screen-space ambient occlusion")                 //
        ("ssr", value<bool>()->default_value(options.graphics.ssr), "enable screen-space reflections")                         //
        ("fxaa", value<bool>()->default_value(options.graphics.fxaa), "enable anti-aliasing")                                  //
        ("sharpen", value<bool>()->default_value(options.graphics.sharpen), "enable image sharpening")                         //
        ("texquality", value<int>()->default_value(static_cast<int>(options.graphics.textureQuality)), "texture quality")      //
        ("shadowres", value<int>()->default_value(glm::log2(options.graphics.shadowResolution) - 10), "shadow map resolution") //
        ("anisofilter", value<int>()->default_value(options.graphics.anisotropicFiltering), "anisotropic filtering")           //
        ("drawdist", value<int>()->default_value(static_cast<int>(kDefaultObjectDrawDistance)), "draw distance")               //
        ("musicvol", value<int>()->default_value(options.audio.musicVolume), "music volume in percents")                       //
        ("voicevol", value<int>()->default_value(options.audio.voiceVolume), "voice volume in percents")                       //
        ("soundvol", value<int>()->default_value(options.audio.soundVolume), "sound volume in percents")                       //
        ("movievol", value<int>()->default_value(options.audio.movieVolume), "movie volume in percents")                       //
        ("loglevel", value<int>()->default_value(static_cast<int>(options.logging.level)), "log level")                        //
        ("logch", value<int>()->default_value(options.logging.channels), "log channel mask")                                   //
        ("logfile", value<bool>()->default_value(options.logging.logToFile), "log to file");                                   //

    options_description descCmdLine {"Usage"};
    descCmdLine.add(descCommon);

    // Parse command line and configuration file

    variables_map vars;
    store(parse_command_line(_argc, _argv, descCmdLine), vars);
    if (boost::filesystem::exists(kConfigFilename)) {
        store(parse_config_file<char>(kConfigFilename, descCommon, true), vars);
    }
    notify(vars);

    // Convert Boost options to game options

    options.game.path = vars.count("game") > 0 ? vars["game"].as<string>() : boost::filesystem::current_path();
    options.game.developer = vars["dev"].as<bool>();
    options.game.neo = vars["neo"].as<bool>();
    options.graphics.width = vars["width"].as<int>();
    options.graphics.height = vars["height"].as<int>();
    options.graphics.fullscreen = vars["fullscreen"].as<bool>();
    options.graphics.vsync = vars["vsync"].as<bool>();
    options.graphics.grass = vars["grass"].as<bool>();
    options.graphics.ssao = vars["ssao"].as<bool>();
    options.graphics.ssr = vars["ssr"].as<bool>();
    options.graphics.fxaa = vars["fxaa"].as<bool>();
    options.graphics.sharpen = vars["sharpen"].as<bool>();
    options.graphics.textureQuality = static_cast<TextureQuality>(vars["texquality"].as<int>());
    options.graphics.shadowResolution = 1 << (10 + vars["shadowres"].as<int>());
    options.graphics.anisotropicFiltering = vars["anisofilter"].as<int>();
    options.graphics.drawDistance = static_cast<float>(vars["drawdist"].as<int>());
    options.audio.musicVolume = vars["musicvol"].as<int>();
    options.audio.voiceVolume = vars["voicevol"].as<int>();
    options.audio.soundVolume = vars["soundvol"].as<int>();
    options.audio.movieVolume = vars["movievol"].as<int>();
    options.logging.level = static_cast<LogLevel>(vars["loglevel"].as<int>());
    options.logging.channels = vars["logch"].as<int>();
    options.logging.logToFile = vars["logfile"].as<bool>();

    return move(options);
}

} // namespace engine

} // namespace reone
