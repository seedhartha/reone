/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include "../common/log.h"
#include "../common/pathutil.h"
#include "../common/streamutil.h"

#include "biffile.h"
#include "erffile.h"
#include "folder.h"
#include "rimfile.h"
#include "util.h"

using namespace std;

namespace fs = boost::filesystem;

namespace reone {

namespace resource {

static const char kKeyFileName[] = "chitin.key";
static const char kPatchFileName[] = "patch.erf";
static const char kTalkTableFileName[] = "dialog.tlk";
static const char kExeFileNameKotor[] = "swkotor.exe";
static const char kExeFileNameTsl[] = "swkotor2.exe";

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
static map<string, shared_ptr<GffStruct>> g_gffCache;
static map<string, shared_ptr<ByteArray>> g_resCache;
static map<string, shared_ptr<TalkTable>> g_talkTableCache;

Resources &Resources::instance() {
    static Resources instance;
    return instance;
}

void Resources::init(GameVersion version, const fs::path &gamePath) {
    _version = version;
    _gamePath = gamePath;

    indexKeyFile();
    indexTexturePacks();
    indexAudioFiles();
    indexOverrideDirectory();
    indexTalkTable();
    indexExeFile();
    loadModuleNames();
}

void Resources::indexKeyFile() {
    fs::path path(getPathIgnoreCase(_gamePath, kKeyFileName));

    if (path.empty()) {
        throw runtime_error(str(boost::format("Key file not found: %s %s") % _gamePath % kKeyFileName));
    }
    _keyFile.load(path);

    debug(boost::format("Resources: indexed: %s") % path);
}

void Resources::indexTexturePacks() {
    if (_version == GameVersion::KotOR) {
        fs::path patchPath(getPathIgnoreCase(_gamePath, kPatchFileName));
        indexErfFile(patchPath);
    }
    fs::path texPacksPath(getPathIgnoreCase(_gamePath, kTexturePackDirectoryName));
    fs::path guiTexPackPath(getPathIgnoreCase(texPacksPath, kGUITexturePackFilename));
    fs::path texPackPath(getPathIgnoreCase(texPacksPath, kTexturePackFilename));

    indexErfFile(guiTexPackPath);
    indexErfFile(texPackPath);
}

void Resources::indexErfFile(const fs::path &path) {
    unique_ptr<ErfFile> erf(new ErfFile());
    erf->load(path);

    _providers.push_back(move(erf));

    debug(boost::format("Resources: indexed: %s") % path);
}

void Resources::indexAudioFiles() {
    fs::path musicPath(getPathIgnoreCase(_gamePath, kMusicDirectoryName));
    fs::path soundsPath(getPathIgnoreCase(_gamePath, kSoundsDirectoryName));

    indexDirectory(musicPath);
    indexDirectory(soundsPath);

    switch (_version) {
        case GameVersion::TheSithLords: {
            fs::path voicePath(getPathIgnoreCase(_gamePath, kVoiceDirectoryName));
            indexDirectory(voicePath);
            break;
        }
        default: {
            fs::path wavesPath(getPathIgnoreCase(_gamePath, kWavesDirectoryName));
            indexDirectory(wavesPath);
            break;
        }
    }
}

void Resources::indexDirectory(const fs::path &path) {
    unique_ptr<Folder> folder(new Folder());
    folder->load(path);

    _providers.push_back(move(folder));

    debug(boost::format("Resources: indexed: %s") % path);
}

void Resources::indexOverrideDirectory() {
    fs::path path(getPathIgnoreCase(_gamePath, kOverrideDirectoryName));
    indexDirectory(path);
}

void Resources::indexTalkTable() {
    fs::path path(getPathIgnoreCase(_gamePath, kTalkTableFileName));
    _tlkFile.load(path);

    debug(boost::format("Resources: indexed: %s") % path);
}

void Resources::indexExeFile() {
    string filename(_version == GameVersion::TheSithLords ? kExeFileNameTsl : kExeFileNameKotor);
    fs::path path(getPathIgnoreCase(_gamePath, filename));

    _exeFile.load(path);

    debug(boost::format("Resources: indexed: %s") % path);
}

void Resources::loadModuleNames() {
    fs::path modules(getPathIgnoreCase(_gamePath, kModulesDirectoryName));

    for (auto &entry : fs::directory_iterator(modules)) {
        string filename(entry.path().filename().string());
        boost::to_lower(filename);

        if (!boost::ends_with(filename, ".rim") || boost::ends_with(filename, "_s.rim")) continue;

        string moduleName(filename.substr(0, filename.size() - 4));
        boost::to_lower(moduleName);

        _moduleNames.push_back(moduleName);
    }

    sort(_moduleNames.begin(), _moduleNames.end());
}

Resources::~Resources() {
    deinit();
}

void Resources::deinit() {
    invalidateCache();

    _transientProviders.clear();
    _providers.clear();
}

void Resources::invalidateCache() {
    g_2daCache.clear();
    g_gffCache.clear();
    g_resCache.clear();
    g_talkTableCache.clear();
}

void Resources::loadModule(const string &name) {
    invalidateCache();
    _transientProviders.clear();

    fs::path modulesPath(getPathIgnoreCase(_gamePath, kModulesDirectoryName));
    fs::path rimPath(getPathIgnoreCase(modulesPath, name + ".rim"));
    fs::path rimsPath(getPathIgnoreCase(modulesPath, name + "_s.rim"));

    indexTransientRimFile(rimPath);
    indexTransientRimFile(rimsPath);

    if (_version == GameVersion::TheSithLords) {
        fs::path dlgPath(getPathIgnoreCase(modulesPath, name + "_dlg.erf"));
        indexTransientErfFile(dlgPath);
    }
}

void Resources::indexTransientRimFile(const fs::path &path) {
    unique_ptr<RimFile> rim(new RimFile());
    rim->load(path);

    _transientProviders.push_back(move(rim));

    debug(boost::format("Resources: indexed: %s") % path);
}

void Resources::indexTransientErfFile(const fs::path &path) {
    unique_ptr<ErfFile> erf(new ErfFile());
    erf->load(path);

    _transientProviders.push_back(move(erf));

    debug(boost::format("Resources: indexed: %s") % path);
}

template <class T>
static shared_ptr<T> findResource(const string &key, map<string, shared_ptr<T>> &cache, const function<shared_ptr<T>()> &getter) {
    auto maybeResource = cache.find(key);
    if (maybeResource != cache.end()) {
        return maybeResource->second;
    };
    auto inserted = cache.insert(make_pair(key, getter()));

    return inserted.first->second;
}

shared_ptr<TwoDaTable> Resources::get2DA(const string &resRef) {
    return findResource<TwoDaTable>(resRef, g_2daCache, [this, &resRef]() {
        shared_ptr<ByteArray> data(get(resRef, ResourceType::TwoDa));
        shared_ptr<TwoDaTable> table;

        if (data) {
            TwoDaFile file;
            file.load(wrap(data));
            table = file.table();
        }

        return move(table);
    });
}

shared_ptr<ByteArray> Resources::get(const string &resRef, ResourceType type, bool logNotFound) {
    string cacheKey(getCacheKey(resRef, type));
    auto res = g_resCache.find(cacheKey);

    if (res != g_resCache.end()) {
        return res->second;
    }
    debug("Resources: load " + cacheKey, 2);

    shared_ptr<ByteArray> data = get(_transientProviders, resRef, type);
    if (!data) {
        data = get(_providers, resRef, type);
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
    if (!data && logNotFound) {
        warn("Resources: not found: " + cacheKey);
    }
    auto pair = g_resCache.insert(make_pair(cacheKey, move(data)));

    return pair.first->second;
}

string Resources::getCacheKey(const string &resRef, resource::ResourceType type) const {
    return str(boost::format("%s.%s") % resRef % getExtByResType(type));
}

shared_ptr<ByteArray> Resources::get(const vector<unique_ptr<IResourceProvider>> &providers, const string &resRef, ResourceType type) {
    for (auto provider = providers.rbegin(); provider != providers.rend(); ++provider) {
        if (!(*provider)->supports(type)) continue;

        shared_ptr<ByteArray> data((*provider)->find(resRef, type));
        if (data) {
            return data;
        }
    }

    return nullptr;
}

shared_ptr<GffStruct> Resources::getGFF(const string &resRef, ResourceType type) {
    string cacheKey(getCacheKey(resRef, type));

    return findResource<GffStruct>(cacheKey, g_gffCache, [this, &resRef, &type]() {
        shared_ptr<ByteArray> data(get(resRef, type));
        shared_ptr<GffStruct> gffs;

        if (data) {
            GffFile gff;
            gff.load(wrap(data));
            gffs = gff.top();
        }

        return move(gffs);
    });
}

shared_ptr<TalkTable> Resources::getTalkTable(const string &resRef) {
    return findResource<TalkTable>(resRef, g_talkTableCache, [this, &resRef]() {
        shared_ptr<ByteArray> data(get(resRef, ResourceType::Conversation));
        shared_ptr<TalkTable> table;

        if (data) {
            TlkFile tlk;
            tlk.load(wrap(data));
            table = tlk.table();
        }

        return move(table);
    });
}

shared_ptr<ByteArray> Resources::getFromExe(uint32_t name, PEResourceType type) {
    return _exeFile.find(name, type);
}

string Resources::getString(int32_t ref) const {
    static string empty;

    shared_ptr<TalkTable> table(_tlkFile.table());
    if (ref == -1 || ref >= table->stringCount()) {
        return empty;
    }

    string text(table->getString(ref).text);
    if (_version == GameVersion::TheSithLords) {
        stripDeveloperNotes(text);
    }

    return move(text);
}

void Resources::stripDeveloperNotes(string &text) const {
    do {
        int openBracketIdx = text.find_first_of('{', 0);
        if (openBracketIdx == -1) break;

        int closeBracketIdx = text.find_first_of('}', openBracketIdx + 1);
        if (closeBracketIdx == -1) break;

        int textLen = static_cast<int>(text.size());
        int noteLen = closeBracketIdx - openBracketIdx + 1;

        for (int i = openBracketIdx; i + noteLen < textLen; ++i) {
            text[i] = text[i + noteLen];
        }

        text.resize(textLen - noteLen);

    } while (true);
}

const vector<string> &Resources::moduleNames() const {
    return _moduleNames;
}

} // namespace resource

} // namespace reone
