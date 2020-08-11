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

#include "resources.h"

#include <map>

#include <boost/algorithm/string.hpp>

#include "../core/log.h"
#include "../core/pathutil.h"
#include "../core/streamutil.h"

#include "erffile.h"
#include "biffile.h"
#include "bwmfile.h"
#include "folder.h"
#include "mdlfile.h"
#include "ncsfile.h"
#include "rimfile.h"
#include "tgafile.h"
#include "tpcfile.h"
#include "util.h"
#include "wavfile.h"

using namespace std;

using namespace reone::audio;
using namespace reone::render;
using namespace reone::script;

namespace fs = boost::filesystem;

namespace reone {

namespace resources {

static const char kKeyFileName[] = "chitin.key";
static const char kPatchFileName[] = "patch.erf";
static const char kTalkTableFileName[] = "dialog.tlk";

static const char kModulesDirectoryName[] = "modules";
static const char kOverrideDirectoryName[] = "override";
static const char kMusicDirectoryName[] = "streammusic";
static const char kSoundsDirectoryName[] = "streamsounds";
static const char kVoiceDirectoryName[] = "streamvoice";
static const char kWavesDirectoryName[] = "streamwaves";
static const char kTexturePackDirectoryName[] = "texturepacks";

static const char kGUITexturePackFilename[] = "swpc_tex_gui.erf";
static const char kTexturePackFilename[] = "swpc_tex_tpa.erf";

static map<string, shared_ptr<TwoDaTable>> g_2daCache;
static map<string, shared_ptr<AudioStream>> g_audioCache;
static map<string, shared_ptr<Font>> g_fontCache;
static map<string, shared_ptr<GffStruct>> g_gffCache;
static map<string, shared_ptr<Model>> g_modelCache;
static map<string, shared_ptr<ScriptProgram>> g_scripts;
static map<string, shared_ptr<ByteArray>> g_resCache;
static map<string, shared_ptr<TalkTable>> g_talkTableCache;
static map<string, shared_ptr<Texture>> g_texCache;
static map<string, shared_ptr<Walkmesh>> g_walkmeshCache;

static map<string, string> g_fontOverride = {
    { "fnt_d16x16", "fnt_d16x16b" }
};

ResourceManager &ResourceManager::instance() {
    static ResourceManager instance;
    return instance;
}

void ResourceManager::init(GameVersion version, const boost::filesystem::path &gamePath) {
    fs::path keyPath(getPathIgnoreCase(gamePath, kKeyFileName));
    if (keyPath.empty()) {
        throw runtime_error(str(boost::format("Key file not found: %s %s") % gamePath % kKeyFileName));
    }
    _keyFile.load(keyPath);

    if (version == GameVersion::KotOR) {
        fs::path patchPath(getPathIgnoreCase(gamePath, kPatchFileName));
        addErfProvider(patchPath);
    }

    fs::path texPacksPath(getPathIgnoreCase(gamePath, kTexturePackDirectoryName));
    fs::path guiTexPackPath(getPathIgnoreCase(texPacksPath, kGUITexturePackFilename));
    fs::path texPackPath(getPathIgnoreCase(texPacksPath, kTexturePackFilename));
    addErfProvider(guiTexPackPath);
    addErfProvider(texPackPath);

    fs::path musicPath(getPathIgnoreCase(gamePath, kMusicDirectoryName));
    fs::path soundsPath(getPathIgnoreCase(gamePath, kSoundsDirectoryName));
    addFolderProvider(musicPath);
    addFolderProvider(soundsPath);

    switch (version) {
        case GameVersion::KotOR: {
            fs::path wavesPath(getPathIgnoreCase(gamePath, kWavesDirectoryName));
            addFolderProvider(wavesPath);
            break;
        }
        case GameVersion::TheSithLords: {
            fs::path voicePath(getPathIgnoreCase(gamePath, kVoiceDirectoryName));
            addFolderProvider(voicePath);
            break;
        }
    }

    fs::path overridePath(getPathIgnoreCase(gamePath, kOverrideDirectoryName));
    addFolderProvider(overridePath);

    fs::path tlkPath(getPathIgnoreCase(gamePath, kTalkTableFileName));
    _tlkFile.load(tlkPath);

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
    g_2daCache.clear();
    g_audioCache.clear();
    g_fontCache.clear();
    g_gffCache.clear();
    g_modelCache.clear();
    g_scripts.clear();
    g_resCache.clear();
    g_talkTableCache.clear();
    g_texCache.clear();
    g_walkmeshCache.clear();
}

void ResourceManager::addErfProvider(const boost::filesystem::path &path) {
    unique_ptr<ErfFile> erf(new ErfFile());
    erf->load(path);
    _providers.push_back(move(erf));
}

void ResourceManager::loadModule(const string &name) {
    _transientProviders.clear();
    clearCaches();

    fs::path modulesPath(getPathIgnoreCase(_gamePath, kModulesDirectoryName));
    fs::path rimPath(getPathIgnoreCase(modulesPath, name + ".rim"));
    fs::path rimsPath(getPathIgnoreCase(modulesPath, name + "_s.rim"));

    addTransientRimProvider(rimPath);
    addTransientRimProvider(rimsPath);

    if (_version == GameVersion::TheSithLords) {
        fs::path dlgPath(getPathIgnoreCase(modulesPath, name + "_dlg.erf"));
        addTransientErfProvider(dlgPath);
    }
}

void ResourceManager::addTransientRimProvider(const fs::path &path) {
    unique_ptr<RimFile> rim(new RimFile());
    rim->load(path);
    _transientProviders.push_back(move(rim));
}

void ResourceManager::addTransientErfProvider(const fs::path &path) {
    unique_ptr<ErfFile> erf(new ErfFile());
    erf->load(path);
    _transientProviders.push_back(move(erf));
}

void ResourceManager::addFolderProvider(const fs::path &path) {
    unique_ptr<Folder> folder(new Folder());
    folder->load(path);
    _providers.push_back(move(folder));
}

void ResourceManager::initModuleNames() {
    fs::path modules(getPathIgnoreCase(_gamePath, kModulesDirectoryName));

    for (auto &entry : fs::directory_iterator(modules)) {
        string filename(entry.path().filename().string());
        if (!boost::ends_with(filename, ".rim") || boost::ends_with(filename, "_s.rim")) continue;

        string moduleName(filename.substr(0, filename.size() - 4));
        boost::to_lower(moduleName);

        _moduleNames.push_back(moduleName);
    }

    sort(_moduleNames.begin(), _moduleNames.end());
}

shared_ptr<ByteArray> ResourceManager::find(const string &resRef, ResourceType type) {
    string cacheKey(getCacheKey(resRef, type));
    auto it = g_resCache.find(cacheKey);
    if (it != g_resCache.end()) {
        return it->second;
    }
    debug("Resources: load " + cacheKey, 2);

    shared_ptr<ByteArray> data = find(_transientProviders, resRef, type);
    if (!data) {
        data = find(_providers, resRef, type);
    }
    if (!data) {
        KeyFile::KeyEntry key;
        if (_keyFile.find(resRef, type, key)) {
            string filename(_keyFile.getFilename(key.bifIdx).c_str());
            boost::replace_all(filename, "\\", "/");

            fs::path bifPath(getPathIgnoreCase(_gamePath, filename));

            BifFile bif;
            bif.load(bifPath);

            data = make_shared<ByteArray>(bif.getResourceData(key.resIdx));
        }
    }
    if (!data) {
        warn("Resources: not found: " + cacheKey);
    }

    auto pair = g_resCache.insert(make_pair(cacheKey, move(data)));

    return pair.first->second;
}

string ResourceManager::getCacheKey(const string &resRef, resources::ResourceType type) const {
    return str(boost::format("%s.%s") % resRef % getExtByResType(type));
}

shared_ptr<ByteArray> ResourceManager::find(const vector<unique_ptr<IResourceProvider>> &providers, const string &resRef, ResourceType type) {
    for (auto it = providers.rbegin(); it != providers.rend(); ++it) {
        const unique_ptr<IResourceProvider> &provider = *it;
        if (!provider->supports(type)) continue;

        shared_ptr<ByteArray> data(provider->find(resRef, type));
        if (data) return data;
    }

    return nullptr;
}

shared_ptr<TwoDaTable> ResourceManager::find2DA(const string &resRef) {
    auto it = g_2daCache.find(resRef);
    if (it != g_2daCache.end()) {
        return it->second;
    }
    shared_ptr<ByteArray> twoDaData(find(resRef, ResourceType::TwoDa));
    shared_ptr<TwoDaTable> table;

    if (twoDaData) {
        TwoDaFile twoDa;
        twoDa.load(wrap(twoDaData));
        table = twoDa.table();
    }

    auto pair = g_2daCache.insert(make_pair(resRef, table));

    return pair.first->second;
}

shared_ptr<GffStruct> ResourceManager::findGFF(const string &resRef, ResourceType type) {
    string cacheKey(getCacheKey(resRef, type));
    auto it = g_gffCache.find(cacheKey);
    if (it != g_gffCache.end()) {
        return it->second;
    }
    shared_ptr<ByteArray> gffData(find(resRef, type));
    shared_ptr<GffStruct> gffs;

    if (gffData) {
        GffFile gff;
        gff.load(wrap(gffData));
        gffs = gff.top();
    }

    auto pair = g_gffCache.insert(make_pair(cacheKey, gffs));

    return pair.first->second;
}

shared_ptr<TalkTable> ResourceManager::findTalkTable(const string &resRef) {
    auto it = g_talkTableCache.find(resRef);
    if (it != g_talkTableCache.end()) {
        return it->second;
    }
    shared_ptr<ByteArray> tlkData(find(resRef, ResourceType::Conversation));
    shared_ptr<TalkTable> table;

    if (tlkData) {
        TlkFile tlk;
        tlk.load(wrap(tlkData));
        table = tlk.table();
    }

    auto pair = g_talkTableCache.insert(make_pair(resRef, table));

    return pair.first->second;
}

shared_ptr<AudioStream> ResourceManager::findAudio(const string &resRef) {
    auto it = g_audioCache.find(resRef);
    if (it != g_audioCache.end()) {
        return it->second;
    }

    shared_ptr<ByteArray> wavData(find(resRef, ResourceType::Wav));
    shared_ptr<AudioStream> stream;

    if (wavData) {
        WavFile wav;
        wav.load(wrap(wavData));
        stream = wav.stream();
    }

    auto pair = g_audioCache.insert(make_pair(resRef, stream));

    return pair.first->second;
}

shared_ptr<Model> ResourceManager::findModel(const string &resRef) {
    auto it = g_modelCache.find(resRef);
    if (it != g_modelCache.end()) {
        return it->second;
    }
    shared_ptr<ByteArray> mdlData(find(resRef, ResourceType::Model));
    shared_ptr<ByteArray> mdxData(find(resRef, ResourceType::Mdx));
    shared_ptr<Model> model;

    if (mdlData && mdxData) {
        MdlFile mdl(_version);
        mdl.load(wrap(mdlData), wrap(mdxData));
        model = mdl.model();
    }

    auto pair = g_modelCache.insert(make_pair(resRef, model));

    return pair.first->second;
}

shared_ptr<Walkmesh> ResourceManager::findWalkmesh(const string &resRef, ResourceType type) {
    auto it = g_walkmeshCache.find(resRef);
    if (it != g_walkmeshCache.end()) {
        return it->second;
    }
    shared_ptr<ByteArray> bwmData(find(resRef, type));
    shared_ptr<Walkmesh> walkmesh;

    if (bwmData) {
        BwmFile bwm;
        bwm.load(wrap(bwmData));
        walkmesh = bwm.walkmesh();
    }

    auto pair = g_walkmeshCache.insert(make_pair(resRef, walkmesh));

    return pair.first->second;
}

shared_ptr<Texture> ResourceManager::findTexture(const string &resRef, TextureType type) {
    auto it = g_texCache.find(resRef);
    if (it != g_texCache.end()) {
        return it->second;
    }
    bool tryTpc = _version == GameVersion::TheSithLords || type != TextureType::Lightmap;
    shared_ptr<Texture> texture;

    if (tryTpc) {
        shared_ptr<ByteArray> tpcData(find(resRef, ResourceType::Texture));
        if (tpcData) {
            TpcFile tpc(resRef, type);
            tpc.load(wrap(tpcData));
            texture = tpc.texture();
        }
    }
    if (!texture) {
        shared_ptr<ByteArray> tgaData(find(resRef, ResourceType::Tga));
        if (tgaData) {
            TgaFile tga(resRef, type);
            tga.load(wrap(tgaData));
            texture = tga.texture();
        }
    }

    auto pair = g_texCache.insert(make_pair(resRef, texture));

    return pair.first->second;
}

shared_ptr<Font> ResourceManager::findFont(const string &resRef) {
    auto fontOverride = g_fontOverride.find(resRef);
    const string &finalResRef = fontOverride != g_fontOverride.end() ? fontOverride->second : resRef;

    auto it = g_fontCache.find(finalResRef);
    if (it != g_fontCache.end()) {
        return it->second;
    }

    shared_ptr<Font> font;
    shared_ptr<Texture> texture(findTexture(finalResRef, TextureType::GUI));

    if (texture) {
        font = make_shared<Font>();
        font->load(texture);
    }

    auto pair = g_fontCache.insert(make_pair(finalResRef, font));

    return pair.first->second;
}

shared_ptr<ScriptProgram> ResourceManager::findScript(const string &resRef) {
    auto it = g_scripts.find(resRef);
    if (it != g_scripts.end()) return it->second;

    shared_ptr<ScriptProgram> program;
    shared_ptr<ByteArray> ncsData(ResMan.find(resRef, ResourceType::CompiledScript));

    if (ncsData) {
        NcsFile ncs(resRef);
        ncs.load(wrap(ncsData));
        program = ncs.program();
    }

    auto pair = g_scripts.insert(make_pair(resRef, program));

    return pair.first->second;
}

const TalkTableString &ResourceManager::getString(int32_t ref) const {
    return _tlkFile.table()->getString(ref);
}

const vector<string> &ResourceManager::moduleNames() const {
    return _moduleNames;
}

} // namespace resources

} // namespace reone
