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

namespace fs = boost::filesystem;

namespace reone {

namespace resources {

void Folder::load(const fs::path &path) {
    if (!fs::is_directory(path)) {
        throw std::runtime_error("Folder not found: " + path.string());
    }
    _path = path;

    for (auto &entry : fs::directory_iterator(path)) {
        const fs::path &path2 = entry.path();
        if (fs::is_directory(path2)) continue;

        std::string resRef(path2.filename().replace_extension("").string());
        boost::to_lower(resRef);

        std::string ext(path2.extension().string().substr(1));

        Resource res;
        res.path = path2;
        res.type = getResTypeByExt(ext);

        _resources.insert(std::make_pair(resRef, res));
    }
}

bool Folder::supports(ResourceType type) const {
    return true;
}

std::shared_ptr<ByteArray> Folder::find(const std::string &resRef, ResourceType type) {
    fs::path path;
    for (auto &res : _resources) {
        if (res.first == resRef && res.second.type == type) {
            path = res.second.path;
            break;
        }
    }
    if (path.empty()) {
        return std::shared_ptr<ByteArray>();
    }
    fs::ifstream in(path, std::ios::binary);

    in.seekg(0, std::ios::end);
    size_t size = in.tellg();

    in.seekg(std::ios::beg);
    ByteArray data(size);
    in.read(&data[0], size);

    return std::make_shared<ByteArray>(std::move(data));
}

} // namespace resources

} // namespace reone
