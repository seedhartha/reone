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

#include "keybifprovider.h"

#include <stdexcept>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "../common/pathutil.h"

using namespace std;

namespace fs = boost::filesystem;

namespace reone {

namespace resource {

static const char kKeyFilename[] = "chitin.key";

KeyBifResourceProvider::KeyBifResourceProvider(const fs::path &gamePath) : _gamePath(gamePath) {
}

void KeyBifResourceProvider::init() {
    fs::path path(getPathIgnoreCase(_gamePath, kKeyFilename));
    if (path.empty()) {
        throw runtime_error(str(boost::format("Key file not found: %s %s") % _gamePath % kKeyFilename));
    }
    _keyFile.load(path);
}

shared_ptr<ByteArray> KeyBifResourceProvider::find(const std::string &resRef, ResourceType type) {
    KeyFile::KeyEntry key;
    if (!_keyFile.find(resRef, type, key)) return nullptr;

    shared_ptr<ByteArray> result;

    auto maybeBif = _bifCache.find(key.bifIdx);
    if (maybeBif != _bifCache.end()) {
        result = make_unique<ByteArray>(maybeBif->second->getResourceData(key.resIdx));

    } else {
        string filename(_keyFile.getFilename(key.bifIdx).c_str());
        boost::replace_all(filename, "\\", "/");

        fs::path bifPath(getPathIgnoreCase(_gamePath, filename));
        if (bifPath.empty()) {
            throw runtime_error(str(boost::format("BIF file not found: %s %s") % _gamePath % filename));
        }

        auto bif = make_unique<BifFile>();
        bif->load(bifPath);

        result = make_unique<ByteArray>(bif->getResourceData(key.resIdx));

        _bifCache.insert(make_pair(key.bifIdx, move(bif)));
    }

    return move(result);
}

bool KeyBifResourceProvider::supports(ResourceType type) const {
    return true;
}

} // namespace resource

} // namespace reone
