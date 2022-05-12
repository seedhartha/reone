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

#include "../../common/collectionutil.h"
#include "../../common/exception/argument.h"
#include "../../common/exception/validation.h"
#include "../../common/stream/fileinput.h"
#include "../../common/stream/fileoutput.h"
#include "../../common/textwriter.h"
#include "../../game/script/routines.h"
#include "../../script/format/ncsreader.h"
#include "../../script/format/ncswriter.h"
#include "../../script/instrutil.h"
#include "../../script/program.h"
#include "../../script/routine.h"
#include "../../script/variable.h"

#include "nwscript/program.h"

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

class PcodeReader {
public:
    PcodeReader(fs::path path, Routines &routines) :
        _path(move(path)),
        _routines(routines) {
    }

    void load() {
        vector<string> insLines;
        map<int, string> labelByLineIdx;
        map<int, uint32_t> addrByLineIdx;

        fs::ifstream pcode(_path);
        string line;
        boost::smatch what;
        boost::regex re("^([_\\d\\w]+):$");
        uint32_t addr = 13;
        while (getline(pcode, line)) {
            boost::trim(line);
            if (line.empty()) {
                continue;
            }
            int lineIdx = static_cast<int>(insLines.size());
            if (boost::regex_match(line, what, re)) {
                labelByLineIdx[lineIdx] = what[1].str();
                continue;
            }
            addrByLineIdx[lineIdx] = addr;
            addr += getInstructionSize(line);
            insLines.push_back(line);
        }

        fs::path filename(_path.filename());
        filename.replace_extension(); // drop .pcode
        filename.replace_extension(); // drop .ncs

        _addrByLabel.clear();
        for (auto &pair : labelByLineIdx) {
            _addrByLabel[pair.second] = addrByLineIdx[pair.first];
        }

        _program = make_shared<ScriptProgram>(filename.string());
        for (size_t i = 0; i < insLines.size(); ++i) {
            uint32_t insAddr = addrByLineIdx.find(static_cast<int>(i))->second;
            _program->add(parseInstruction(insLines[i], insAddr));
        }
    }

    shared_ptr<ScriptProgram> program() { return _program; }

private:
    fs::path _path;
    Routines &_routines;

    shared_ptr<ScriptProgram> _program;
    map<string, uint32_t> _addrByLabel;

    int getInstructionSize(const string &line) {
        int result = 2;

        size_t spaceIdx = line.find(" ");
        string typeDesc(spaceIdx != string::npos ? line.substr(0, spaceIdx) : line);
        string argsLine(line.substr(typeDesc.length()));
        InstructionType type = parseInstructionType(typeDesc);

        switch (type) {
        case InstructionType::CPDOWNSP:
        case InstructionType::CPTOPSP:
        case InstructionType::CPDOWNBP:
        case InstructionType::CPTOPBP:
        case InstructionType::DESTRUCT:
            result += 6;
            break;
        case InstructionType::CONSTI:
        case InstructionType::CONSTF:
        case InstructionType::CONSTO:
        case InstructionType::MOVSP:
        case InstructionType::JMP:
        case InstructionType::JSR:
        case InstructionType::JZ:
        case InstructionType::JNZ:
        case InstructionType::DECISP:
        case InstructionType::INCISP:
        case InstructionType::DECIBP:
        case InstructionType::INCIBP:
            result += 4;
            break;
        case InstructionType::CONSTS:
            result += 2;
            applyArguments(argsLine, "^ \"(.*)\"$", 1, [&result](auto &args) {
                result += args[0].length();
            });
            break;
        case InstructionType::ACTION:
            result += 3;
            break;
        case InstructionType::STORE_STATE:
            result += 8;
            break;
        case InstructionType::EQUALTT:
        case InstructionType::NEQUALTT:
            result += 2;
            break;
        default:
            break;
        };

        return result;
    }

    Instruction parseInstruction(const string &line, uint32_t addr) const {
        size_t spaceIdx = line.find(" ");
        string typeDesc(spaceIdx != string::npos ? line.substr(0, spaceIdx) : line);
        string argsLine(line.substr(typeDesc.length()));
        InstructionType type = parseInstructionType(typeDesc);

        Instruction ins;
        ins.offset = addr;
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
            applyArguments(argsLine, "^ (\\w+), (\\d+)$", 2, [this, &ins](auto &args) {
                ins.routine = _routines.getIndexByName(args[0]);
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
            applyArguments(argsLine, "^ ([_\\d\\w]+)$", 1, [this, &ins](auto &args) {
                const string &label = args[0];
                auto maybeAddr = _addrByLabel.find(label);
                if (maybeAddr == _addrByLabel.end()) {
                    throw ValidationException("Instruction address not found by label '" + label + "'");
                }
                ins.jumpOffset = maybeAddr->second - ins.offset;
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

class PcodeWriter {
public:
    PcodeWriter(ScriptProgram &program, Routines &routines) :
        _program(program),
        _routines(routines) {
    }

    void save(const fs::path &path) {
        fs::ofstream pcode(path);
        try {
            set<uint32_t> jumpOffsets;
            for (auto &instr : _program.instructions()) {
                switch (instr.type) {
                case InstructionType::JMP:
                case InstructionType::JSR:
                case InstructionType::JZ:
                case InstructionType::JNZ:
                    jumpOffsets.insert(instr.offset + instr.jumpOffset);
                    break;
                default:
                    break;
                }
            }
            for (auto &instr : _program.instructions()) {
                writeInstruction(instr, pcode, jumpOffsets);
            }
        } catch (const exception &e) {
            fs::remove(path);
            throw runtime_error(e.what());
        }
    }

private:
    ScriptProgram &_program;
    Routines &_routines;

    void writeInstruction(const Instruction &ins, fs::ofstream &pcode, const set<uint32_t> &jumpOffsets) {
        if (jumpOffsets.count(ins.offset) > 0) {
            string label(str(boost::format("loc_%08x:") % ins.offset));
            pcode << label << endl;
        }

        string desc(describeInstructionType(ins.type));

        switch (ins.type) {
        case InstructionType::CPDOWNSP:
        case InstructionType::CPTOPSP:
        case InstructionType::CPDOWNBP:
        case InstructionType::CPTOPBP:
            desc += str(boost::format(" %d, %d") % ins.stackOffset % ins.size);
            break;
        case InstructionType::CONSTI:
            desc += " " + to_string(ins.intValue);
            break;
        case InstructionType::CONSTF:
            desc += " " + to_string(ins.floatValue);
            break;
        case InstructionType::CONSTS:
            desc += " \"" + ins.strValue + "\"";
            break;
        case InstructionType::CONSTO:
            desc += " " + to_string(ins.objectId);
            break;
        case InstructionType::ACTION:
            desc += str(boost::format(" %s, %d") % _routines.get(ins.routine).name() % ins.argCount);
            break;
        case InstructionType::EQUALTT:
        case InstructionType::NEQUALTT:
            desc += " " + to_string(ins.size);
            break;
        case InstructionType::MOVSP:
            desc += " " + to_string(ins.stackOffset);
            break;
        case InstructionType::JMP:
        case InstructionType::JSR:
        case InstructionType::JZ:
        case InstructionType::JNZ: {
            uint32_t jumpAddr = ins.offset + ins.jumpOffset;
            desc += str(boost::format(" loc_%08x") % jumpAddr);
            break;
        }
        case InstructionType::DESTRUCT:
            desc += str(boost::format(" %d, %d, %d") % ins.size % ins.stackOffset % ins.sizeNoDestroy);
            break;
        case InstructionType::DECISP:
        case InstructionType::INCISP:
        case InstructionType::DECIBP:
        case InstructionType::INCIBP:
            desc += " " + to_string(ins.stackOffset);
            break;
        case InstructionType::STORE_STATE:
            desc += str(boost::format(" %d, %d") % ins.size % ins.sizeLocals);
            break;
        default:
            break;
        }

        pcode << desc << endl;
    }
};

class NssWriter {
public:
    NssWriter(
        NwscriptProgram &program,
        Routines &routines) :
        _program(program),
        _routines(routines) {
    }

    void save(IOutputStream &stream) {
        auto writer = TextWriter(stream);
        for (auto &function : _program.functions()) {
            writeFunction(*function, writer);
        }
    }

private:
    NwscriptProgram &_program;
    Routines &_routines;

    void writeFunction(const NwscriptProgram::Function &function, TextWriter &writer) {
        auto name = describeFunction(function);
        writer.putLine(str(boost::format("void %s()") % name));
        writeExpression(0, *function.block, writer);
        writer.putLine("");
    }

    void writeExpression(int level, const NwscriptProgram::Expression &expression, TextWriter &writer) {
        auto indent = string(4 * level, ' ');

        if (expression.type == NwscriptProgram::ExpressionType::Block) {
            auto &blockExpr = static_cast<const NwscriptProgram::BlockExpression &>(expression);
            writer.putLine(indent + string("{"));
            for (auto &innerExpr : blockExpr.expressions) {
                writeExpression(1 + level, *innerExpr, writer);
            }
            writer.putLine(indent + string("}"));

        } else if (expression.type == NwscriptProgram::ExpressionType::Return) {
            writer.putLine(indent + string("return;"));

        } else if (expression.type == NwscriptProgram::ExpressionType::Constant) {
            auto &constExpr = static_cast<const NwscriptProgram::ConstantExpression &>(expression);
            auto type = describeVariableType(constExpr.value.type);
            auto name = describeExpression(constExpr);
            string value;
            if (constExpr.value.type == VariableType::Int) {
                value = to_string(constExpr.value.intValue);
            } else if (constExpr.value.type == VariableType::Float) {
                value = str(boost::format("%ff") % constExpr.value.floatValue);
            } else if (constExpr.value.type == VariableType::String) {
                value = str(boost::format("\"%s\"") % constExpr.value.strValue);
            } else if (constExpr.value.type == VariableType::Object) {
                value = to_string(constExpr.value.objectId);
            }
            writer.putLine(str(boost::format("%s%s %s = %s;") % indent % type % name % value));

        } else if (expression.type == NwscriptProgram::ExpressionType::Parameter) {
            auto &paramExpr = static_cast<const NwscriptProgram::ParameterExpression &>(expression);
            auto type = describeVariableType(paramExpr.variableType);
            auto name = describeExpression(paramExpr);
            writer.putLine(str(boost::format("%s%s %s;") % indent % type % name));

        } else if (expression.type == NwscriptProgram::ExpressionType::Assign) {
            auto &binaryExpr = static_cast<const NwscriptProgram::BinaryExpression &>(expression);
            auto left = describeExpression(*binaryExpr.left);
            auto right = describeExpression(*binaryExpr.right);
            writer.putLine(str(boost::format("%s%s = %s;") % indent % left % right));

        } else if (expression.type == NwscriptProgram::ExpressionType::Call) {
            auto &callExpr = static_cast<const NwscriptProgram::CallExpression &>(expression);
            auto name = describeFunction(*callExpr.function);
            writer.putLine(str(boost::format("%s%s();") % indent % name));

        } else if (expression.type == NwscriptProgram::ExpressionType::Action) {
            auto &actionExpr = static_cast<const NwscriptProgram::ActionExpression &>(expression);
            auto description = describeExpression(actionExpr);
            writer.putLine(str(boost::format("%s%s;") % indent % description));

        } else if (expression.type == NwscriptProgram::ExpressionType::Conditional) {
            auto &condExpr = static_cast<const NwscriptProgram::ConditionalExpression &>(expression);
            auto testDescription = describeExpression(*condExpr.test);
            writer.putLine(str(boost::format("%sif(%s)") % indent % testDescription));
            writeExpression(level, *condExpr.ifTrue, writer);
            writer.putLine(indent + string("else"));
            writeExpression(level, *condExpr.ifFalse, writer);
        }
    }

    std::string describeFunction(const NwscriptProgram::Function &function) {
        return !function.name.empty() ? function.name : str(boost::format("loc_%08x") % function.offset);
    }

    std::string describeExpression(const NwscriptProgram::Expression &expression) {
        if (expression.type == NwscriptProgram::ExpressionType::Constant) {
            auto &constExpr = static_cast<const NwscriptProgram::ConstantExpression &>(expression);
            return str(boost::format("CONST_%08x") % constExpr.offset);

        } else if (expression.type == NwscriptProgram::ExpressionType::Parameter) {
            auto &paramExpr = static_cast<const NwscriptProgram::ParameterExpression &>(expression);
            return str(boost::format("var_%08x") % paramExpr.offset);

        } else if (expression.type == NwscriptProgram::ExpressionType::Action) {
            auto &actionExpr = static_cast<const NwscriptProgram::ActionExpression &>(expression);
            auto name = _routines.get(actionExpr.action).name();
            vector<string> arguments;
            for (auto &argExpr : actionExpr.arguments) {
                arguments.push_back(describeExpression(*argExpr));
            }
            return str(boost::format("%s(%s)") % name % boost::join(arguments, ", "));

        } else if (expression.type == NwscriptProgram::ExpressionType::Zero ||
                   expression.type == NwscriptProgram::ExpressionType::NotZero) {
            auto &unaryExpr = static_cast<const NwscriptProgram::UnaryExpression &>(expression);
            if (expression.type == NwscriptProgram::ExpressionType::Zero) {
                return string("!") + describeExpression(*unaryExpr.operand);
            } else if (expression.type == NwscriptProgram::ExpressionType::NotZero) {
                return describeExpression(*unaryExpr.operand);
            }
        }

        throw ArgumentException("Cannot describe expression of type " + to_string(static_cast<int>(expression.type)));
    }

    std::string describeVariableType(VariableType type) {
        switch (type) {
        case VariableType::Int:
            return "int";
        case VariableType::Float:
            return "float";
        case VariableType::String:
            return "string";
        case VariableType::Vector:
            return "vector";
        case VariableType::Object:
            return "object";
        case VariableType::Effect:
            return "effect";
        case VariableType::Event:
            return "event";
        case VariableType::Location:
            return "location";
        case VariableType::Talent:
            return "talent";
        case VariableType::Action:
            return "action";
        default:
            throw ArgumentException("Cannot describe variable of type: " + to_string(static_cast<int>(type)));
        }
    }
};

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
    auto program = NwscriptProgram::fromCompiled(*compiledProgram, routines);

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
