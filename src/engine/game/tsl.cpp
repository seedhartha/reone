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

#include "tsl.h"

#include "../common/pathutil.h"

using namespace std;

using namespace reone::audio;
using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;
using namespace reone::script;

namespace fs = boost::filesystem;

namespace reone {

namespace game {

static constexpr char kMainMenuMusicResRef[] = "mus_sion";
static constexpr char kVoiceDirectoryName[] = "streamvoice";
static constexpr char kLocalizationLipFilename[] = "localization";
static constexpr char kExeFilename[] = "swkotor2.exe";

TSL::TSL(
    GameID gameId,
    fs::path path,
    Options options,
    ResourceServices &resource,
    GraphicsServices &graphics,
    AudioServices &audio,
    SceneServices &scene,
    ScriptServices &script
) : Game(
    gameId,
    move(path),
    move(options),
    resource, graphics, audio, scene, script
) {
    _mainMenuMusicResRef = kMainMenuMusicResRef;

    _guiColorBase = glm::vec3(0.192157f, 0.768627f, 0.647059f);
    _guiColorHilight = glm::vec3(0.768627f, 0.768627f, 0.686275f);
    _guiColorDisabled = glm::vec3(0.513725f, 0.513725f, 0.415686f);
}

void TSL::initResourceProviders() {
    _resource.resources().indexKeyFile(getPathIgnoreCase(_path, kKeyFilename));

    fs::path texPacksPath(getPathIgnoreCase(_path, kTexturePackDirectoryName));
    _resource.resources().indexErfFile(getPathIgnoreCase(texPacksPath, kGUITexturePackFilename));
    _resource.resources().indexErfFile(getPathIgnoreCase(texPacksPath, kTexturePackFilename));

    _resource.resources().indexDirectory(getPathIgnoreCase(_path, kMusicDirectoryName));
    _resource.resources().indexDirectory(getPathIgnoreCase(_path, kSoundsDirectoryName));
    _resource.resources().indexDirectory(getPathIgnoreCase(_path, kVoiceDirectoryName));

    fs::path lipsPath(getPathIgnoreCase(_path, kLipsDirectoryName));
    _resource.resources().indexErfFile(getPathIgnoreCase(lipsPath, kLocalizationLipFilename));

    _resource.resources().indexExeFile(getPathIgnoreCase(_path, kExeFilename));
    _resource.resources().indexDirectory(getPathIgnoreCase(_path, kOverrideDirectoryName));
    _resource.resources().indexDirectory(getPathIgnoreCase(fs::current_path(), kDataDirectoryName));
}

} // namespace game

} // namespace reone
