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

#include "keybif.h"

#include "../../common/pathutil.h"
#include "../../common/stream/fileinput.h"
#include "../../resource/typeutil.h"

using namespace std;

using namespace reone::resource;

namespace fs = boost::filesystem;

namespace reone {

void KeyBifTool::invoke(Operation operation, const fs::path &input, const fs::path &outputDir, const fs::path &gamePath) {
    bool isKey = input.extension() == ".key";
    if (isKey) {
        auto stream = FileInputStream(input, OpenMode::Binary);

        KeyReader key;
        key.load(stream);

        listKEY(key);

    } else {
        auto keyPath = getPathIgnoreCase(gamePath, "chitin.key");
        auto key = FileInputStream(keyPath, OpenMode::Binary);

        auto keyReader = KeyReader();
        keyReader.load(key);

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
        cout << file.filename << " " << file.fileSize << endl;
    }
}

void KeyBifTool::listBIF(const KeyReader &key, int bifIdx) {
    for (auto &keyEntry : key.keys()) {
        if (keyEntry.bifIdx == bifIdx) {
            cout << keyEntry.resId.string() << endl;
        }
    }
}

void KeyBifTool::extractBIF(const KeyReader &key, int bifIdx, const fs::path &bifPath, const fs::path &destPath) {
    if (!fs::exists(destPath)) {
        // Create destination directory if it does not exist
        fs::create_directory(destPath);
    } else if (!fs::is_directory(destPath)) {
        // Return if destination exists, but is not a directory
        return;
    }

    auto bif = FileInputStream(bifPath, OpenMode::Binary);

    auto bifReader = BifReader();
    bifReader.load(bif);

    auto &bifResources = bifReader.resources();

    for (auto &keyEntry : key.keys()) {
        if (keyEntry.bifIdx != bifIdx) {
            continue;
        }
        cout << "Extracting " + keyEntry.resId.string() << endl;

        auto &bifResource = bifResources.at(keyEntry.resIdx);
        auto buffer = ByteArray(bifResource.fileSize, '\0');
        bif.seek(bifResource.offset, SeekOrigin::Begin);
        bif.read(&buffer[0], buffer.size());

        auto resPath = fs::path(destPath);
        auto &ext = getExtByResType(keyEntry.resId.type);
        resPath.append(keyEntry.resId.resRef + "." + ext);

        auto out = fs::ofstream(resPath, ios::binary);
        out.write(&buffer[0], buffer.size());
    }
}

bool KeyBifTool::supports(Operation operation, const fs::path &input) const {
    if (fs::is_directory(input))
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
