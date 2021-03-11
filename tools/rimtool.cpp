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

void RimTool::invoke(Operation operation, const fs::path &target, const fs::path &gamePath, const fs::path &destPath) {
    RimReader rim;
    rim.load(target);

    if (operation == Operation::List) {
        list(rim);
    } else if (operation == Operation::Extract) {
        extract(rim, destPath);
    }
}

void RimTool::list(const RimReader &rim) {
    for (auto &res : rim.resources()) {
        cout << res.resRef << " " << getExtByResType(res.resType) << endl;
    }
}

void RimTool::extract(RimReader &rim, const fs::path &destPath) {
    if (!fs::is_directory(destPath) || !fs::exists(destPath)) return;

    for (size_t i = 0; i < rim.resources().size(); ++i) {
        const RimReader::Resource &resEntry = rim.resources()[i];
        string ext(getExtByResType(resEntry.resType));
        cout << "Extracting " << resEntry.resRef << " " << ext << endl;
        ByteArray data(rim.getResourceData(static_cast<int>(i)));

        fs::path resPath(destPath);
        resPath.append(resEntry.resRef + "." + ext);

        fs::ofstream res(resPath, ios::binary);
        res.write(&data[0], data.size());
    }
}

bool RimTool::supports(Operation operation, const fs::path &target) const {
    return
        !fs::is_directory(target) &&
        target.extension() == ".rim" &&
        (operation == Operation::List || operation == Operation::Extract);
}

} // namespace tools

} // namespace reone
