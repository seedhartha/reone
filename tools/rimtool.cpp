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

#include "../src/resource/format/rimwriter.h"
#include "../src/resource/typeutil.h"

using namespace std;

using namespace reone::resource;

namespace fs = boost::filesystem;

namespace reone {

namespace tools {

void RimTool::invoke(Operation operation, const fs::path &target, const fs::path &gamePath, const fs::path &destPath) {
    switch (operation) {
        case Operation::List:
        case Operation::Extract: {
            RimReader rim;
            rim.load(target);
            if (operation == Operation::List) {
                list(rim);
            } else if (operation == Operation::Extract) {
                extract(rim, destPath);
            }
            break;
        }
        case Operation::ToRIM:
            toRIM(target);
            break;
        default:
            break;
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

void RimTool::toRIM(const fs::path &target) {
    RimWriter rim;

    for (auto &entry : fs::directory_iterator(target)) {
        fs::path path(entry);
        if (fs::is_directory(path)) continue;

        string ext(path.extension().string());
        ext.erase(0, 1);

        ResourceType resType = getResTypeByExt(ext, false);
        if (resType == ResourceType::Invalid) continue;

        fs::ifstream in(path, ios::binary);
        in.seekg(0, ios::end);
        size_t size = in.tellg();
        ByteArray data(size);
        in.seekg(0);
        in.read(&data[0], size);

        fs::path resRef(path.filename());
        resRef.replace_extension("");

        RimWriter::Resource res;
        res.resRef = resRef.string();
        res.resType = resType;
        res.data = move(data);

        rim.add(move(res));
    }

    fs::path rimPath(target.parent_path());
    rimPath.append(target.filename().string() + ".rim");
    rim.save(rimPath);
}

bool RimTool::supports(Operation operation, const fs::path &target) const {
    switch (operation) {
        case Operation::List:
        case Operation::Extract:
            return !fs::is_directory(target) && target.extension() == ".rim";
        case Operation::ToRIM:
            return fs::is_directory(target);
        default:
            return false;
    }
}

} // namespace tools

} // namespace reone
