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

#include "resources.h"

#include "../common/log.h"
#include "../common/pathutil.h"
#include "../common/streamutil.h"

#include "2da.h"
#include "gffstruct.h"
#include "format/2dareader.h"
#include "format/bifreader.h"
#include "format/erfreader.h"
#include "format/gffreader.h"
#include "format/rimreader.h"
#include "folder.h"
#include "keybifprovider.h"
#include "resourceprovider.h"
#include "typeutil.h"

using namespace std;

namespace fs = boost::filesystem;

namespace reone {

namespace resource {

void Resources::indexKeyFile(const fs::path &path) {
    if (!fs::exists(path)) return;

    auto keyBif = make_unique<KeyBifResourceProvider>();
    keyBif->init(path);

    _providers.push_back(move(keyBif));

    debug("Index " + path.string(), LogChannels::resources);
}

void Resources::indexErfFile(const fs::path &path, bool transient) {
    if (!fs::exists(path)) return;

    auto erf = make_unique<ErfReader>();
    erf->load(path);

    if (transient) {
        _transientProviders.push_back(move(erf));
    } else {
        _providers.push_back(move(erf));
    }

    debug("Index " + path.string(), LogChannels::resources);
}

void Resources::indexRimFile(const fs::path &path, bool transient) {
    if (!fs::exists(path)) return;

    auto rim = make_unique<RimReader>();
    rim->load(path);

    if (transient) {
        _transientProviders.push_back(move(rim));
    } else {
        _providers.push_back(move(rim));
    }

    debug("Index " + path.string(), LogChannels::resources);
}

void Resources::indexDirectory(const fs::path &path) {
    if (!fs::exists(path)) return;

    auto folder = make_unique<Folder>();
    folder->load(path);

    _providers.push_back(move(folder));

    debug("Index " + path.string(), LogChannels::resources);
}

void Resources::indexExeFile(const fs::path &path) {
    if (!fs::exists(path)) return;

    _exeFile.load(path);

    debug("Index " + path.string(), LogChannels::resources);
}

void Resources::invalidateCache() {
    _rawCache.clear();
    _2daCache.clear();
    _gffCache.clear();
}

void Resources::clearTransientProviders() {
    _transientProviders.clear();
}

template <class T>
static shared_ptr<T> getResource(const string &key, unordered_map<string, shared_ptr<T>> &cache, const function<shared_ptr<T>()> &getter) {
    auto maybeResource = cache.find(key);
    if (maybeResource != cache.end()) return maybeResource->second;

    auto inserted = cache.insert(make_pair(key, getter()));
    return inserted.first->second;
}

shared_ptr<ByteArray> Resources::getRaw(const string &resRef, ResourceType type, bool logNotFound) {
    if (resRef.empty()) return nullptr;

    string cacheKey(getCacheKey(resRef, type));
    auto res = _rawCache.find(cacheKey);
    if (res != _rawCache.end()) return res->second;

    shared_ptr<ByteArray> data = doGetRaw(_transientProviders, resRef, type);
    if (!data) {
        data = doGetRaw(_providers, resRef, type);
    }
    if (!data && logNotFound) {
        warn("Not found: " + cacheKey, LogChannels::resources);
    }
    auto pair = _rawCache.insert(make_pair(cacheKey, move(data)));

    return pair.first->second;
}

string Resources::getCacheKey(const string &resRef, ResourceType type) const {
    return str(boost::format("%s.%s") % resRef % getExtByResType(type));
}

shared_ptr<TwoDA> Resources::get2DA(const string &resRef, bool logNotFound) {
    return getResource<TwoDA>(resRef, _2daCache, [&]() {
        shared_ptr<ByteArray> data(getRaw(resRef, ResourceType::TwoDa, logNotFound));
        shared_ptr<TwoDA> twoDa;

        if (data) {
            TwoDaReader file;
            file.load(wrap(data));
            twoDa = file.twoDa();
        }

        return move(twoDa);
    });
}

shared_ptr<ByteArray> Resources::doGetRaw(const vector<unique_ptr<IResourceProvider>> &providers, const string &resRef, ResourceType type) {
    for (auto provider = providers.rbegin(); provider != providers.rend(); ++provider) {
        if (!(*provider)->supports(type)) continue;

        shared_ptr<ByteArray> data((*provider)->find(resRef, type));
        if (data) return data;
    }

    return nullptr;
}

shared_ptr<GffStruct> Resources::getGFF(const string &resRef, ResourceType type) {
    string cacheKey(getCacheKey(resRef, type));

    return getResource<GffStruct>(cacheKey, _gffCache, [this, &resRef, &type]() {
        shared_ptr<ByteArray> data(getRaw(resRef, type));
        shared_ptr<GffStruct> gffs;

        if (data) {
            GffReader gff;
            gff.load(wrap(data));
            gffs = gff.root();
        }

        return move(gffs);
    });
}

shared_ptr<ByteArray> Resources::getFromExe(uint32_t name, PEResourceType type) {
    return _exeFile.find(name, type);
}

} // namespace resource

} // namespace reone
