/*
 * Copyright (c) 2020 The reone project contributors
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

#include "tools.h"

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "../src/common/log.h"
#include "../src/resource/biffile.h"
#include "../src/resource/util.h"

using namespace std;

using namespace reone::resource;

namespace fs = boost::filesystem;

namespace reone {

namespace tools {

void BifTool::list(const fs::path &path, const fs::path &keyPath) const {
    if (!fs::exists(keyPath)) {
        throw runtime_error("BIF: key file does not exist: " + keyPath.string());
    }
    KeyFile key;
    key.load(keyPath);

    auto &files = key.files();
    string filename(path.filename().string());
    int bifIdx = getFileIndexByFilename(files, filename);
    if (bifIdx == -1) {
        throw runtime_error("BIF: filename not found in Key file: " + filename);
    }

    for (auto &key : key.keys()) {
        if (key.bifIdx == bifIdx) {
            info(boost::format("%16s\t%4s") % key.resRef % getExtByResType(key.resType));
        }
    }
}

int BifTool::getFileIndexByFilename(const vector<KeyFile::FileEntry> &files, const string &filename) const {
    for (int i = 0; i < files.size(); ++i) {
        if (boost::contains(files[i].filename, filename)) {
            return i;
        }
    }
    return -1;
}

void BifTool::extract(const fs::path &path, const fs::path &keyPath, const fs::path &destPath) const {
    if (!fs::exists(keyPath)) {
        throw runtime_error("BIF: key file does not exist: " + keyPath.string());
    }
    KeyFile key;
    key.load(keyPath);

    auto &files = key.files();
    string filename(path.filename().string());
    int bifIdx = getFileIndexByFilename(files, filename);
    if (bifIdx == -1) {
        throw runtime_error("BIF: filename not found in Key file: " + filename);
    }
    BifFile bif;
    bif.load(path);

    for (auto &key : key.keys()) {
        if (key.bifIdx != bifIdx) continue;
        info(boost::format("Extracting %16s\t%4s") % key.resRef % getExtByResType(key.resType));

        fs::path resPath(destPath);
        resPath.append(key.resRef + "." + getExtByResType(key.resType));

        ByteArray data(bif.getResourceData(key.resIdx));
        fs::ofstream res(resPath, ios::binary);
        res.write(data.data(), data.size());
    }
}

} // namespace tools

} // namespace reone
