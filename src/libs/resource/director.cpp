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
static constexpr char kOverrideDirectoryName[] = "override";

static constexpr char kTexturePackFilenameGUI[] = "swpc_tex_gui.erf";
static constexpr char kTexturePackFilenameHigh[] = "swpc_tex_tpa.erf";
static constexpr char kTexturePackFilenameMedium[] = "swpc_tex_tpb.erf";
static constexpr char kTexturePackFilenameLow[] = "swpc_tex_tpc.erf";

static constexpr char kExeFilenameKotor[] = "swkotor.exe";
static constexpr char kExeFilenameTsl[] = "swkotor2.exe";

static constexpr char kShaderPackFilename[] = "shaderpack.erf";

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
    _resources.addERF(getFileIgnoreCase(std::filesystem::current_path(), kShaderPackFilename));

    auto keyPath = findFileIgnoreCase(_gamePath, kKeyFilename);
    if (keyPath) {
        _resources.addKEY(*keyPath);
    }

    auto texPacksPath = findFileIgnoreCase(_gamePath, kTexturePackDirectoryName);
    if (texPacksPath) {
        auto guiPackPath = findFileIgnoreCase(*texPacksPath, kTexturePackFilenameGUI);
        if (guiPackPath) {
            _resources.addERF(*guiPackPath);
        }
        auto &texPack = kTexQualityToTexPack.at(_graphicsOpt.textureQuality);
        auto texPackPath = findFileIgnoreCase(*texPacksPath, texPack);
        if (texPackPath) {
            _resources.addERF(*texPackPath);
        }
    }

    auto musicPath = findFileIgnoreCase(_gamePath, kMusicDirectoryName);
    if (musicPath) {
        _resources.addFolder(*musicPath);
    }
    auto soundsPath = findFileIgnoreCase(_gamePath, kSoundsDirectoryName);
    if (soundsPath) {
        _resources.addFolder(*soundsPath);
    }

    if (_gameId == GameID::TSL) {
        auto voicePath = findFileIgnoreCase(_gamePath, kVoiceDirectoryName);
        if (voicePath) {
            _resources.addFolder(*voicePath);
        }
    } else {
        auto wavesPath = findFileIgnoreCase(_gamePath, kWavesDirectoryName);
        if (wavesPath) {
            _resources.addFolder(*wavesPath);
        }
    }

    auto lipsPath = findFileIgnoreCase(_gamePath, kLipsDirectoryName);
    if (lipsPath) {
        for (auto &filename : g_globalLipFiles) {
            auto globalLipPath = findFileIgnoreCase(*lipsPath, filename);
            if (globalLipPath) {
                _resources.addERF(*globalLipPath);
            }
        }
    }

    auto patchPath = findFileIgnoreCase(_gamePath, kPatchFilename);
    if (patchPath) {
        _resources.addERF(*patchPath);
    }
    auto overridePath = findFileIgnoreCase(_gamePath, kOverrideDirectoryName);
    if (overridePath) {
        _resources.addFolder(*overridePath);
    }

    std::optional<std::filesystem::path> exePath;
    if (_gameId == GameID::TSL) {
        exePath = findFileIgnoreCase(_gamePath, kExeFilenameTsl);
    } else {
        exePath = findFileIgnoreCase(_gamePath, kExeFilenameKotor);
    }
    if (exePath) {
        _resources.addEXE(*exePath);
    }
}

void ResourceDirector::loadModuleResources(const std::string &name) {
    auto modulesPath = findFileIgnoreCase(_gamePath, kModulesDirectoryName);
    if (!modulesPath) {
        throw ResourceNotFoundException("Modules directory not found");
    }

    auto &resources = _resources;
    auto rimPath = findFileIgnoreCase(*modulesPath, name + ".rim");
    if (rimPath) {
        resources.addRIM(*rimPath, true);
    }
    auto rimsPath = findFileIgnoreCase(*modulesPath, name + "_s.rim");
    if (rimsPath) {
        resources.addRIM(*rimsPath, true);
    }
    auto modPath = findFileIgnoreCase(*modulesPath, name + ".mod");
    if (modPath) {
        resources.addERF(*modPath, true);
    }
    if (!rimPath && !rimsPath && !modPath) {
        throw ResourceNotFoundException("Module archives not found: " + name);
    }

    auto lipsPath = findFileIgnoreCase(_gamePath, kLipsDirectoryName);
    if (lipsPath) {
        auto locModPath = findFileIgnoreCase(*lipsPath, name + "_loc.mod");
        if (locModPath) {
            resources.addERF(*locModPath, true);
        }
    }

    if (_gameId == GameID::TSL) {
        auto dlgErfPath = findFileIgnoreCase(*modulesPath, name + "_dlg.erf");
        if (dlgErfPath) {
            resources.addERF(*dlgErfPath, true);
        }
    }
}

} // namespace resource

} // namespace reone
