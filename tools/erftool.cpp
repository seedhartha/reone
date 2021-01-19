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

#include "tools.h"

#include <boost/format.hpp>

#include "../src/common/log.h"
#include "../src/resource/erffile.h"
#include "../src/resource/typeutil.h"

using namespace std;

using namespace reone::resource;

namespace fs = boost::filesystem;

namespace reone {

namespace tools {

void ErfTool::list(const fs::path &path, const fs::path &keyPath) const {
    ErfFile erf;
    erf.load(path);

    for (auto &key : erf.keys()) {
        info(boost::format("%16s\t%4s") % key.resRef % getExtByResType(key.resType));
    }
}

void ErfTool::extract(const fs::path &path, const fs::path &keyPath, const fs::path &destPath) const {
    ErfFile erf;
    erf.load(path);

    auto &keys = erf.keys();
    for (int i = 0; i < keys.size(); ++i) {
        const ErfFile::Key &key = keys[i];
        info(boost::format("Extracting %16s\t%4s") % key.resRef % getExtByResType(key.resType));

        fs::path resPath(destPath);
        resPath.append(key.resRef + "." + getExtByResType(key.resType));

        ByteArray data(erf.getResourceData(i));
        fs::ofstream res(resPath, ios::binary);
        res.write(data.data(), data.size());
    }
}

} // namespace tools

} // namespace reone
