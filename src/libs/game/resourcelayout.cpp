/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/game/resourcelayout.h"

#include "reone/graphics/types.h"
#include "reone/resource/2das.h"
#include "reone/resource/di/services.h"
#include "reone/resource/exception/notfound.h"
#include "reone/resource/gffs.h"
#include "reone/resource/resources.h"
#include "reone/system/fileutil.h"

#include "reone/game/options.h"
#include "reone/game/types.h"

using namespace reone::graphics;
using namespace reone::resource;

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

static const std::vector<std::string> g_nonTransientLipFiles {"global.mod", "localization.mod"};

static const std::unordered_map<TextureQuality, std::string> texPackByQuality {
    {TextureQuality::High, kTexturePackFilenameHigh},
    {TextureQuality::Medium, kTexturePackFilenameMedium},
    {TextureQuality::Low, kTexturePackFilenameLow}};

void ResourceLayout::init() {
    if (_gameId == GameID::KotOR) {
        initForKotOR();
    } else if (_gameId == GameID::TSL) {
        initForTSL();
    } else {
        throw std::logic_error("Invalid game ID: " + std::to_string(static_cast<int>(_gameId)));
    }
}

void ResourceLayout::initForKotOR() {
    auto &resources = _resourceSvc.resources;

    resources.indexKEY(*findFileIgnoreCase(_options.game.path, kKeyFilename));
    resources.indexERF(*findFileIgnoreCase(_options.game.path, kPatchFilename));

    auto texPacksPath = findFileIgnoreCase(_options.game.path, kTexturePackDirectoryName);
    resources.indexERF(*findFileIgnoreCase(*texPacksPath, kTexturePackFilenameGUI));

    auto &texPack = texPackByQuality.find(_options.graphics.textureQuality)->second;
    resources.indexERF(*findFileIgnoreCase(*texPacksPath, texPack));

    resources.indexFolder(*findFileIgnoreCase(_options.game.path, kMusicDirectoryName));
    resources.indexFolder(*findFileIgnoreCase(_options.game.path, kSoundsDirectoryName));
    resources.indexFolder(*findFileIgnoreCase(_options.game.path, kWavesDirectoryName));

    auto lipsPath = findFileIgnoreCase(_options.game.path, kLipsDirectoryName);
    for (auto &filename : g_nonTransientLipFiles) {
        resources.indexERF(*findFileIgnoreCase(*lipsPath, filename));
    }

    resources.indexFolder(*findFileIgnoreCase(_options.game.path, kOverrideDirectoryName));
    resources.indexEXE(*findFileIgnoreCase(_options.game.path, kExeFilenameKotor));
}

void ResourceLayout::initForTSL() {
    auto &resources = _resourceSvc.resources;

    resources.indexKEY(*findFileIgnoreCase(_options.game.path, kKeyFilename));

    auto texPacksPath = findFileIgnoreCase(_options.game.path, kTexturePackDirectoryName);
    resources.indexERF(*findFileIgnoreCase(*texPacksPath, kTexturePackFilenameGUI));

    auto &texPack = texPackByQuality.find(_options.graphics.textureQuality)->second;
    resources.indexERF(*findFileIgnoreCase(*texPacksPath, texPack));

    resources.indexFolder(*findFileIgnoreCase(_options.game.path, kMusicDirectoryName));
    resources.indexFolder(*findFileIgnoreCase(_options.game.path, kSoundsDirectoryName));
    resources.indexFolder(*findFileIgnoreCase(_options.game.path, kVoiceDirectoryName));

    auto lipsPath = findFileIgnoreCase(_options.game.path, kLipsDirectoryName);
    resources.indexERF(*findFileIgnoreCase(*lipsPath, kLocalizationLipFilename));

    resources.indexFolder(*findFileIgnoreCase(_options.game.path, kOverrideDirectoryName));
    resources.indexEXE(*findFileIgnoreCase(_options.game.path, kExeFilenameTsl));
}

std::set<std::string> ResourceLayout::moduleNames() {
    auto moduleNames = std::set<std::string>();
    auto modulesPath = findFileIgnoreCase(_options.game.path, kModulesDirectoryName);
    if (!modulesPath) {
        throw ResourceNotFoundException("Modules directory not found");
    }
    for (auto &entry : std::filesystem::directory_iterator(*modulesPath)) {
        auto filename = boost::to_lower_copy(entry.path().filename().string());
        if (boost::ends_with(filename, ".mod") || (boost::ends_with(filename, ".rim") && !boost::ends_with(filename, "_s.rim"))) {
            auto moduleName = boost::to_lower_copy(filename.substr(0, filename.size() - 4));
            moduleNames.insert(moduleName);
        }
    }
    return moduleNames;
}

void ResourceLayout::loadModuleResources(const std::string &moduleName) {
    _resourceSvc.gffs.clear();

    auto modulesPath = findFileIgnoreCase(_options.game.path, kModulesDirectoryName);
    if (!modulesPath) {
        throw ResourceNotFoundException("Modules directory not found");
    }

    _resourceSvc.resources.indexRIM(*findFileIgnoreCase(*modulesPath, moduleName + ".rim"));
    _resourceSvc.resources.indexRIM(*findFileIgnoreCase(*modulesPath, moduleName + "_s.rim"));
    auto modPath = findFileIgnoreCase(*modulesPath, moduleName + ".mod");
    if (modPath) {
        _resourceSvc.resources.indexERF(*modPath);
    }

    auto lipsPath = findFileIgnoreCase(_options.game.path, kLipsDirectoryName);
    if (lipsPath) {
        _resourceSvc.resources.indexERF(*findFileIgnoreCase(*lipsPath, moduleName + "_loc.mod"));
    }

    if (_gameId == GameID::TSL) {
        _resourceSvc.resources.indexERF(*findFileIgnoreCase(*modulesPath, moduleName + "_dlg.erf"));
    }
}

} // namespace game

} // namespace reone
