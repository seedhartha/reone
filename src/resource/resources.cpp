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

#include "audio/mp3file.h"
#include "audio/wavfile.h"
#include "bwmfile.h"
#include "collection/biffile.h"
#include "collection/erffile.h"
#include "collection/folder.h"
#include "collection/rimfile.h"
#include "image/curfile.h"
#include "image/tgafile.h"
#include "image/tpcfile.h"
#include "mdlfile.h"
#include "ncsfile.h"
#include "util.h"

using namespace std;

using namespace reone::audio;
using namespace reone::render;
using namespace reone::script;

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

static map<string, uint32_t> g_cursorNameByResRefKotor = {
    { "gui_mp_defaultu", 4 },
    { "gui_mp_defaultd", 5 }
};

static map<string, uint32_t> g_cursorNameByResRefTsl = {
    { "gui_mp_defaultu", 3 },
    { "gui_mp_defaultd", 4 }
};

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
static map<string, shared_ptr<CreatureBlueprint>> g_utcCache;
static map<string, shared_ptr<DoorBlueprint>> g_utdCache;
static map<string, shared_ptr<ItemBlueprint>> g_utiCache;
static map<string, shared_ptr<PlaceableBlueprint>> g_utpCache;
static map<string, shared_ptr<TriggerBlueprint>> g_uttCache;
static map<string, shared_ptr<WaypointBlueprint>> g_utwCache;

static map<string, string> g_fontOverride = {
    { "fnt_d16x16", "fnt_d16x16b" }
};

ResourceManager &ResourceManager::instance() {
    static ResourceManager instance;
    return instance;
}

void ResourceManager::init(GameVersion version, const fs::path &gamePath) {
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

void ResourceManager::indexKeyFile() {
    fs::path path(getPathIgnoreCase(_gamePath, kKeyFileName));

    if (path.empty()) {
        throw runtime_error(str(boost::format("Key file not found: %s %s") % _gamePath % kKeyFileName));
    }
    _keyFile.load(path);

    debug(boost::format("Resources: indexed: %s") % path);
}

void ResourceManager::indexTexturePacks() {
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

void ResourceManager::indexErfFile(const fs::path &path) {
    unique_ptr<ErfFile> erf(new ErfFile());
    erf->load(path);

    _providers.push_back(move(erf));

    debug(boost::format("Resources: indexed: %s") % path);
}

void ResourceManager::indexAudioFiles() {
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

void ResourceManager::indexDirectory(const fs::path &path) {
    unique_ptr<Folder> folder(new Folder());
    folder->load(path);

    _providers.push_back(move(folder));

    debug(boost::format("Resources: indexed: %s") % path);
}

void ResourceManager::indexOverrideDirectory() {
    fs::path path(getPathIgnoreCase(_gamePath, kOverrideDirectoryName));
    indexDirectory(path);
}

void ResourceManager::indexTalkTable() {
    fs::path path(getPathIgnoreCase(_gamePath, kTalkTableFileName));
    _tlkFile.load(path);

    debug(boost::format("Resources: indexed: %s") % path);
}

void ResourceManager::indexExeFile() {
    string filename(_version == GameVersion::TheSithLords ? kExeFileNameTsl : kExeFileNameKotor);
    fs::path path(getPathIgnoreCase(_gamePath, filename));

    _exeFile.load(path);

    debug(boost::format("Resources: indexed: %s") % path);
}

void ResourceManager::loadModuleNames() {
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
    g_utcCache.clear();
    g_utdCache.clear();
    g_utiCache.clear();
    g_utpCache.clear();
    g_uttCache.clear();
    g_utwCache.clear();
}

void ResourceManager::loadModule(const string &name) {
    _transientProviders.clear();
    clearCaches();

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

void ResourceManager::indexTransientRimFile(const fs::path &path) {
    unique_ptr<RimFile> rim(new RimFile());
    rim->load(path);

    _transientProviders.push_back(move(rim));

    debug(boost::format("Resources: indexed: %s") % path);
}

void ResourceManager::indexTransientErfFile(const fs::path &path) {
    unique_ptr<ErfFile> erf(new ErfFile());
    erf->load(path);

    _transientProviders.push_back(move(erf));

    debug(boost::format("Resources: indexed: %s") % path);
}

template <class T>
static shared_ptr<T> findResource(const string &key, map<string, shared_ptr<T>> &cache, const function<shared_ptr<T>()> &getter) {
    auto res = cache.find(key);
    if (res != cache.end()) {
        return res->second;
    };
    auto pair = cache.insert(make_pair(key, getter()));

    return pair.first->second;
}

shared_ptr<TwoDaTable> ResourceManager::find2DA(const string &resRef) {
    return findResource<TwoDaTable>(resRef, g_2daCache, [this, &resRef]() {
        shared_ptr<ByteArray> data(findRaw(resRef, ResourceType::TwoDa));
        shared_ptr<TwoDaTable> table;

        if (data) {
            TwoDaFile file;
            file.load(wrap(data));
            table = file.table();
        }

        return move(table);
    });
}

shared_ptr<ByteArray> ResourceManager::findRaw(const string &resRef, ResourceType type, bool logNotFound) {
    string cacheKey(getCacheKey(resRef, type));
    auto res = g_resCache.find(cacheKey);

    if (res != g_resCache.end()) {
        return res->second;
    }
    debug("Resources: load " + cacheKey, 2);

    shared_ptr<ByteArray> data = findRaw(_transientProviders, resRef, type);
    if (!data) {
        data = findRaw(_providers, resRef, type);
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

string ResourceManager::getCacheKey(const string &resRef, resource::ResourceType type) const {
    return str(boost::format("%s.%s") % resRef % getExtByResType(type));
}

shared_ptr<ByteArray> ResourceManager::findRaw(const vector<unique_ptr<IResourceProvider>> &providers, const string &resRef, ResourceType type) {
    for (auto provider = providers.rbegin(); provider != providers.rend(); ++provider) {
        if (!(*provider)->supports(type)) continue;

        shared_ptr<ByteArray> data((*provider)->find(resRef, type));
        if (data) {
            return data;
        }
    }

    return nullptr;
}

shared_ptr<GffStruct> ResourceManager::findGFF(const string &resRef, ResourceType type) {
    string cacheKey(getCacheKey(resRef, type));

    return findResource<GffStruct>(cacheKey, g_gffCache, [this, &resRef, &type]() {
        shared_ptr<ByteArray> data(findRaw(resRef, type));
        shared_ptr<GffStruct> gffs;

        if (data) {
            GffFile gff;
            gff.load(wrap(data));
            gffs = gff.top();
        }

        return move(gffs);
    });
}

shared_ptr<TalkTable> ResourceManager::findTalkTable(const string &resRef) {
    return findResource<TalkTable>(resRef, g_talkTableCache, [this, &resRef]() {
        shared_ptr<ByteArray> data(findRaw(resRef, ResourceType::Conversation));
        shared_ptr<TalkTable> table;

        if (data) {
            TlkFile tlk;
            tlk.load(wrap(data));
            table = tlk.table();
        }

        return move(table);
    });
}

shared_ptr<AudioStream> ResourceManager::findAudio(const string &resRef) {
    return findResource<AudioStream>(resRef, g_audioCache, [this, &resRef]() {
        shared_ptr<ByteArray> mp3Data(findRaw(resRef, ResourceType::Mp3, false));
        shared_ptr<AudioStream> stream;

        if (mp3Data) {
            Mp3File mp3;
            mp3.load(wrap(mp3Data));
            stream = mp3.stream();

        } else {
            shared_ptr<ByteArray> wavData(findRaw(resRef, ResourceType::Wav));
            if (wavData) {
                WavFile wav;
                wav.load(wrap(wavData));
                stream = wav.stream();
            }
        }

        return move(stream);
    });
}

shared_ptr<Model> ResourceManager::findModel(const string &resRef) {
    return findResource<Model>(resRef, g_modelCache, [this, &resRef]() {
        shared_ptr<ByteArray> mdlData(findRaw(resRef, ResourceType::Model));
        shared_ptr<ByteArray> mdxData(findRaw(resRef, ResourceType::Mdx));
        shared_ptr<Model> model;

        if (mdlData && mdxData) {
            MdlFile mdl(_version);
            mdl.load(wrap(mdlData), wrap(mdxData));
            model = mdl.model();
            if (model) {
                model->initGL();
            }
        }

        return move(model);
    });
}

shared_ptr<Walkmesh> ResourceManager::findWalkmesh(const string &resRef, ResourceType type) {
    return findResource<Walkmesh>(resRef, g_walkmeshCache, [this, &resRef, &type]() {
        shared_ptr<ByteArray> data(findRaw(resRef, type));
        shared_ptr<Walkmesh> walkmesh;

        if (data) {
            BwmFile bwm;
            bwm.load(wrap(data));
            walkmesh = bwm.walkmesh();
        }

        return move(walkmesh);
    });
}

shared_ptr<Texture> ResourceManager::findTexture(const string &resRef, TextureType type) {
    return findResource<Texture>(resRef, g_texCache, [this, &resRef, &type]() {
        bool tryCur = type == TextureType::Cursor;
        bool tryTpc = _version == GameVersion::TheSithLords || type != TextureType::Lightmap;
        shared_ptr<Texture> texture;

        if (tryCur) {
            uint32_t name;
            switch (_version) {
                case GameVersion::TheSithLords:
                    name = g_cursorNameByResRefTsl.find(resRef)->second;
                    break;
                default:
                    name = g_cursorNameByResRefKotor.find(resRef)->second;
                    break;
            }
            shared_ptr<ByteArray> curData(_exeFile.find(name, PEResourceType::Cursor));
            if (curData) {
                CurFile cur(resRef);
                cur.load(wrap(curData));
                texture = cur.texture();
            }
        }
        if (!texture && tryTpc) {
            shared_ptr<ByteArray> tpcData(findRaw(resRef, ResourceType::Texture));
            if (tpcData) {
                TpcFile tpc(resRef, type);
                tpc.load(wrap(tpcData));
                texture = tpc.texture();
            }
        }
        if (!texture) {
            shared_ptr<ByteArray> tgaData(findRaw(resRef, ResourceType::Tga));
            if (tgaData) {
                TgaFile tga(resRef, type);
                tga.load(wrap(tgaData));
                texture = tga.texture();
            }
        }
        if (texture) {
            texture->initGL();
        }

        return move(texture);
    });
}

shared_ptr<Font> ResourceManager::findFont(const string &resRef) {
    auto fontOverride = g_fontOverride.find(resRef);
    const string &finalResRef = fontOverride != g_fontOverride.end() ? fontOverride->second : resRef;

    return findResource<Font>(finalResRef, g_fontCache, [this, &finalResRef]() {
        shared_ptr<Texture> texture(findTexture(finalResRef, TextureType::GUI));
        shared_ptr<Font> font;

        if (texture) {
            font = make_shared<Font>();
            font->load(texture);
            if (font) {
                font->initGL();
            }
        }

        return move(font);
    });
}

shared_ptr<ScriptProgram> ResourceManager::findScript(const string &resRef) {
    return findResource<ScriptProgram>(resRef, g_scripts, [this, &resRef]() {
        shared_ptr<ByteArray> data(findRaw(resRef, ResourceType::CompiledScript));
        shared_ptr<ScriptProgram> program;

        if (data) {
            NcsFile ncs(resRef);
            ncs.load(wrap(data));
            program = ncs.program();
        }

        return move(program);
    });
}

shared_ptr<CreatureBlueprint> ResourceManager::findCreatureBlueprint(const string &resRef) {
    return findBlueprint<CreatureBlueprint>(resRef, ResourceType::CreatureBlueprint, g_utcCache);
}

template <class T>
shared_ptr<T> ResourceManager::findBlueprint(const string &resRef, ResourceType type, map<string, shared_ptr<T>> &cache) {
    return findResource<T>(resRef, cache, [this, &resRef, &type]() {
        shared_ptr<GffStruct> data(findGFF(resRef, type));
        shared_ptr<T> blueprint;

        if (data) {
            blueprint.reset(new T());
            blueprint->load(*data);
        }

        return move(blueprint);
    });
}

shared_ptr<DoorBlueprint> ResourceManager::findDoorBlueprint(const string &resRef) {
    return findBlueprint<DoorBlueprint>(resRef, ResourceType::DoorBlueprint, g_utdCache);
}

shared_ptr<ItemBlueprint> ResourceManager::findItemBlueprint(const string &resRef) {
    return findBlueprint<ItemBlueprint>(resRef, ResourceType::ItemBlueprint, g_utiCache);
}

shared_ptr<PlaceableBlueprint> ResourceManager::findPlaceableBlueprint(const string &resRef) {
    return findBlueprint<PlaceableBlueprint>(resRef, ResourceType::PlaceableBlueprint, g_utpCache);
}

shared_ptr<TriggerBlueprint> ResourceManager::findTriggerBlueprint(const string &resRef) {
    return findBlueprint<TriggerBlueprint>(resRef, ResourceType::TriggerBlueprint, g_uttCache);
}

shared_ptr<WaypointBlueprint> ResourceManager::findWaypointBlueprint(const string &resRef) {
    return findBlueprint<WaypointBlueprint>(resRef, ResourceType::WaypointBlueprint, g_utwCache);
}

const TalkTableString &ResourceManager::getString(int32_t ref) const {
    return _tlkFile.table()->getString(ref);
}

const vector<string> &ResourceManager::moduleNames() const {
    return _moduleNames;
}

} // namespace resources

} // namespace reone
