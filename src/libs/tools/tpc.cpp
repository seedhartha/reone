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

#include "reone/tools/tpc.h"

#include "reone/system/exception/validation.h"
#include "reone/system/logutil.h"
#include "reone/system/stream/fileinput.h"
#include "reone/graphics/format/tgawriter.h"
#include "reone/graphics/format/tpcreader.h"

using namespace std;

using namespace reone::graphics;

namespace reone {

void TpcTool::invoke(
    Operation operation,
    const boost::filesystem::path &input,
    const boost::filesystem::path &outputDir,
    const boost::filesystem::path &gamePath) {

    invokeBatch(operation, vector<boost::filesystem::path> {input}, outputDir, gamePath);
}

void TpcTool::invokeBatch(
    Operation operation,
    const std::vector<boost::filesystem::path> &input,
    const boost::filesystem::path &outputDir,
    const boost::filesystem::path &gamePath) {

    return doInvokeBatch(input, outputDir, [this, &operation](auto &path, auto &outDir) {
        if (operation == Operation::ToTGA) {
            toTGA(path, outDir);
        }
    });
}

void TpcTool::toTGA(const boost::filesystem::path &path, const boost::filesystem::path &destPath) {
    // Read TPC

    auto stream = FileInputStream(path, OpenMode::Binary);

    TpcReader tpc("", TextureUsage::GUI);
    tpc.load(stream);

    // Write TGA

    boost::filesystem::path tgaPath(destPath);
    tgaPath.append(path.filename().string());
    tgaPath.replace_extension("tga");

    TgaWriter writer(tpc.texture());
    writer.save(tgaPath, true);

    // Write TXI

    if (!tpc.txiData().empty()) {
        boost::filesystem::path txiPath(tgaPath);
        txiPath.replace_extension("txi");

        boost::filesystem::ofstream txi(txiPath, ios::binary);
        txi.write(tpc.txiData().data(), tpc.txiData().size());
    }
}

bool TpcTool::supports(Operation operation, const boost::filesystem::path &input) const {
    return !boost::filesystem::is_directory(input) &&
           input.extension() == ".tpc" &&
           operation == Operation::ToTGA;
}

} // namespace reone
