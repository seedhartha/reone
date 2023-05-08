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

#include "reone/tools/tool/rim.h"

#include "reone/common/stream/fileinput.h"
#include "reone/resource/format/rimwriter.h"
#include "reone/resource/typeutil.h"

using namespace std;

using namespace reone::resource;

namespace fs = boost::filesystem;

namespace reone {

void RimTool::invoke(Operation operation, const fs::path &input, const fs::path &outputDir, const fs::path &gamePath) {
    switch (operation) {
    case Operation::List:
    case Operation::Extract: {
        auto rim = FileInputStream(input, OpenMode::Binary);
        auto rimReader = RimReader();
        rimReader.load(rim);
        if (operation == Operation::List) {
            list(rimReader);
        } else if (operation == Operation::Extract) {
            extract(rimReader, input, outputDir);
        }
        break;
    }
    case Operation::ToRIM:
        toRIM(input);
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

void RimTool::extract(RimReader &rim, const fs::path &rimPath, const fs::path &destPath) {
    if (!fs::exists(destPath)) {
        // Create destination directory if it does not exist
        fs::create_directory(destPath);
    } else if (!fs::is_directory(destPath)) {
        // Return if destination exists, but is not a directory
        return;
    }

    for (size_t i = 0; i < rim.resources().size(); ++i) {
        auto &rimResource = rim.resources()[i];
        cout << "Extracting " << rimResource.resId.string() << endl;

        auto buffer = ByteArray(rimResource.size, '\0');
        auto rim = FileInputStream(rimPath, OpenMode::Binary);
        rim.seek(rimResource.offset, SeekOrigin::Begin);
        rim.read(&buffer[0], buffer.size());

        auto resPath = destPath;
        auto &ext = getExtByResType(rimResource.resId.type);
        resPath.append(rimResource.resId.resRef + "." + ext);

        auto res = fs::ofstream(resPath, ios::binary);
        res.write(&buffer[0], buffer.size());
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
        ByteArray data(size, '\0');
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

bool RimTool::supports(Operation operation, const fs::path &input) const {
    switch (operation) {
    case Operation::List:
    case Operation::Extract:
        return !fs::is_directory(input) && input.extension() == ".rim";
    case Operation::ToRIM:
        return fs::is_directory(input);
    default:
        return false;
    }
}

} // namespace reone
