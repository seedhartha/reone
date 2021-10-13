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

#include "rim.h"

#include "../../engine/resource/format/rimwriter.h"
#include "../../engine/resource/typeutil.h"

using namespace std;

using namespace reone::resource;

namespace fs = boost::filesystem;

namespace reone {

namespace tools {

void RimTool::invoke(Operation operation, const fs::path &target, const fs::path &gamePath, const fs::path &destPath) {
    switch (operation) {
    case Operation::List:
    case Operation::Extract: {
        RimReader rim(0);
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
        cout << res.resId.string() << endl;
    }
}

void RimTool::extract(RimReader &rim, const fs::path &destPath) {
    if (!fs::exists(destPath)) {
        // Create destination directory if it does not exist
        fs::create_directory(destPath);
    } else if (!fs::is_directory(destPath)) {
        // Return if destination exists, but is not a directory
        return;
    }

    for (size_t i = 0; i < rim.resources().size(); ++i) {
        const RimReader::ResourceEntry &resEntry = rim.resources()[i];
        cout << "Extracting " << resEntry.resId.string() << endl;
        string ext(getExtByResType(resEntry.resId.type));
        ByteArray data(rim.getResourceData(static_cast<int>(i)));

        fs::path resPath(destPath);
        resPath.append(resEntry.resId.resRef + "." + ext);

        fs::ofstream res(resPath, ios::binary);
        res.write(&data[0], data.size());
    }
}

void RimTool::toRIM(const fs::path &target) {
    RimWriter rim;

    for (auto &entry : fs::directory_iterator(target)) {
        fs::path path(entry);
        if (fs::is_directory(path))
            continue;

        string ext(path.extension().string());
        ext.erase(0, 1);

        ResourceType resType = getResTypeByExt(ext, false);
        if (resType == ResourceType::Invalid)
            continue;

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
