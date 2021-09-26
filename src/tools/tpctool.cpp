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

#include "s3tc.h"

#include "../engine/common/logutil.h"
#include "../engine/graphics/texture/tgawriter.h"
#include "../engine/graphics/texture/tpcreader.h"

using namespace std;

using namespace reone::graphics;

namespace fs = boost::filesystem;

namespace reone {

namespace tools {

void TpcTool::invoke(Operation operation, const fs::path &target, const fs::path &gamePath, const fs::path &destPath) {
    if (operation == Operation::ToTGA) {
        toTGA(target, destPath);
    }
}

void TpcTool::toTGA(const fs::path &path, const fs::path &destPath) {
    // Read TPC

    TpcReader tpc("", TextureUsage::GUI, true);
    tpc.load(path);

    // Write TGA

    fs::path tgaPath(destPath);
    tgaPath.append(path.filename().string());
    tgaPath.replace_extension("tga");

    TgaWriter writer(tpc.texture());
    writer.save(tgaPath, true);

    // Write TXI

    if (!tpc.txiData().empty()) {
        fs::path txiPath(tgaPath);
        txiPath.replace_extension("txi");

        fs::ofstream txi(txiPath, ios::binary);
        txi.write(tpc.txiData().data(), tpc.txiData().size());
    }
}

bool TpcTool::supports(Operation operation, const fs::path &target) const {
    return !fs::is_directory(target) &&
           target.extension() == ".tpc" &&
           operation == Operation::ToTGA;
}

} // namespace tools

} // namespace reone
