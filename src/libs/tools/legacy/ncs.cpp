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

#include "reone/tools/legacy/ncs.h"

#include "reone/game/script/routines.h"
#include "reone/resource/exception/format.h"
#include "reone/resource/format/ncsreader.h"
#include "reone/resource/format/ncswriter.h"
#include "reone/system/logutil.h"
#include "reone/system/stream/fileinput.h"
#include "reone/system/stream/fileoutput.h"

#include "reone/tools/script/exprtree.h"
#include "reone/tools/script/exprtreeoptimizer.h"
#include "reone/tools/script/format/nsswriter.h"
#include "reone/tools/script/format/pcodereader.h"
#include "reone/tools/script/format/pcodewriter.h"

using namespace reone::game;
using namespace reone::resource;
using namespace reone::script;

namespace reone {

void NcsTool::invoke(
    Operation operation,
    const std::filesystem::path &input,
    const std::filesystem::path &outputDir,
    const std::filesystem::path &gamePath) {

    invokeBatch(operation, std::vector<std::filesystem::path> {input}, outputDir, gamePath);
}

void NcsTool::invokeBatch(
    Operation operation,
    const std::vector<std::filesystem::path> &input,
    const std::filesystem::path &outputDir,
    const std::filesystem::path &gamePath) {

    auto routines = Routines(_gameId, nullptr, nullptr);
    routines.init();

    return doInvokeBatch(input, outputDir, [this, &operation, &routines](auto &path, auto &outDir) {
        if (operation == Operation::ToPCODE) {
            toPCODE(path, outDir, routines);
        } else if (operation == Operation::ToNCS) {
            toNCS(path, outDir, routines);
        } else if (operation == Operation::ToNSS) {
            toNSS(path, outDir, routines);
        }
    });
}

void NcsTool::toPCODE(const std::filesystem::path &input, const std::filesystem::path &outputDir, Routines &routines) {
    auto ncs = FileInputStream(input);

    auto pcodePath = outputDir;
    pcodePath.append(input.filename().string() + ".pcode");
    auto pcode = FileOutputStream(pcodePath);

    toPCODE(ncs, pcode, routines);
}

void NcsTool::toPCODE(IInputStream &ncs, IOutputStream &pcode, Routines &routines) {
    auto reader = NcsReader(ncs, "");
    reader.load();

    auto writer = PcodeWriter(*reader.program(), routines);
    writer.save(pcode);
}

void NcsTool::toNCS(const std::filesystem::path &input, const std::filesystem::path &outputDir, Routines &routines) {
    PcodeReader pcode(input, routines);
    pcode.load();
    auto program = pcode.program();

    std::filesystem::path ncsPath(outputDir);
    ncsPath.append(input.filename().string());
    ncsPath.replace_extension(); // drop .pcode

    NcsWriter writer(*program);
    writer.save(ncsPath);
}

void NcsTool::toNSS(const std::filesystem::path &input, const std::filesystem::path &outputDir, Routines &routines) {
    auto ncs = FileInputStream(input);

    auto nssPath = outputDir;
    nssPath.append(input.filename().string() + ".nss");
    auto nss = FileOutputStream(nssPath);

    toNSS(ncs, nss, routines);
}

void NcsTool::toNSS(IInputStream &ncs, IOutputStream &nss, Routines &routines, bool optimize) {
    auto reader = NcsReader(ncs, "");
    reader.load();

    std::unique_ptr<IExpressionTreeOptimizer> optimizer;
    if (optimize) {
        optimizer = std::make_unique<ExpressionTreeOptimizer>();
    } else {
        optimizer = std::make_unique<NoOpExpressionTreeOptimizer>();
    }
    auto exprTree = ExpressionTree::fromProgram(*reader.program(), routines, *optimizer);

    auto writer = NssWriter(exprTree, routines);
    writer.save(nss);
}

bool NcsTool::supports(Operation operation, const std::filesystem::path &input) const {
    return !std::filesystem::is_directory(input) &&
           ((input.extension() == ".ncs" && (operation == Operation::ToPCODE || operation == Operation::ToNSS)) ||
            (input.extension() == ".pcode" && operation == Operation::ToNCS));
}

} // namespace reone
