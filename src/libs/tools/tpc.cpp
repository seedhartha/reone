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

#include "reone/graphics/format/tgawriter.h"
#include "reone/graphics/format/tpcreader.h"
#include "reone/resource/exception/format.h"
#include "reone/system/logutil.h"
#include "reone/system/stream/fileinput.h"
#include "reone/system/stream/fileoutput.h"
#include "reone/system/stream/memoryoutput.h"

using namespace reone::graphics;

namespace reone {

void TpcTool::invoke(
    Operation operation,
    const boost::filesystem::path &input,
    const boost::filesystem::path &outputDir,
    const boost::filesystem::path &gamePath) {

    invokeBatch(operation, std::vector<boost::filesystem::path> {input}, outputDir, gamePath);
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
    auto tpc = FileInputStream(path, OpenMode::Binary);

    auto tgaPath = destPath;
    tgaPath.append(path.filename().string());
    tgaPath.replace_extension("tga");

    auto tga = FileOutputStream(tgaPath, OpenMode::Binary);
    auto txiBytes = ByteArray();
    auto txiMemory = MemoryOutputStream(txiBytes);
    toTGA(tpc, tga, txiMemory, true);

    if (!txiBytes.empty()) {
        auto txiPath = tgaPath;
        txiPath.replace_extension("txi");

        auto txi = FileOutputStream(txiPath, OpenMode::Binary);
        txi.write(&txiBytes[0], txiBytes.size());
    }
}

void TpcTool::toTGA(IInputStream &tpc, IOutputStream &tga, IOutputStream &txi, bool compress) {
    auto reader = TpcReader("", TextureUsage::GUI);
    reader.load(tpc);

    auto tgaWriter = TgaWriter(reader.texture());
    tgaWriter.save(tga, compress);

    if (!reader.txiData().empty()) {
        txi.write(reader.txiData().data(), reader.txiData().size());
    }
}

bool TpcTool::supports(Operation operation, const boost::filesystem::path &input) const {
    return !boost::filesystem::is_directory(input) &&
           input.extension() == ".tpc" &&
           operation == Operation::ToTGA;
}

} // namespace reone
