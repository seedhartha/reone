/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "manager.h"

#include <boost/algorithm/string.hpp>

#include "../core/log.h"
#include "../core/pathutil.h"
#include "../core/streamutil.h"

#include "erffile.h"
#include "biffile.h"
#include "bwmfile.h"
#include "folder.h"
#include "mdlfile.h"
#include "rimfile.h"
#include "tgafile.h"
#include "tpcfile.h"
#include "wavfile.h"

using namespace reone::audio;
using namespace reone::render;

namespace fs = boost::filesystem;

namespace reone {

namespace resources {

static const char kKeyFileName[] = "chitin.key";
static const char kPatchFileName[] = "patch.erf";
static const char kTalkTableFileName[] = "dialog.tlk";
static const char kModulesDirectoryName[] = "modules";
static const char kTexturePackDirectoryName[] = "texturepacks";
static const char kTexturePackFilename[] = "swpc_tex_tpa.erf";
static const char kGUITexturePackFilename[] = "swpc_tex_gui.erf";
static const char kMusicDirectoryName[] = "streammusic";

static std::map<std::string, std::shared_ptr<ByteArray>> g_resCache = std::map<std::string, std::shared_ptr<ByteArray>>();
static std::map<std::string, std::shared_ptr<TwoDaTable>> g_2daCache = std::map<std::string, std::shared_ptr<TwoDaTable>>();
static std::map<std::string, std::shared_ptr<GffStruct>> g_gffCache = std::map<std::string, std::shared_ptr<GffStruct>>();
static std::map<std::string, std::shared_ptr<TalkTable>> g_talkTableCache = std::map<std::string, std::shared_ptr<TalkTable>>();
static std::map<std::string, std::shared_ptr<AudioStream>> g_audioCache = std::map<std::string, std::shared_ptr<AudioStream>>();
static std::map<std::string, std::shared_ptr<Model>> g_modelCache = std::map<std::string, std::shared_ptr<Model>>();
static std::map<std::string, std::shared_ptr<Walkmesh>> g_walkmeshCache = std::map<std::string, std::shared_ptr<Walkmesh>>();
static std::map<std::string, std::shared_ptr<Texture>> g_texCache = std::map<std::string, std::shared_ptr<Texture>>();
static std::map<std::string, std::shared_ptr<Font>> g_fontCache = std::map<std::string, std::shared_ptr<Font>>();

ResourceManager &ResourceManager::instance() {
    static ResourceManager instance;
    return instance;
}

void ResourceManager::init(GameVersion version, const boost::filesystem::path &gamePath) {
    fs::path keyPath(getPathIgnoreCase(gamePath, kKeyFileName));
    _keyFile.load(keyPath);

    if (version == GameVersion::KotOR) {
        fs::path patchPath(getPathIgnoreCase(gamePath, kPatchFileName));
        addErfProvider(patchPath);
    }

    fs::path tlkPath(getPathIgnoreCase(gamePath, kTalkTableFileName));
    _tlkFile.load(tlkPath);

    fs::path texPacksPath(getPathIgnoreCase(gamePath, kTexturePackDirectoryName));
    fs::path texPackPath(getPathIgnoreCase(texPacksPath, kTexturePackFilename));
    fs::path guiTexPackPath(getPathIgnoreCase(texPacksPath, kGUITexturePackFilename));
    fs::path musicPath(getPathIgnoreCase(gamePath, kMusicDirectoryName));

    addErfProvider(texPackPath);
    addErfProvider(guiTexPackPath);
    addFolderProvider(musicPath);

    _version = version;
    _gamePath = gamePath;

    initModuleNames();
}

ResourceManager::~ResourceManager() {
    deinit();
}

void ResourceManager::deinit() {
    clearCaches();

    _transientProviders.clear();
    _providers.clear();
}

void ResourceManager::clearCaches() {
    g_resCache.clear();
    g_2daCache.clear();
    g_gffCache.clear();
    g_audioCache.clear();
    g_modelCache.clear();
    g_walkmeshCache.clear();
    g_texCache.clear();
    g_fontCache.clear();
}

void ResourceManager::addErfProvider(const boost::filesystem::path &path) {
    std::unique_ptr<ErfFile> erf(new ErfFile());
    erf->load(path);
    _providers.push_back(std::move(erf));
}

void ResourceManager::loadModule(const std::string &name) {
    _transientProviders.clear();
    clearCaches();

    fs::path modulesPath(getPathIgnoreCase(_gamePath, kModulesDirectoryName));
    fs::path rimPath(getPathIgnoreCase(modulesPath, name + ".rim"));
    fs::path rimsPath(getPathIgnoreCase(modulesPath, name + "_s.rim"));

    addTransientRimProvider(rimPath);
    addTransientRimProvider(rimsPath);
}

void ResourceManager::addTransientRimProvider(const fs::path &path) {
    std::unique_ptr<RimFile> rim(new RimFile());
    rim->load(path);
    _transientProviders.push_back(std::move(rim));
}

void ResourceManager::addFolderProvider(const fs::path &path) {
    std::unique_ptr<Folder> folder(new Folder());
    folder->load(path);
    _providers.push_back(std::move(folder));
}

void ResourceManager::initModuleNames() {
    fs::path modules(getPathIgnoreCase(_gamePath, kModulesDirectoryName));

    for (auto &entry : fs::directory_iterator(modules)) {
        std::string filename(entry.path().filename().string());
        if (!boost::ends_with(filename, ".rim") || boost::ends_with(filename, "_s.rim")) continue;

        std::string moduleName(filename.substr(0, filename.size() - 4));
        boost::to_lower(moduleName);

        _moduleNames.push_back(moduleName);
    }

    std::sort(_moduleNames.begin(), _moduleNames.end());
}

std::shared_ptr<ByteArray> ResourceManager::find(const std::string &resRef, ResourceType type) {
    std::string cacheKey(getCacheKey(resRef, type));
    auto it = g_resCache.find(cacheKey);
    if (it != g_resCache.end()) {
        return it->second;
    }
    debug("Loading resource " + cacheKey);

    std::shared_ptr<ByteArray> data = find(_transientProviders, resRef, type);
    if (!data) {
        data = find(_providers, resRef, type);
    }
    if (!data) {
        KeyFile::KeyEntry key;
        if (_keyFile.find(resRef, type, key)) {
            std::string filename(_keyFile.getFilename(key.bifIdx).c_str());
            boost::replace_all(filename, "\\", "/");

            fs::path bifPath(getPathIgnoreCase(_gamePath, filename));

            BifFile bif;
            bif.load(bifPath);

            data = std::make_shared<ByteArray>(bif.getResourceData(key.resIdx));
        }
    }
    if (!data) {
        warn("Resource not found: " + cacheKey);
    }

    auto pair = g_resCache.insert(std::make_pair(cacheKey, std::move(data)));

    return pair.first->second;
}

std::string ResourceManager::getCacheKey(const std::string &resRef, resources::ResourceType type) const {
    return str(boost::format("%s:%d") % resRef % static_cast<int>(type));
}

std::shared_ptr<ByteArray> ResourceManager::find(const std::vector<std::unique_ptr<IResourceProvider>> &providers, const std::string &resRef, ResourceType type) {
    for (auto it = providers.rbegin(); it != providers.rend(); ++it) {
        const std::unique_ptr<IResourceProvider> &provider = *it;
        if (!provider->supports(type)) continue;

        std::shared_ptr<ByteArray> data(provider->find(resRef, type));
        if (data) return data;
    }

    return nullptr;
}

std::shared_ptr<TwoDaTable> ResourceManager::find2DA(const std::string &resRef) {
    auto it = g_2daCache.find(resRef);
    if (it != g_2daCache.end()) {
        return it->second;
    }
    debug("Loading 2DA table " + resRef);

    std::shared_ptr<ByteArray> twoDaData(find(resRef, ResourceType::TwoDa));
    std::shared_ptr<TwoDaTable> table;

    if (twoDaData) {
        TwoDaFile twoDa;
        twoDa.load(wrap(twoDaData));
        table = twoDa.table();
    } else {
        warn("2DA table not found: " + resRef);
    }

    auto pair = g_2daCache.insert(std::make_pair(resRef, table));

    return pair.first->second;
}

std::shared_ptr<GffStruct> ResourceManager::findGFF(const std::string &resRef, ResourceType type) {
    std::string cacheKey(getCacheKey(resRef, type));
    auto it = g_gffCache.find(cacheKey);
    if (it != g_gffCache.end()) {
        return it->second;
    }
    debug("Loading GFF " + cacheKey);

    std::shared_ptr<ByteArray> gffData(find(resRef, type));
    std::shared_ptr<GffStruct> gffs;

    if (gffData) {
        GffFile gff;
        gff.load(wrap(gffData));
        gffs = gff.top();
    } else {
        warn("GFF file not found: " + cacheKey);
    }

    auto pair = g_gffCache.insert(std::make_pair(cacheKey, gffs));

    return pair.first->second;
}

std::shared_ptr<TalkTable> ResourceManager::findTalkTable(const std::string &resRef) {
    auto it = g_talkTableCache.find(resRef);
    if (it != g_talkTableCache.end()) {
        return it->second;
    }
    debug("Loading talk table " + resRef);

    std::shared_ptr<ByteArray> tlkData(find(resRef, ResourceType::Conversation));
    std::shared_ptr<TalkTable> table;

    if (tlkData) {
        TlkFile tlk;
        tlk.load(wrap(tlkData));
        table = tlk.table();
    } else {
        warn("TLK file not found: " + resRef);
    }

    auto pair = g_talkTableCache.insert(std::make_pair(resRef, table));

    return pair.first->second;
}

std::shared_ptr<AudioStream> ResourceManager::findAudio(const std::string &resRef) {
    auto it = g_audioCache.find(resRef);
    if (it != g_audioCache.end()) {
        return it->second;
    }
    debug("Loading audio stream " + resRef);

    std::shared_ptr<ByteArray> wavData(find(resRef, ResourceType::Wav));
    std::shared_ptr<AudioStream> stream;

    if (wavData) {
        WavFile wav;
        wav.load(wrap(wavData));
        stream = wav.stream();
    } else {
        warn("Audio stream not found: " + resRef);
    }

    auto pair = g_audioCache.insert(std::make_pair(resRef, stream));

    return pair.first->second;
}

std::shared_ptr<Model> ResourceManager::findModel(const std::string &resRef) {
    auto it = g_modelCache.find(resRef);
    if (it != g_modelCache.end()) {
        return it->second;
    }
    debug("Loading model " + resRef);

    std::shared_ptr<ByteArray> mdlData(find(resRef, ResourceType::Model));
    std::shared_ptr<ByteArray> mdxData(find(resRef, ResourceType::Mdx));
    std::shared_ptr<Model> model;

    if (mdlData && mdxData) {
        MdlFile mdl(_version);
        mdl.load(wrap(mdlData), wrap(mdxData));
        model = mdl.model();
    } else {
        warn("Model not found: " + resRef);
    }

    auto pair = g_modelCache.insert(std::make_pair(resRef, model));

    return pair.first->second;
}

std::shared_ptr<Walkmesh> ResourceManager::findWalkmesh(const std::string &resRef, ResourceType type) {
    auto it = g_walkmeshCache.find(resRef);
    if (it != g_walkmeshCache.end()) {
        return it->second;
    }
    debug("Loading walkmesh " + resRef);

    std::shared_ptr<ByteArray> bwmData(find(resRef, type));
    std::shared_ptr<Walkmesh> walkmesh;

    if (bwmData) {
        BwmFile bwm;
        bwm.load(wrap(bwmData));
        walkmesh = bwm.walkmesh();
    }

    auto pair = g_walkmeshCache.insert(std::make_pair(resRef, walkmesh));

    return pair.first->second;
}

std::shared_ptr<Texture> ResourceManager::findTexture(const std::string &resRef, TextureType type) {
    auto it = g_texCache.find(resRef);
    if (it != g_texCache.end()) {
        return it->second;
    }
    debug("Loading texture " + resRef);

    std::shared_ptr<Texture> texture;
    std::shared_ptr<ByteArray> tpcData(find(resRef, ResourceType::Texture));

    if (tpcData) {
        TpcFile tpc(resRef, type);
        tpc.load(wrap(tpcData));
        texture = tpc.texture();
    } else {
        std::shared_ptr<ByteArray> tgaData(find(resRef, ResourceType::Tga));
        if (tgaData) {
            TgaFile tga(resRef, type);
            tga.load(wrap(tgaData));
            texture = tga.texture();
        }
    }
    if (!texture) {
        warn("Texture not found: " + resRef);
    }

    auto pair = g_texCache.insert(std::make_pair(resRef, texture));

    return pair.first->second;
}

std::shared_ptr<Font> ResourceManager::findFont(const std::string &resRef) {
    auto it = g_fontCache.find(resRef);
    if (it != g_fontCache.end()) {
        return it->second;
    }
    debug("Loading font " + resRef);

    std::shared_ptr<Font> font;
    std::shared_ptr<Texture> texture(findTexture(resRef, TextureType::Font));

    if (texture) {
        font = std::make_shared<Font>();
        font->load(texture);
    }
    if (!font) {
        warn("Font not found: " + resRef);
    }

    auto pair = g_fontCache.insert(std::make_pair(resRef, font));

    return pair.first->second;
}

const TalkTableString &ResourceManager::getString(int32_t ref) const {
    return _tlkFile.table()->getString(ref);
}

const std::vector<std::string> &ResourceManager::moduleNames() const {
    return _moduleNames;
}

} // namespace resources

} // namespace reone
