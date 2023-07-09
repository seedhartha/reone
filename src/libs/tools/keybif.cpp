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

#include "reone/tools/keybif.h"

#include "reone/resource/typeutil.h"
#include "reone/system/fileutil.h"
#include "reone/system/stream/fileinput.h"

using namespace reone::resource;

namespace reone {

void KeyBifTool::invoke(Operation operation, const std::filesystem::path &input, const std::filesystem::path &outputDir, const std::filesystem::path &gamePath) {
    bool isKey = input.extension() == ".key";
    if (isKey) {
        auto stream = FileInputStream(input);

        KeyReader key(stream);
        key.load();

        listKEY(key);

    } else {
        auto keyPath = getFileIgnoreCase(gamePath, "chitin.key");
        auto key = FileInputStream(keyPath);

        auto keyReader = KeyReader(key);
        keyReader.load();

        int bifIdx = -1;
        for (size_t i = 0; i < keyReader.files().size(); ++i) {
            if (boost::iends_with(keyReader.files()[i].filename, input.filename().string())) {
                bifIdx = static_cast<int>(i);
                break;
            }
        }
        if (bifIdx == -1) {
            return;
        }

        if (operation == Operation::List) {
            listBIF(keyReader, bifIdx);
        } else if (operation == Operation::Extract) {
            extractBIF(keyReader, bifIdx, input, outputDir);
        }
    }
}

void KeyBifTool::listKEY(const KeyReader &key) {
    for (auto &file : key.files()) {
        std::cout << file.filename << " " << file.fileSize << std::endl;
    }
}

void KeyBifTool::listBIF(const KeyReader &key, int bifIdx) {
    for (auto &keyEntry : key.keys()) {
        if (keyEntry.bifIdx == bifIdx) {
            std::cout << keyEntry.resId.string() << std::endl;
        }
    }
}

void KeyBifTool::extractBIF(const KeyReader &key, int bifIdx, const std::filesystem::path &bifPath, const std::filesystem::path &destPath) {
    if (!std::filesystem::exists(destPath)) {
        // Create destination directory if it does not exist
        std::filesystem::create_directory(destPath);
    } else if (!std::filesystem::is_directory(destPath)) {
        // Return if destination exists, but is not a directory
        return;
    }

    auto bif = FileInputStream(bifPath);

    auto bifReader = BifReader(bif);
    bifReader.load();

    auto &bifResources = bifReader.resources();

    for (auto &keyEntry : key.keys()) {
        if (keyEntry.bifIdx != bifIdx) {
            continue;
        }
        debug("Extracting " + keyEntry.resId.string());

        auto &bifResource = bifResources.at(keyEntry.resIdx);
        auto buffer = ByteBuffer(bifResource.fileSize, '\0');
        bif.seek(bifResource.offset, SeekOrigin::Begin);
        bif.read(&buffer[0], buffer.size());

        auto resPath = std::filesystem::path(destPath);
        auto &ext = getExtByResType(keyEntry.resId.type);
        resPath.append(keyEntry.resId.resRef + "." + ext);

        auto out = std::ofstream(resPath, std::ios::binary);
        out.write(&buffer[0], buffer.size());
    }
}

bool KeyBifTool::supports(Operation operation, const std::filesystem::path &input) const {
    if (std::filesystem::is_directory(input))
        return false;

    bool key = input.extension() == ".key";
    bool bif = input.extension() == ".bif";

    if (key) {
        return operation == Operation::List;
    } else if (bif) {
        return operation == Operation::List || operation == Operation::Extract;
    } else {
        return false;
    }
}

} // namespace reone
