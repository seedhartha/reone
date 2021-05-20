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

 /** @file
  *  Game functions specific to TSL.
  */

#include "game.h"

#include "../common/pathutil.h"

#include "../resource/keybifprovider.h"
#include "../resource/resources.h"

using namespace std;

using namespace reone::resource;

namespace fs = boost::filesystem;

namespace reone {

namespace game {

static constexpr char kVoiceDirectoryName[] = "streamvoice";
static constexpr char kLocalizationLipFilename[] = "localization";
static constexpr char kExeFilename[] = "swkotor2.exe";

void Game::initResourceProvidersForTSL() {
    _resources.indexKeyFile(getPathIgnoreCase(_path, kKeyFilename));

    fs::path texPacksPath(getPathIgnoreCase(_path, kTexturePackDirectoryName));
    _resources.indexErfFile(getPathIgnoreCase(texPacksPath, kGUITexturePackFilename));
    _resources.indexErfFile(getPathIgnoreCase(texPacksPath, kTexturePackFilename));

    _resources.indexDirectory(getPathIgnoreCase(_path, kMusicDirectoryName));
    _resources.indexDirectory(getPathIgnoreCase(_path, kSoundsDirectoryName));
    _resources.indexDirectory(getPathIgnoreCase(_path, kVoiceDirectoryName));

    fs::path lipsPath(getPathIgnoreCase(_path, kLipsDirectoryName));
    _resources.indexErfFile(getPathIgnoreCase(lipsPath, kLocalizationLipFilename));

    _resources.indexExeFile(getPathIgnoreCase(_path, kExeFilename));
    _resources.indexDirectory(getPathIgnoreCase(_path, kOverrideDirectoryName));
}

} // namespace game

} // namespace reone
