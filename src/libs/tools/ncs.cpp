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

#include "reone/tools/ncs.h"

#include <boost/regex.hpp>

#include "reone/system/exception/validation.h"
#include "reone/system/logutil.h"
#include "reone/system/stream/fileinput.h"
#include "reone/system/stream/fileoutput.h"
#include "reone/game/script/routines.h"
#include "reone/script/format/ncsreader.h"
#include "reone/script/format/ncswriter.h"

#include "reone/tools/script/expressiontree.h"
#include "reone/tools/script/format/nsswriter.h"
#include "reone/tools/script/format/pcodereader.h"
#include "reone/tools/script/format/pcodewriter.h"

using namespace std;

using namespace reone::game;
using namespace reone::resource;
using namespace reone::script;

namespace reone {

void NcsTool::invoke(
    Operation operation,
    const boost::filesystem::path &input,
    const boost::filesystem::path &outputDir,
    const boost::filesystem::path &gamePath) {

    invokeBatch(operation, vector<boost::filesystem::path> {input}, outputDir, gamePath);
}

void NcsTool::invokeBatch(
    Operation operation,
    const std::vector<boost::filesystem::path> &input,
    const boost::filesystem::path &outputDir,
    const boost::filesystem::path &gamePath) {

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

void NcsTool::toPCODE(const boost::filesystem::path &input, const boost::filesystem::path &outputDir, Routines &routines) {
    auto stream = FileInputStream(input, OpenMode::Binary);

    NcsReader ncs("");
    ncs.load(stream);

    boost::filesystem::path pcodePath(outputDir);
    pcodePath.append(input.filename().string() + ".pcode");

    PcodeWriter pcode(*ncs.program(), routines);
    pcode.save(pcodePath);
}

void NcsTool::toNCS(const boost::filesystem::path &input, const boost::filesystem::path &outputDir, Routines &routines) {
    PcodeReader pcode(input, routines);
    pcode.load();
    auto program = pcode.program();

    boost::filesystem::path ncsPath(outputDir);
    ncsPath.append(input.filename().string());
    ncsPath.replace_extension(); // drop .pcode

    NcsWriter writer(*program);
    writer.save(ncsPath);
}

void NcsTool::toNSS(const boost::filesystem::path &input, const boost::filesystem::path &outputDir, Routines &routines) {
    auto ncs = FileInputStream(input, OpenMode::Binary);

    auto reader = NcsReader("");
    reader.load(ncs);

    auto compiledProgram = reader.program();
    auto program = ExpressionTree::fromProgram(*compiledProgram, routines);

    auto nssPath = outputDir;
    nssPath.append(input.filename().string() + ".nss");

    auto nss = FileOutputStream(nssPath);

    auto writer = NssWriter(program, routines);
    writer.save(nss);
}

bool NcsTool::supports(Operation operation, const boost::filesystem::path &input) const {
    return !boost::filesystem::is_directory(input) &&
           ((input.extension() == ".ncs" && (operation == Operation::ToPCODE || operation == Operation::ToNSS)) ||
            (input.extension() == ".pcode" && operation == Operation::ToNCS));
}

} // namespace reone
