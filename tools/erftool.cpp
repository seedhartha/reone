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

#include "../src/resource/format/erfwriter.h"
#include "../src/resource/typeutil.h"

using namespace std;

using namespace reone::resource;

namespace fs = boost::filesystem;

namespace reone {

namespace tools {

void ErfTool::invoke(Operation operation, const fs::path &target, const fs::path &gamePath, const fs::path &destPath) {
    switch (operation) {
        case Operation::List:
        case Operation::Extract: {
            ErfReader erf;
            erf.load(target);
            if (operation == Operation::Extract) {
                extract(erf, destPath);
            } else {
                list(erf);
            }
            break;
        }
        case Operation::ToERF:
        case Operation::ToMOD:
            toERF(operation, target);
            break;
    }
}

void ErfTool::list(const ErfReader &erf) {
    for (auto &key : erf.keys()) {
        cout << key.resRef << " " << getExtByResType(key.resType) << endl;
    }
}

void ErfTool::extract(ErfReader &erf, const fs::path &destPath) {
    if (!fs::exists(destPath)) {
        // Create destination directory if it does not exist
        fs::create_directory(destPath);
    } else if (!fs::is_directory(destPath)) {
        // Return if destination exists, but is not a directory
        return;
    }

    for (size_t i = 0; i < erf.keys().size(); ++i) {
        const ErfReader::Key &key = erf.keys()[i];
        string ext(getExtByResType(key.resType));
        cout << "Extracting " << key.resRef << " " << ext << endl;
        ByteArray data(erf.getResourceData(static_cast<int>(i)));

        fs::path resPath(destPath);
        resPath.append(key.resRef + "." + ext);

        fs::ofstream res(resPath, ios::binary);
        res.write(&data[0], data.size());
    }
}

void ErfTool::toERF(Operation operation, const fs::path &target) {
    ErfWriter erf;

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

        ErfWriter::Resource res;
        res.resRef = resRef.string();
        res.resType = resType;
        res.data = move(data);

        erf.add(move(res));
    }

    ErfWriter::FileType type;
    string ext;

    if (operation == Operation::ToMOD) {
        type = ErfWriter::FileType::MOD;
        ext = ".mod";
    } else {
        type = ErfWriter::FileType::ERF;
        ext = ".erf";
    }

    fs::path erfPath(target.parent_path());
    erfPath.append(target.filename().string() + ext);
    erf.save(type, erfPath);
}

bool ErfTool::supports(Operation operation, const fs::path &target) const {
    switch (operation) {
        case Operation::List:
        case Operation::Extract: {
            string ext(target.extension().string());
            return
                !fs::is_directory(target) &&
                (ext == ".erf" || ext == ".mod" || ext == ".sav");
        }
        case Operation::ToERF:
        case Operation::ToMOD:
            return fs::is_directory(target);

        default:
            return false;
    }
}

} // namespace tools

} // namespace reone
