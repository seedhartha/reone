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

#include "ncs.h"

#include <boost/regex.hpp>

#include "../../common/stream/fileinput.h"
#include "../../common/stream/fileoutput.h"
#include "../../game/script/routines.h"
#include "../../script/expressiontree.h"
#include "../../script/format/ncsreader.h"
#include "../../script/format/ncswriter.h"
#include "../../script/format/nsswriter.h"
#include "../../script/format/pcodereader.h"
#include "../../script/format/pcodewriter.h"

using namespace std;

using namespace reone::game;
using namespace reone::resource;
using namespace reone::script;

namespace fs = boost::filesystem;

namespace reone {

static void initRoutines(GameID gameId, Routines &routines) {
    if (gameId == GameID::TSL) {
        routines.initForTSL();
    } else {
        routines.initForKotOR();
    }
}

void NcsTool::invoke(Operation operation, const fs::path &target, const fs::path &gamePath, const fs::path &destPath) {
    if (operation == Operation::ToPCODE) {
        toPCODE(target, destPath);
    } else if (operation == Operation::ToNCS) {
        toNCS(target, destPath);
    } else if (operation == Operation::ToNSS) {
        toNSS(target, destPath);
    }
}

void NcsTool::toPCODE(const fs::path &path, const fs::path &destPath) {
    Routines routines;
    initRoutines(_gameId, routines);

    auto stream = FileInputStream(path, OpenMode::Binary);

    NcsReader ncs("");
    ncs.load(stream);

    fs::path pcodePath(destPath);
    pcodePath.append(path.filename().string() + ".pcode");

    PcodeWriter pcode(*ncs.program(), routines);
    pcode.save(pcodePath);
}

void NcsTool::toNCS(const fs::path &path, const fs::path &destPath) {
    Routines routines;
    initRoutines(_gameId, routines);

    PcodeReader pcode(path, routines);
    pcode.load();
    auto program = pcode.program();

    fs::path ncsPath(destPath);
    ncsPath.append(path.filename().string());
    ncsPath.replace_extension(); // drop .pcode

    NcsWriter writer(*program);
    writer.save(ncsPath);
}

void NcsTool::toNSS(const fs::path &path, const fs::path &destPath) {
    auto routines = Routines();
    initRoutines(_gameId, routines);

    auto ncs = FileInputStream(path, OpenMode::Binary);
    auto reader = NcsReader("");
    reader.load(ncs);
    auto compiledProgram = reader.program();
    auto program = ExpressionTree::fromProgram(*compiledProgram, routines);

    auto nssPath = destPath;
    nssPath.append(path.filename().string() + ".nss");
    auto nss = FileOutputStream(nssPath);
    auto writer = NssWriter(program, routines);
    writer.save(nss);
}

bool NcsTool::supports(Operation operation, const fs::path &target) const {
    return !fs::is_directory(target) &&
           ((target.extension() == ".ncs" && (operation == Operation::ToPCODE || operation == Operation::ToNSS)) ||
            (target.extension() == ".pcode" && operation == Operation::ToNCS));
}

} // namespace reone
