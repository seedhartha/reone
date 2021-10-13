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

#include "ncs.h"

#include <boost/regex.hpp>

#include "../../engine/common/collectionutil.h"
#include "../../engine/common/logutil.h"
#include "../../engine/game/core/script/routine/iroutines.h"
#include "../../engine/game/kotor/routine/registrar.h"
#include "../../engine/game/limbo/routine/registrar.h"
#include "../../engine/game/tsl/routine/registrar.h"
#include "../../engine/script/format/ncsreader.h"
#include "../../engine/script/format/ncswriter.h"
#include "../../engine/script/instrutil.h"
#include "../../engine/script/program.h"
#include "../../engine/script/routine.h"
#include "../../engine/script/variable.h"

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

class PcodeReader {
public:
    PcodeReader(fs::path path) :
        _path(move(path)) {
    }

    void load() {
        fs::ifstream pcode(_path);

        _program = make_shared<ScriptProgram>("");

        string line;
        while (getline(pcode, line)) {
            _program->add(parseInstruction(line));
        }
    }

    shared_ptr<ScriptProgram> program() { return _program; }

private:
    fs::path _path;

    shared_ptr<ScriptProgram> _program;

    Instruction parseInstruction(const string &line) const {
        char *p = nullptr;

        boost::smatch what;
        boost::regex re("^([\\d\\w]{8}) (\\w+).*$");
        if (!boost::regex_match(line, what, re)) {
            throw invalid_argument("line must contain at least offset and instruction type");
        }
        uint32_t offset = strtoul(what[1].str().c_str(), &p, 16);
        string typeDesc = what[2].str();
        string argsLine(line.substr(9 + typeDesc.length()));

        InstructionType type = parseInstructionType(typeDesc);

        Instruction ins;
        ins.offset = offset;
        ins.type = type;

        switch (type) {
        case InstructionType::CPDOWNSP:
        case InstructionType::CPTOPSP:
        case InstructionType::CPDOWNBP:
        case InstructionType::CPTOPBP:
            applyArguments(argsLine, "^ ([-\\d]+), (\\d+)$", 2, [&ins](auto &args) {
                ins.stackOffset = atoi(args[0].c_str());
                ins.size = atoi(args[1].c_str());
            });
            break;
        case InstructionType::CONSTI:
            applyArguments(argsLine, "^ ([-\\d]+)$", 1, [&ins](auto &args) {
                ins.intValue = atoi(args[0].c_str());
            });
            break;
        case InstructionType::CONSTF:
            applyArguments(argsLine, "^ ([-\\.\\d]+)$", 1, [&ins](auto &args) {
                ins.floatValue = atof(args[0].c_str());
            });
            break;
        case InstructionType::CONSTS:
            applyArguments(argsLine, "^ \"(.*)\"$", 1, [&ins](auto &args) {
                ins.strValue = args[0];
            });
            break;
        case InstructionType::CONSTO:
            applyArguments(argsLine, "^ (\\d+)$", 1, [&ins](auto &args) {
                ins.objectId = atoi(args[0].c_str());
            });
            break;
        case InstructionType::ACTION:
            applyArguments(argsLine, "^ \\w+\\((\\d+)\\), (\\d+)$", 2, [&ins](auto &args) {
                ins.routine = atoi(args[0].c_str());
                ins.argCount = atoi(args[1].c_str());
            });
            break;
        case InstructionType::MOVSP:
            applyArguments(argsLine, "^ ([-\\d]+)$", 1, [&ins](auto &args) {
                ins.stackOffset = atoi(args[0].c_str());
            });
            break;
        case InstructionType::JMP:
        case InstructionType::JSR:
        case InstructionType::JZ:
        case InstructionType::JNZ:
            applyArguments(argsLine, "^ [\\d\\w]{8}\\(([-\\d]+)\\)$", 1, [&ins](auto &args) {
                ins.jumpOffset = atoi(args[0].c_str());
            });
            break;
        case InstructionType::DESTRUCT:
            applyArguments(argsLine, "^ (\\d+), ([-\\d]+), (\\d+)$", 3, [&ins](auto &args) {
                ins.size = atoi(args[0].c_str());
                ins.stackOffset = atoi(args[1].c_str());
                ins.sizeNoDestroy = atoi(args[2].c_str());
            });
            break;
        case InstructionType::DECISP:
        case InstructionType::INCISP:
        case InstructionType::DECIBP:
        case InstructionType::INCIBP:
            applyArguments(argsLine, "^ ([-\\d]+)$", 1, [&ins](auto &args) {
                ins.stackOffset = atoi(args[0].c_str());
            });
            break;
        case InstructionType::STORE_STATE:
            applyArguments(argsLine, "^ (\\d+), (\\d+)$", 2, [&ins](auto &args) {
                ins.size = atoi(args[0].c_str());
                ins.sizeLocals = atoi(args[1].c_str());
            });
            break;
        case InstructionType::EQUALTT:
        case InstructionType::NEQUALTT:
            applyArguments(argsLine, "^ (\\d+)$", 1, [&ins](auto &args) {
                ins.size = atoi(args[0].c_str());
            });
            break;
        default:
            break;
        };

        return move(ins);
    }

    void applyArguments(const string &line, const string &restr, int numArgs, const function<void(const vector<string> &)> &fn) const {
        boost::smatch what;
        boost::regex re(restr);
        if (!boost::regex_match(line, what, re)) {
            throw invalid_argument(str(boost::format("Arguments line '%s' must match regular expression '%s'") % line % restr));
        }
        vector<string> args;
        for (int i = 0; i < numArgs; ++i) {
            args.push_back(what[1 + i].str());
        }
        fn(move(args));
    }
};

void NcsTool::invoke(Operation operation, const fs::path &target, const fs::path &gamePath, const fs::path &destPath) {
    if (operation == Operation::ToPCODE) {
        toPCODE(target, destPath);
    } else if (operation == Operation::ToNCS) {
        toNCS(target, destPath);
    }
}

void NcsTool::toPCODE(const fs::path &path, const fs::path &destPath) {
    StubRoutines routines;
    fillRoutines(routines);

    NcsReader ncs("");
    ncs.load(path);
    auto program = ncs.program();

    fs::path pcodePath(destPath);
    pcodePath.append(path.filename().string() + ".pcode");

    fs::ofstream pcode(pcodePath);
    try {
        for (auto &instr : program->instructions()) {
            pcode << describeInstruction(instr, routines) << endl;
        }
    } catch (const exception &e) {
        fs::remove(pcodePath);
        throw runtime_error(e.what());
    }
}

void NcsTool::toNCS(const fs::path &path, const fs::path &destPath) {
    StubRoutines routines;
    fillRoutines(routines);

    PcodeReader pcode(path);
    pcode.load();
    auto program = pcode.program();

    fs::path ncsPath(destPath);
    ncsPath.append(path.filename().string());
    ncsPath.replace_extension(); // drop .pcode

    NcsWriter writer(*program);
    writer.save(ncsPath);
}

bool NcsTool::supports(Operation operation, const fs::path &target) const {
    return !fs::is_directory(target) &&
           ((target.extension() == ".ncs" && operation == Operation::ToPCODE) ||
            (target.extension() == ".pcode" && operation == Operation::ToNCS));
}

void NcsTool::fillRoutines(IRoutines &routines) {
    switch (_gameId) {
    case GameID::TSL:
        TSLRoutineRegistrar(routines).invoke();
        break;
    case GameID::Limbo:
        LimboRoutineRegistrar(routines).invoke();
        break;
    default: {
        KotORRoutineRegistrar(routines).invoke();
        break;
    }
    }
}

} // namespace tools

} // namespace reone
