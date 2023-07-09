/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/tools/rim.h"

#include "reone/resource/format/rimwriter.h"
#include "reone/resource/typeutil.h"
#include "reone/system/stream/fileinput.h"

using namespace reone::resource;

namespace reone {

void RimTool::invoke(Operation operation, const boost::filesystem::path &input, const boost::filesystem::path &outputDir, const boost::filesystem::path &gamePath) {
    switch (operation) {
    case Operation::List:
    case Operation::Extract: {
        auto rim = FileInputStream(input);
        auto rimReader = RimReader(rim);
        rimReader.load();
        if (operation == Operation::List) {
            list(rimReader);
        } else if (operation == Operation::Extract) {
            extract(rimReader, input, outputDir);
        }
        break;
    }
    case Operation::ToRIM:
        toRIM(input, outputDir);
        break;
    default:
        break;
    }
}

void RimTool::list(const RimReader &rim) {
    for (auto &res : rim.resources()) {
        std::cout << res.resId.string() << std::endl;
    }
}

void RimTool::extract(RimReader &rim, const boost::filesystem::path &rimPath, const boost::filesystem::path &destPath) {
    if (!boost::filesystem::exists(destPath)) {
        // Create destination directory if it does not exist
        boost::filesystem::create_directory(destPath);
    } else if (!boost::filesystem::is_directory(destPath)) {
        // Return if destination exists, but is not a directory
        return;
    }

    for (size_t i = 0; i < rim.resources().size(); ++i) {
        auto &rimResource = rim.resources()[i];
        debug("Extracting " + rimResource.resId.string());

        auto buffer = ByteBuffer(rimResource.size, '\0');
        auto rim = FileInputStream(rimPath);
        rim.seek(rimResource.offset, SeekOrigin::Begin);
        rim.read(&buffer[0], buffer.size());

        auto resPath = destPath;
        auto &ext = getExtByResType(rimResource.resId.type);
        resPath.append(rimResource.resId.resRef + "." + ext);

        auto res = boost::filesystem::ofstream(resPath, std::ios::binary);
        res.write(&buffer[0], buffer.size());
    }
}

void RimTool::toRIM(const boost::filesystem::path &target, const boost::filesystem::path &destPath) {
    RimWriter rim;

    for (auto &entry : boost::filesystem::directory_iterator(target)) {
        boost::filesystem::path path(entry);
        if (boost::filesystem::is_directory(path))
            continue;

        std::string ext(path.extension().string());
        ext.erase(0, 1);

        ResourceType resType = getResTypeByExt(ext, false);
        if (resType == ResourceType::Invalid)
            continue;

        boost::filesystem::ifstream in(path, std::ios::binary);
        in.seekg(0, std::ios::end);
        size_t size = in.tellg();
        ByteBuffer data(size, '\0');
        in.seekg(0);
        in.read(&data[0], size);

        boost::filesystem::path resRef(path.filename());
        resRef.replace_extension("");

        RimWriter::Resource res;
        res.resRef = resRef.string();
        res.resType = resType;
        res.data = std::move(data);

        rim.add(std::move(res));
    }

    auto rimPath = destPath;
    rimPath.append(target.filename().string() + ".rim");
    rim.save(rimPath);
}

bool RimTool::supports(Operation operation, const boost::filesystem::path &input) const {
    switch (operation) {
    case Operation::List:
    case Operation::Extract:
        return !boost::filesystem::is_directory(input) && input.extension() == ".rim";
    case Operation::ToRIM:
        return boost::filesystem::is_directory(input);
    default:
        return false;
    }
}

} // namespace reone
