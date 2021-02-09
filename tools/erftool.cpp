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

#include <iostream>

#include "../src/resource/typeutil.h"

using namespace std;

using namespace reone::resource;

namespace fs = boost::filesystem;

namespace reone {

namespace tools {

void ErfTool::invoke(Operation operation, const fs::path &target, const fs::path &gamePath, const fs::path &destPath) {
    ErfFile erf;
    erf.load(target);

    if (operation == Operation::List) {
        list(erf);
    } else if (operation == Operation::Extract) {
        extract(erf, destPath);
    }
}

void ErfTool::list(const ErfFile &erf) {
    for (auto &key : erf.keys()) {
        cout << key.resRef << " " << getExtByResType(key.resType) << endl;
    }
}

void ErfTool::extract(ErfFile &erf, const fs::path &destPath) {
    if (!fs::is_directory(destPath) || !fs::exists(destPath)) return;

    for (size_t i = 0; i < erf.keys().size(); ++i) {
        const ErfFile::Key &key = erf.keys()[i];
        string ext(getExtByResType(key.resType));
        cout << "Extracting " << key.resRef << " " << ext << endl;
        ByteArray data(erf.getResourceData(i));

        fs::path resPath(destPath);
        resPath.append(key.resRef + "." + ext);

        fs::ofstream res(resPath, ios::binary);
        res.write(&data[0], data.size());
    }
}

bool ErfTool::supports(Operation operation, const fs::path &target) const {
    if (fs::is_directory(target)) return false;

    string ext(target.extension().string());
    if (ext != ".erf" && ext != ".mod" && ext != ".sav") return false;

    return operation == Operation::List || operation == Operation::Extract;
}

} // namespace tools

} // namespace reone
