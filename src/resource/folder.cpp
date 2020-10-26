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

#include "folder.h"

#include <boost/algorithm/string.hpp>

#include "util.h"

using namespace std;

namespace fs = boost::filesystem;

namespace reone {

namespace resource {

void Folder::load(const fs::path &path) {
    if (!fs::is_directory(path)) {
        throw runtime_error("Folder not found: " + path.string());
    }
    loadDirectory(path);
    _path = path;
}

void Folder::loadDirectory(const fs::path &path) {
    for (auto &entry : fs::directory_iterator(path)) {
        const fs::path &childPath = entry.path();
        if (fs::is_directory(childPath)) {
            loadDirectory(childPath);
            continue;
        }

        string resRef(childPath.filename().replace_extension("").string());
        boost::to_lower(resRef);

        string ext(childPath.extension().string().substr(1));
        boost::to_lower(ext);

        Resource res;
        res.path = childPath;
        res.type = getResTypeByExt(ext);

        _resources.insert(make_pair(resRef, res));
    }
}

bool Folder::supports(ResourceType type) const {
    return true;
}

shared_ptr<ByteArray> Folder::find(const string &resRef, ResourceType type) {
    fs::path path;
    for (auto &res : _resources) {
        if (res.first == resRef && res.second.type == type) {
            path = res.second.path;
            break;
        }
    }
    if (path.empty()) {
        return shared_ptr<ByteArray>();
    }
    fs::ifstream in(path, ios::binary);

    in.seekg(0, ios::end);
    size_t size = in.tellg();

    in.seekg(ios::beg);
    ByteArray data(size);
    in.read(&data[0], size);

    return make_shared<ByteArray>(move(data));
}

} // namespace resource

} // namespace reone
