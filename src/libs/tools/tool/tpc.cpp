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

#include "reone/tools/tool/tpc.h"

#include "reone/common/exception/validation.h"
#include "reone/common/logutil.h"
#include "reone/common/stream/fileinput.h"
#include "reone/graphics/format/tgawriter.h"
#include "reone/graphics/format/tpcreader.h"

using namespace std;

using namespace reone::graphics;

namespace fs = boost::filesystem;

namespace reone {

void TpcTool::invoke(
    Operation operation,
    const fs::path &input,
    const fs::path &outputDir,
    const fs::path &gamePath) {

    invokeBatch(operation, vector<fs::path> {input}, outputDir, gamePath);
}

void TpcTool::invokeBatch(
    Operation operation,
    const std::vector<fs::path> &input,
    const fs::path &outputDir,
    const fs::path &gamePath) {

    return doInvokeBatch(input, outputDir, [this, &operation](auto &path, auto &outDir) {
        if (operation == Operation::ToTGA) {
            toTGA(path, outDir);
        }
    });
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
