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

#include "reone/resource/director.h"

#include "reone/graphics/di/services.h"
#include "reone/graphics/options.h"
#include "reone/graphics/types.h"
#include "reone/resource/di/services.h"
#include "reone/resource/exception/notfound.h"
#include "reone/resource/provider/2das.h"
#include "reone/resource/provider/dialogs.h"
#include "reone/resource/provider/gffs.h"
#include "reone/resource/provider/lips.h"
#include "reone/resource/provider/paths.h"
#include "reone/resource/provider/scripts.h"
#include "reone/resource/resources.h"
#include "reone/script/di/services.h"
#include "reone/system/fileutil.h"

using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace resource {

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

static const std::vector<std::string> g_globalLipFiles {"global.mod", "localization.mod"};

static const std::unordered_map<TextureQuality, std::string> kTexQualityToTexPack {
    {TextureQuality::High, kTexturePackFilenameHigh},
    {TextureQuality::Medium, kTexturePackFilenameMedium},
    {TextureQuality::Low, kTexturePackFilenameLow}};

void ResourceDirector::init() {
    loadGlobalResources();
}

void ResourceDirector::onModuleLoad(const std::string &name) {
    _dialogs.clear();
    _paths.clear();
    _scripts.clear();
    _lips.clear();
    _gffs.clear();
    _resources.clearLocal();

    loadModuleResources(name);
}

std::set<std::string> ResourceDirector::moduleNames() {
    auto moduleNames = std::set<std::string>();
    auto modulesPath = findFileIgnoreCase(_gamePath, kModulesDirectoryName);
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

void ResourceDirector::loadGlobalResources() {
    auto &resources = _resources;
    resources.addKEY(getFileIgnoreCase(_gamePath, kKeyFilename));

    auto texPacksPath = getFileIgnoreCase(_gamePath, kTexturePackDirectoryName);
    auto &texPack = kTexQualityToTexPack.at(_graphicsOpt.textureQuality);

    if (_gameId == GameID::TSL) {
        resources.addERF(getFileIgnoreCase(texPacksPath, kTexturePackFilenameGUI));
        resources.addERF(getFileIgnoreCase(texPacksPath, texPack));

        resources.addFolder(getFileIgnoreCase(_gamePath, kMusicDirectoryName));
        resources.addFolder(getFileIgnoreCase(_gamePath, kSoundsDirectoryName));
        resources.addFolder(getFileIgnoreCase(_gamePath, kVoiceDirectoryName));

        auto lipsPath = getFileIgnoreCase(_gamePath, kLipsDirectoryName);
        resources.addERF(getFileIgnoreCase(lipsPath, kLocalizationLipFilename));

        resources.addFolder(getFileIgnoreCase(_gamePath, kOverrideDirectoryName));
        resources.addEXE(getFileIgnoreCase(_gamePath, kExeFilenameTsl));

    } else {
        resources.addERF(getFileIgnoreCase(_gamePath, kPatchFilename));
        resources.addERF(getFileIgnoreCase(texPacksPath, kTexturePackFilenameGUI));
        resources.addERF(getFileIgnoreCase(texPacksPath, texPack));

        resources.addFolder(getFileIgnoreCase(_gamePath, kMusicDirectoryName));
        resources.addFolder(getFileIgnoreCase(_gamePath, kSoundsDirectoryName));
        resources.addFolder(getFileIgnoreCase(_gamePath, kWavesDirectoryName));

        auto lipsPath = getFileIgnoreCase(_gamePath, kLipsDirectoryName);
        for (auto &filename : g_globalLipFiles) {
            resources.addERF(getFileIgnoreCase(lipsPath, filename));
        }

        resources.addFolder(getFileIgnoreCase(_gamePath, kOverrideDirectoryName));
        resources.addEXE(getFileIgnoreCase(_gamePath, kExeFilenameKotor));
    }
}

void ResourceDirector::loadModuleResources(const std::string &name) {
    auto modulesPath = findFileIgnoreCase(_gamePath, kModulesDirectoryName);
    if (!modulesPath) {
        throw ResourceNotFoundException("Modules directory not found");
    }

    auto &resources = _resources;
    resources.addRIM(getFileIgnoreCase(*modulesPath, name + ".rim"), true);
    resources.addRIM(getFileIgnoreCase(*modulesPath, name + "_s.rim"), true);
    auto modPath = findFileIgnoreCase(*modulesPath, name + ".mod");
    if (modPath) {
        resources.addERF(*modPath, true);
    }

    auto lipsPath = findFileIgnoreCase(_gamePath, kLipsDirectoryName);
    if (lipsPath) {
        resources.addERF(getFileIgnoreCase(*lipsPath, name + "_loc.mod"), true);
    }

    if (_gameId == GameID::TSL) {
        resources.addERF(getFileIgnoreCase(*modulesPath, name + "_dlg.erf"), true);
    }
}

} // namespace resource

} // namespace reone
