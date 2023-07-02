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

#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/regex.hpp>

#include "reone/resource/format/erfwriter.h"
#include "reone/system/exception/notimplemented.h"
#include "reone/system/stream/fileoutput.h"
#include "reone/system/textwriter.h"

using namespace reone;
using namespace reone::resource;

static const boost::regex CONST_REG_EXP = boost::regex("^(\\w+)\\s+(\\w+)\\s*=(.*);.*");
static const boost::regex CONST_VEC_REG_EXP = boost::regex("^\\[\\s*([\\.\\d]+f?),\\s*([\\.\\d]+f?),\\s*([\\.\\d]+f?)\\s*\\]$");

static const boost::regex FUNC_DECL_REG_EXP = boost::regex("^(\\w+)\\s+(\\w+)\\s*\\((.*)\\);$");
static const boost::regex FUNC_ARG_REG_EXP = boost::regex("^(\\w+)\\s+(\\w+)(.*)$");
static const boost::regex FUNC_ARG_DEF_VAL_REG_EXP = boost::regex("^(\\w+)\\s+(\\w+)\\s*=(.*)$");

static const std::string REONE_COPYRIGHT_NOTICE = R"END(/*
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
 */)END";

static const std::string REONE_INDENT = "    ";

struct Constant {
    std::string type;
    std::string name;
    std::string value;

    Constant() = default;

    Constant(std::string type, std::string name, std::string value) :
        type(type), name(name), value(value) {
    }
};

struct FunctionArgument {
    std::string type;
    std::string name;
    std::string defVal;

    FunctionArgument() = default;

    FunctionArgument(std::string type, std::string name, std::string defVal) :
        type(type), name(name), defVal(defVal) {
    }
};

struct Function {
    std::string retType;
    std::string name;
    std::vector<FunctionArgument> args;

    Function() = default;

    Function(std::string retType, std::string name, std::vector<FunctionArgument> args) :
        retType(retType), name(name), args(std::move(args)) {
    }
};

static Constant parseConstant(const boost::smatch &match) {
    auto type = boost::to_lower_copy(match[1].str());
    auto name = match[2].str();
    auto value = boost::trim_copy(match[3].str());
    return Constant(type, name, value);
}

static FunctionArgument parseFunctionArgument(const std::string &str) {
    boost::smatch match;
    if (!boost::regex_search(str, match, FUNC_ARG_REG_EXP)) {
        throw std::invalid_argument("Invalid function argument format: " + str);
    }
    auto type = boost::to_lower_copy(match[1].str());
    auto name = match[2].str();
    boost::smatch defValMatch;
    std::string defVal;
    if (boost::regex_search(str, defValMatch, FUNC_ARG_DEF_VAL_REG_EXP)) {
        defVal = boost::trim_copy(defValMatch[3].str());
    }
    return FunctionArgument(type, name, defVal);
}

static std::vector<std::string> splitFunctionArguments(const std::string &str) {
    std::vector<std::string> args;
    std::vector<char> argChars;
    bool inStr = false;
    bool inVec = false;
    for (auto &ch : str) {
        if (ch == '"') {
            inStr = !inStr;
        }
        if (ch == '[' && !inStr) {
            inVec = true;
        } else if (ch == ']' && !inStr) {
            inVec = false;
        } else if (ch == ',' && !inStr && !inVec) {
            auto arg = boost::trim_copy(std::string(argChars.begin(), argChars.end()));
            if (!arg.empty()) {
                args.push_back(arg);
            }
            argChars.clear();
            continue;
        }
        argChars.push_back(ch);
    }
    auto arg = boost::trim_copy(std::string(argChars.begin(), argChars.end()));
    if (!arg.empty()) {
        args.push_back(arg);
    }
    return args;
}

static std::vector<FunctionArgument> parseFunctionArguments(const std::string &str) {
    std::vector<FunctionArgument> args;
    auto splitted = splitFunctionArguments(str);
    for (auto &arg : splitted) {
        args.push_back(parseFunctionArgument(arg));
    }
    return args;
}

static Function parseFunction(const boost::smatch &match) {
    auto retType = boost::to_lower_copy(match[1].str());
    auto name = match[2].str();
    auto argsStr = match[3].str();
    std::vector<FunctionArgument> args;
    if (!argsStr.empty()) {
        args = parseFunctionArguments(argsStr);
    }
    return Function(retType, name, std::move(args));
}

static std::tuple<std::map<std::string, Constant>, std::vector<Function>> parseNwscriptNss(const boost::filesystem::path &path) {
    std::map<std::string, Constant> constants;
    std::vector<Function> functions;
    auto f = boost::filesystem::ifstream(path);
    for (std::string line; std::getline(f, line);) {
        boost::smatch constMatch;
        if (boost::regex_search(line, constMatch, CONST_REG_EXP)) {
            auto constant = parseConstant(constMatch);
            constants[constant.name] = std::move(constant);
        }
        boost::smatch funcMatch;
        if (boost::regex_search(line, funcMatch, FUNC_DECL_REG_EXP)) {
            functions.push_back(parseFunction(funcMatch));
        }
    }
    return std::make_tuple(constants, functions);
}

static std::string evaluateConstant(const std::string &constStr, const std::map<std::string, Constant> &constants) {
    if (constants.count(constStr) > 0) {
        return constants.at(constStr).value;
    } else {
        boost::smatch vecMatch;
        if (boost::regex_search(constStr, vecMatch, CONST_VEC_REG_EXP)) {
            auto x = vecMatch[1].str();
            auto y = vecMatch[2].str();
            auto z = vecMatch[3].str();
            return str(boost::format("glm::vec3(%ff, %ff, %ff)") % x % y % z);
        } else {
            return constStr;
        }
    }
}

void writeReoneRoutineGetArgExpr(int idx,
                                 const FunctionArgument &arg,
                                 const std::map<std::string, Constant> &constants,
                                 TextWriter &code) {
    if (!arg.defVal.empty()) {
        auto evaluated = evaluateConstant(arg.defVal, constants);
        if (arg.type == "int") {
            code.put(str(boost::format("getIntOrElse(args, %d, %s)") % idx % evaluated));
        } else if (arg.type == "float") {
            code.put(str(boost::format("getFloatOrElse(args, %d, %s)") % idx % evaluated));
        } else if (arg.type == "string") {
            code.put(str(boost::format("getStringOrElse(args, %d, %s)") % idx % evaluated));
        } else if (arg.type == "vector") {
            code.put(str(boost::format("getVectorOrElse(args, %d, %s)") % idx % evaluated));
        } else if (arg.type == "object") {
            if (arg.defVal == "0" || arg.defVal == "OBJECT_SELF") {
                code.put(str(boost::format("getObjectOrCaller(args, %d, ctx)") % idx));
            } else if (arg.defVal == "1" || arg.defVal == "OBJECT_INVALID" || arg.defVal == "OBJECT_TYPE_INVALID") {
                code.put(str(boost::format("getObjectOrNull(args, %d, ctx)") % idx));
            } else {
                throw NotImplementedException(arg.defVal);
            }
        } else {
            throw NotImplementedException(arg.type);
        }
    } else {
        if (arg.type == "int") {
            code.put(str(boost::format("getInt(args, %d)") % idx));
        } else if (arg.type == "float") {
            code.put(str(boost::format("getFloat(args, %d)") % idx));
        } else if (arg.type == "string") {
            code.put(str(boost::format("getString(args, %d)") % idx));
        } else if (arg.type == "vector") {
            code.put(str(boost::format("getVector(args, %d)") % idx));
        } else if (arg.type == "object") {
            code.put(str(boost::format("getObject(args, %d, ctx)") % idx));
        } else if (arg.type == "object_id") {
            code.put(str(boost::format("getObject(args, %d, ctx)") % idx));
        } else if (arg.type == "effect") {
            code.put(str(boost::format("getEffect(args, %d)") % idx));
        } else if (arg.type == "event") {
            code.put(str(boost::format("getEvent(args, %d)") % idx));
        } else if (arg.type == "location") {
            code.put(str(boost::format("getLocationArgument(args, %d)") % idx));
        } else if (arg.type == "talent") {
            code.put(str(boost::format("getTalent(args, %d)") % idx));
        } else if (arg.type == "action") {
            code.put(str(boost::format("getAction(args, %d)") % idx));
        } else {
            throw NotImplementedException(arg.type);
        }
    }
}

void writeReoneRoutineInitArgExpr(int idx,
                                  const FunctionArgument &arg,
                                  const std::map<std::string, Constant> &constants,
                                  TextWriter &code) {
    code.put(REONE_INDENT + "auto " + arg.name + " = ");
    writeReoneRoutineGetArgExpr(idx, arg, constants, code);
    code.put(";\n");
}

void writeReoneRoutineImpl(const Function &func,
                           const std::map<std::string, Constant> &constants,
                           TextWriter &code) {
    code.put(str(boost::format("static Variable %s(const std::vector<Variable> &args, const RoutineContext &ctx) {\n") % func.name));
    if (!func.args.empty()) {
        code.put(REONE_INDENT + "// Load\n");
    }
    for (size_t i = 0; i < func.args.size(); ++i) {
        auto &arg = func.args[i];
        writeReoneRoutineInitArgExpr(i, arg, constants, code);
    }
    if (!func.args.empty()) {
        code.put("\n");
        code.put(REONE_INDENT + "// Transform\n\n");
    }
    code.put(REONE_INDENT + "// Execute\n");
    code.put(str(boost::format("%sthrow RoutineNotImplementedException(\"%s\");\n") % REONE_INDENT % func.name));
    code.put("}\n\n");
}

std::string nssTypeToMacro(const std::string &type) {
    if (type == "void") {
        return "R_VOID";
    } else if (type == "int") {
        return "R_INT";
    } else if (type == "float") {
        return "R_FLOAT";
    } else if (type == "string") {
        return "R_STRING";
    } else if (type == "vector") {
        return "R_VECTOR";
    } else if (type == "object") {
        return "R_OBJECT";
    } else if (type == "object_id") {
        return "R_OBJECT";
    } else if (type == "effect") {
        return "R_EFFECT";
    } else if (type == "event") {
        return "R_EVENT";
    } else if (type == "location") {
        return "R_LOCATION";
    } else if (type == "talent") {
        return "R_TALENT";
    } else if (type == "action") {
        return "R_ACTION";
    }
    throw NotImplementedException(type);
}

void writeReoneRegisterRoutinesFunc(const std::string &category,
                                    const std::string &game,
                                    const std::vector<std::tuple<int, Function>> &functions,
                                    TextWriter &code) {
    code.put(str(boost::format("void register%s%sRoutines(Routines &routines) {\n") % category % game));
    for (auto &[idx, func] : functions) {
        auto retType = nssTypeToMacro(func.retType);
        std::vector<std::string> args;
        for (auto &arg : func.args) {
            args.push_back(nssTypeToMacro(arg.type));
        }
        auto argsStr = boost::join(args, ", ");
        code.put(str(boost::format("%sroutines.insert(%d, \"%s\", %s, {%s}, &routine::%s);\n") % REONE_INDENT % idx % func.name % retType % argsStr % func.name));
    }
    code.put("}\n\n");
}

void writeReoneRoutineImplFile(const std::string &category,
                               const std::map<std::string, Constant> &constants,
                               const std::vector<std::tuple<int, Function>> &k1functions,
                               const std::vector<std::tuple<int, Function>> &k2functions,
                               const boost::filesystem::path &path) {
    auto stream = FileOutputStream(path);
    auto code = TextWriter(stream);
    code.put(REONE_COPYRIGHT_NOTICE + "\n\n");
    code.put("#include \"reone/game/script/routine/argutil.h\"\n");
    code.put("#include \"reone/game/script/routine/context.h\"\n");
    code.put("#include \"reone/game/script/routines.h\"\n");
    code.put("#include \"reone/script/routine/exception/notimplemented.h\"\n");
    code.put("#include \"reone/script/variable.h\"\n");
    code.put("\n");
    code.put("#define R_VOID script::VariableType::Void\n");
    code.put("#define R_INT script::VariableType::Int\n");
    code.put("#define R_FLOAT script::VariableType::Float\n");
    code.put("#define R_OBJECT script::VariableType::Object\n");
    code.put("#define R_STRING script::VariableType::String\n");
    code.put("#define R_EFFECT script::VariableType::Effect\n");
    code.put("#define R_EVENT script::VariableType::Event\n");
    code.put("#define R_LOCATION script::VariableType::Location\n");
    code.put("#define R_TALENT script::VariableType::Talent\n");
    code.put("#define R_VECTOR script::VariableType::Vector\n");
    code.put("#define R_ACTION script::VariableType::Action\n");
    code.put("\n");
    code.put("using namespace reone::script;\n");
    code.put("\n");
    code.put("namespace reone {\n\n");
    code.put("namespace game {\n\n");
    code.put("namespace routine {\n\n");
    for (auto &[_, func] : k2functions) {
        writeReoneRoutineImpl(func, constants, code);
    }
    code.put("} // namespace routine\n\n");
    writeReoneRegisterRoutinesFunc(category, "Kotor", k1functions, code);
    writeReoneRegisterRoutinesFunc(category, "Tsl", k2functions, code);
    code.put("} // namespace game\n\n");
    code.put("} // namespace reone\n\n");
}

int main(int argc, char **argv) {
    try {
        boost::program_options::options_description description;
        description.add_options()                                                               //
            ("k1nssfile", boost::program_options::value<boost::filesystem::path>()->required()) //
            ("k2nssfile", boost::program_options::value<boost::filesystem::path>()->required()) //
            ("destdir", boost::program_options::value<boost::filesystem::path>()->required());  //

        boost::program_options::positional_options_description positionalDesc;
        positionalDesc.add("k1nssfile", 1);
        positionalDesc.add("k2nssfile", 1);
        positionalDesc.add("destdir", 1);

        auto options = boost::program_options::command_line_parser(argc, argv)
                           .options(description)
                           .positional(positionalDesc)
                           .run();

        boost::program_options::variables_map vars;
        boost::program_options::store(options, vars);
        boost::program_options::notify(vars);

        auto &k1nssfile = vars["k1nssfile"].as<boost::filesystem::path>();
        if (!boost::filesystem::exists(k1nssfile) || !boost::filesystem::is_regular_file(k1nssfile)) {
            throw std::runtime_error("File not found: " + k1nssfile.string());
        }

        auto &k2nssfile = vars["k2nssfile"].as<boost::filesystem::path>();
        if (!boost::filesystem::exists(k2nssfile) || !boost::filesystem::is_regular_file(k1nssfile)) {
            throw std::runtime_error("File not found: " + k2nssfile.string());
        }

        auto &destdir = vars["destdir"].as<boost::filesystem::path>();
        if (!boost::filesystem::exists(destdir) || !boost::filesystem::is_directory(destdir)) {
            throw std::runtime_error("Destination directory does not exist: " + destdir.string());
        }

        auto [_, k1functions] = parseNwscriptNss(k1nssfile);
        std::vector<std::tuple<int, Function>> k1mainFunctions;
        std::vector<std::tuple<int, Function>> k1actionFunctions;
        std::vector<std::tuple<int, Function>> k1effectFunctions;
        std::vector<std::tuple<int, Function>> k1minigameFunctions;
        for (size_t i = 0; i < k1functions.size(); ++i) {
            auto &func = k1functions[i];
            if (boost::starts_with(func.name, "Action")) {
                k1actionFunctions.push_back(std::make_tuple(static_cast<int>(i), func));
            } else if (boost::starts_with(func.name, "Effect")) {
                k1effectFunctions.push_back(std::make_tuple(static_cast<int>(i), func));
            } else if (boost::starts_with(func.name, "SWMG_")) {
                k1minigameFunctions.push_back(std::make_tuple(static_cast<int>(i), func));
            } else {
                k1mainFunctions.push_back(std::make_tuple(static_cast<int>(i), func));
            }
        }

        auto [k2constants, k2functions] = parseNwscriptNss(k2nssfile);
        std::vector<std::tuple<int, Function>> k2mainFunctions;
        std::vector<std::tuple<int, Function>> k2actionFunctions;
        std::vector<std::tuple<int, Function>> k2effectFunctions;
        std::vector<std::tuple<int, Function>> k2minigameFunctions;
        for (size_t i = 0; i < k2functions.size(); ++i) {
            auto &func = k2functions[i];
            if (boost::starts_with(func.name, "Action")) {
                k2actionFunctions.push_back(std::make_tuple(static_cast<int>(i), func));
            } else if (boost::starts_with(func.name, "Effect")) {
                k2effectFunctions.push_back(std::make_tuple(static_cast<int>(i), func));
            } else if (boost::starts_with(func.name, "SWMG_")) {
                k2minigameFunctions.push_back(std::make_tuple(static_cast<int>(i), func));
            } else {
                k2mainFunctions.push_back(std::make_tuple(static_cast<int>(i), func));
            }
        }

        auto mainCppPath = destdir;
        mainCppPath.append("main.cpp");
        writeReoneRoutineImplFile("Main", k2constants, k1mainFunctions, k2mainFunctions, mainCppPath);

        auto actionCppPath = destdir;
        actionCppPath.append("action.cpp");
        writeReoneRoutineImplFile("Action", k2constants, k1actionFunctions, k2actionFunctions, actionCppPath);

        auto effectCppPath = destdir;
        effectCppPath.append("effect.cpp");
        writeReoneRoutineImplFile("Effect", k2constants, k1effectFunctions, k2effectFunctions, effectCppPath);

        auto minigameCppPath = destdir;
        minigameCppPath.append("minigame.cpp");
        writeReoneRoutineImplFile("Minigame", k2constants, k1minigameFunctions, k2minigameFunctions, minigameCppPath);

        return 0;

    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
}
