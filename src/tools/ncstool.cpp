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

#include "../engine/common/collectionutil.h"
#include "../engine/common/logutil.h"
#include "../engine/game/script/routine/iroutines.h"
#include "../engine/game/script/routine/registrar/kotor.h"
#include "../engine/game/script/routine/registrar/tsl.h"
#include "../engine/script/format/ncsreader.h"
#include "../engine/script/instrutil.h"
#include "../engine/script/program.h"
#include "../engine/script/routine.h"
#include "../engine/script/variable.h"

using namespace std;

using namespace reone::game;
using namespace reone::resource;
using namespace reone::script;

namespace fs = boost::filesystem;

namespace reone {

namespace tools {

class StubRoutines : public IRoutines {
public:
    void add(
        string name,
        VariableType retType,
        vector<VariableType> argTypes,
        Variable (*fn)(const vector<Variable> &args, const RoutineContext &ctx)) override {

        Routine routine(
            move(name),
            retType,
            move(argTypes),
            [](auto &args, auto &ctx) { return Variable::ofNull(); });

        _routines.push_back(move(routine));
    }

    const Routine &get(int index) const override { return _routines[index]; }

private:
    vector<Routine> _routines;
};

void NcsTool::invoke(Operation operation, const fs::path &target, const fs::path &gamePath, const fs::path &destPath) {
    if (operation == Operation::ToPCODE) {
        toPCODE(target, destPath);
    }
}

void NcsTool::toPCODE(const fs::path &path, const fs::path &destPath) {
    NcsReader ncs("");
    ncs.load(path);
    auto program = ncs.program();
    auto instructions = mapToValues(program->instructions());
    sort(instructions.begin(), instructions.end(), [](auto &a, auto &b) { return a.offset < b.offset; });

    StubRoutines routines;
    if (_tsl) {
        TSLRoutineRegistrar registar(routines);
        registar.invoke();
    } else {
        KotORRoutineRegistrar registar(routines);
        registar.invoke();
    }

    fs::path pcodePath(destPath);
    pcodePath.append(path.filename().string() + ".pcode");

    fs::ofstream pcode(pcodePath);
    try {
        for (auto &instr : instructions) {
            pcode << describeInstruction(instr, routines) << endl;
        }
    } catch (const exception &e) {
        fs::remove(pcodePath);
        throw runtime_error(e.what());
    }
}

bool NcsTool::supports(Operation operation, const fs::path &target) const {
    return !fs::is_directory(target) && ((target.extension() == ".ncs" && operation == Operation::ToPCODE));
}

} // namespace tools

} // namespace reone
