/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "erf.h"

#include "reone/common/stream/fileinput.h"
#include "reone/resource/format/erfwriter.h"
#include "reone/resource/typeutil.h"

using namespace std;

using namespace reone::resource;

namespace fs = boost::filesystem;

namespace reone {

void ErfTool::invoke(Operation operation, const fs::path &input, const fs::path &outputDir, const fs::path &gamePath) {
    switch (operation) {
    case Operation::List:
    case Operation::Extract: {
        auto stream = FileInputStream(input, OpenMode::Binary);
        ErfReader erf;
        erf.load(stream);
        if (operation == Operation::Extract) {
            extract(erf, input, outputDir);
        } else {
            list(erf);
        }
        break;
    }
    case Operation::ToERF:
    case Operation::ToMOD:
        toERF(operation, input);
        break;
    }
}

void ErfTool::list(const ErfReader &erf) {
    for (auto &key : erf.keys()) {
        cout << key.resId.string() << endl;
    }
}

void ErfTool::extract(ErfReader &erf, const fs::path &erfPath, const fs::path &destPath) {
    if (!fs::exists(destPath)) {
        // Create destination directory if it does not exist
        fs::create_directory(destPath);
    } else if (!fs::is_directory(destPath)) {
        // Return if destination exists, but is not a directory
        return;
    }

    for (size_t i = 0; i < erf.keys().size(); ++i) {
        auto &key = erf.keys()[i];
        auto &erfResource = erf.resources()[i];
        cout << "Extracting " << key.resId.string() << endl;

        auto buffer = ByteArray(erfResource.size, '\0');
        auto erf = FileInputStream(erfPath, OpenMode::Binary);
        erf.seek(erfResource.offset, SeekOrigin::Begin);
        erf.read(&buffer[0], buffer.size());

        auto resPath = destPath;
        auto &ext = getExtByResType(key.resId.type);
        resPath.append(key.resId.resRef + "." + ext);

        auto res = fs::ofstream(resPath, ios::binary);
        res.write(&buffer[0], buffer.size());
    }
}

void ErfTool::toERF(Operation operation, const fs::path &target) {
    ErfWriter erf;

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
        ByteArray data(size, '\0');
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

bool ErfTool::supports(Operation operation, const fs::path &input) const {
    switch (operation) {
    case Operation::List:
    case Operation::Extract: {
        string ext(input.extension().string());
        return !fs::is_directory(input) &&
               (ext == ".erf" || ext == ".mod" || ext == ".sav");
    }
    case Operation::ToERF:
    case Operation::ToMOD:
        return fs::is_directory(input);

    default:
        return false;
    }
}

} // namespace reone
