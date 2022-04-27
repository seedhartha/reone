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

#include "resourcelayout.h"

#include "../common/exception/validation.h"
#include "../common/pathutil.h"
#include "../graphics/types.h"
#include "../resource/2das.h"
#include "../resource/gffs.h"
#include "../resource/resources.h"
#include "../resource/services.h"

#include "options.h"
#include "types.h"

using namespace std;

using namespace reone::graphics;

namespace fs = boost::filesystem;

namespace reone {

namespace game {

static constexpr char kKeyFilename[] = "chitin.key";
static constexpr char kPatchFilename[] = "patch.erf";
static constexpr char kTexturePackDirectoryName[] = "texturepacks";
static constexpr char kMusicDirectoryName[] = "streammusic";
static constexpr char kSoundsDirectoryName[] = "streamsounds";
static constexpr char kWavesDirectoryName[] = "streamwaves";
static constexpr char kVoiceDirectoryName[] = "streamvoice";
static constexpr char kModulesDirectoryName[] = "modules";
static constexpr char kLipsDirectoryName[] = "lips";
static constexpr char kLocalizationLipFilename[] = "localization";
static constexpr char kOverrideDirectoryName[] = "override";

static constexpr char kTexturePackFilenameGUI[] = "swpc_tex_gui.erf";
static constexpr char kTexturePackFilenameHigh[] = "swpc_tex_tpa.erf";
static constexpr char kTexturePackFilenameMedium[] = "swpc_tex_tpb.erf";
static constexpr char kTexturePackFilenameLow[] = "swpc_tex_tpc.erf";

static constexpr char kExeFilenameKotor[] = "swkotor.exe";
static constexpr char kExeFilenameTsl[] = "swkotor2.exe";

static const vector<string> g_nonTransientLipFiles {"global.mod", "localization.mod"};

static const unordered_map<TextureQuality, string> texPackByQuality {
    {TextureQuality::High, kTexturePackFilenameHigh},
    {TextureQuality::Medium, kTexturePackFilenameMedium},
    {TextureQuality::Low, kTexturePackFilenameLow}};

void ResourceLayout::init() {
    if (_gameId == GameID::KotOR) {
        initForKotOR();
    } else if (_gameId == GameID::TSL) {
        initForTSL();
    } else {
        throw logic_error("Unsupported game ID: " + to_string(static_cast<int>(_gameId)));
    }
}

void ResourceLayout::initForKotOR() {
    _resourceSvc.resources.indexKeyFile(getPathIgnoreCase(_options.gamePath, kKeyFilename));
    _resourceSvc.resources.indexErfFile(getPathIgnoreCase(_options.gamePath, kPatchFilename));

    fs::path texPacksPath(getPathIgnoreCase(_options.gamePath, kTexturePackDirectoryName));
    _resourceSvc.resources.indexErfFile(getPathIgnoreCase(texPacksPath, kTexturePackFilenameGUI));

    auto texPack = texPackByQuality.find(_options.graphics.textureQuality)->second;
    _resourceSvc.resources.indexErfFile(getPathIgnoreCase(texPacksPath, texPack));

    _resourceSvc.resources.indexDirectory(getPathIgnoreCase(_options.gamePath, kMusicDirectoryName));
    _resourceSvc.resources.indexDirectory(getPathIgnoreCase(_options.gamePath, kSoundsDirectoryName));
    _resourceSvc.resources.indexDirectory(getPathIgnoreCase(_options.gamePath, kWavesDirectoryName));

    fs::path lipsPath(getPathIgnoreCase(_options.gamePath, kLipsDirectoryName));
    for (auto &filename : g_nonTransientLipFiles) {
        _resourceSvc.resources.indexErfFile(getPathIgnoreCase(lipsPath, filename));
    }

    _resourceSvc.resources.indexDirectory(getPathIgnoreCase(_options.gamePath, kOverrideDirectoryName));
    _resourceSvc.resources.indexExeFile(getPathIgnoreCase(_options.gamePath, kExeFilenameKotor));
}

void ResourceLayout::initForTSL() {
    _resourceSvc.resources.indexKeyFile(getPathIgnoreCase(_options.gamePath, kKeyFilename));

    fs::path texPacksPath(getPathIgnoreCase(_options.gamePath, kTexturePackDirectoryName));
    _resourceSvc.resources.indexErfFile(getPathIgnoreCase(texPacksPath, kTexturePackFilenameGUI));

    auto texPack = texPackByQuality.find(_options.graphics.textureQuality)->second;
    _resourceSvc.resources.indexErfFile(getPathIgnoreCase(texPacksPath, texPack));

    _resourceSvc.resources.indexDirectory(getPathIgnoreCase(_options.gamePath, kMusicDirectoryName));
    _resourceSvc.resources.indexDirectory(getPathIgnoreCase(_options.gamePath, kSoundsDirectoryName));
    _resourceSvc.resources.indexDirectory(getPathIgnoreCase(_options.gamePath, kVoiceDirectoryName));

    fs::path lipsPath(getPathIgnoreCase(_options.gamePath, kLipsDirectoryName));
    _resourceSvc.resources.indexErfFile(getPathIgnoreCase(lipsPath, kLocalizationLipFilename));

    _resourceSvc.resources.indexDirectory(getPathIgnoreCase(_options.gamePath, kOverrideDirectoryName));
    _resourceSvc.resources.indexExeFile(getPathIgnoreCase(_options.gamePath, kExeFilenameTsl));
}

void ResourceLayout::loadModuleResources(const string &moduleName) {
    _resourceSvc.twoDas.invalidate();
    _resourceSvc.gffs.invalidate();
    _resourceSvc.resources.clearTransientProviders();

    fs::path modulesPath(getPathIgnoreCase(_options.gamePath, kModulesDirectoryName));
    if (modulesPath.empty()) {
        throw ValidationException("Modules directory not found");
    }

    fs::path modPath(getPathIgnoreCase(modulesPath, moduleName + ".mod"));
    if (!modPath.empty()) {
        _resourceSvc.resources.indexErfFile(getPathIgnoreCase(modulesPath, moduleName + ".mod", false), true);
    } else {
        _resourceSvc.resources.indexRimFile(getPathIgnoreCase(modulesPath, moduleName + ".rim"), true);
        _resourceSvc.resources.indexRimFile(getPathIgnoreCase(modulesPath, moduleName + "_s.rim"), true);
    }

    fs::path lipsPath(getPathIgnoreCase(_options.gamePath, kLipsDirectoryName));
    if (!lipsPath.empty()) {
        _resourceSvc.resources.indexErfFile(getPathIgnoreCase(lipsPath, moduleName + "_loc.mod"), true);
    }

    if (_gameId == GameID::TSL) {
        _resourceSvc.resources.indexErfFile(getPathIgnoreCase(modulesPath, moduleName + "_dlg.erf"), true);
    }
}

} // namespace game

} // namespace reone
