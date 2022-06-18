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

#include "tpc.h"

#include "../../common/stream/fileinput.h"
#include "../../graphics/format/tgawriter.h"
#include "../../graphics/format/tpcreader.h"

using namespace std;

using namespace reone::graphics;

namespace fs = boost::filesystem;

namespace reone {

void TpcTool::invoke(Operation operation, const fs::path &input, const fs::path &outputDir, const fs::path &gamePath) {
    if (operation == Operation::ToTGA) {
        toTGA(input, outputDir);
    }
}

void TpcTool::toTGA(const fs::path &path, const fs::path &destPath) {
    // Read TPC

    auto stream = FileInputStream(path, OpenMode::Binary);

    TpcReader tpc("", TextureUsage::GUI);
    tpc.load(stream);

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

bool TpcTool::supports(Operation operation, const fs::path &input) const {
    return !fs::is_directory(input) &&
           input.extension() == ".tpc" &&
           operation == Operation::ToTGA;
}

} // namespace reone
