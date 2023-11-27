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

#include "routines.h"

#include "reone/resource/provider/keybif.h"
#include "reone/system/exception/notimplemented.h"
#include "reone/system/fileutil.h"
#include "reone/system/stream/fileoutput.h"
#include "reone/system/stream/memoryinput.h"
#include "reone/system/textreader.h"
#include "reone/system/textwriter.h"

#include "templates.h"

using namespace reone::resource;

namespace reone {

static const std::regex kConstRegex = std::regex("^(\\w+)\\s+(\\w+)\\s*=(.*);.*");
static const std::regex kConstVecRegex = std::regex("^\\[\\s*([\\.\\d]+f?),\\s*([\\.\\d]+f?),\\s*([\\.\\d]+f?)\\s*\\]$");

static const std::regex kFuncDeclRegex = std::regex("^(\\w+)\\s+(\\w+)\\s*\\((.*)\\);$");
static const std::regex kFuncArgRegEx = std::regex("^(\\w+)\\s+(\\w+)(.*)$");
static const std::regex kFuncArgDefValRegEx = std::regex("^(\\w+)\\s+(\\w+)\\s*=(.*)$");

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

static Constant parseConstant(const std::smatch &match) {
    auto type = boost::to_lower_copy(match[1].str());
    auto name = match[2].str();
    auto value = boost::trim_copy(match[3].str());
    return Constant(type, name, value);
}

static FunctionArgument parseFunctionArgument(const std::string &str) {
    std::smatch match;
    if (!std::regex_search(str, match, kFuncArgRegEx)) {
        throw std::invalid_argument("Invalid function argument format: " + str);
    }
    auto type = boost::to_lower_copy(match[1].str());
    auto name = match[2].str();
    std::smatch defValMatch;
    std::string defVal;
    if (std::regex_search(str, defValMatch, kFuncArgDefValRegEx)) {
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

static Function parseFunction(const std::smatch &match) {
    auto retType = boost::to_lower_copy(match[1].str());
    auto name = match[2].str();
    auto argsStr = match[3].str();
    std::vector<FunctionArgument> args;
    if (!argsStr.empty()) {
        args = parseFunctionArguments(argsStr);
    }
    return Function(retType, name, std::move(args));
}

static std::tuple<std::map<std::string, Constant>, std::vector<Function>> parseNwscriptNss(IInputStream &nss) {
    std::map<std::string, Constant> constants;
    std::vector<Function> functions;
    auto reader = TextReader(nss);
    while (auto line = reader.readLine()) {
        if (!line) {
            break;
        }
        std::smatch constMatch;
        if (std::regex_search(*line, constMatch, kConstRegex)) {
            auto constant = parseConstant(constMatch);
            constants[constant.name] = std::move(constant);
        }
        std::smatch funcMatch;
        if (std::regex_search(*line, funcMatch, kFuncDeclRegex)) {
            functions.push_back(parseFunction(funcMatch));
        }
    }
    return std::make_tuple(constants, functions);
}

static std::string evaluateConstant(const std::string &constStr, const std::map<std::string, Constant> &constants) {
    if (constants.count(constStr) > 0) {
        return constants.at(constStr).value;
    } else {
        std::smatch vecMatch;
        if (std::regex_search(constStr, vecMatch, kConstVecRegex)) {
            auto x = vecMatch[1].str();
            auto y = vecMatch[2].str();
            auto z = vecMatch[3].str();
            return str(boost::format("glm::vec3(%ff, %ff, %ff)") % x % y % z);
        } else {
            return constStr;
        }
    }
}

static void writeReoneRoutineGetArgExpr(int idx,
                                        const FunctionArgument &arg,
                                        const std::map<std::string, Constant> &constants,
                                        TextWriter &code) {
    if (!arg.defVal.empty()) {
        auto evaluated = evaluateConstant(arg.defVal, constants);
        if (arg.type == "int") {
            code.write(str(boost::format("getIntOrElse(args, %d, %s)") % idx % evaluated));
        } else if (arg.type == "float") {
            code.write(str(boost::format("getFloatOrElse(args, %d, %s)") % idx % evaluated));
        } else if (arg.type == "string") {
            code.write(str(boost::format("getStringOrElse(args, %d, %s)") % idx % evaluated));
        } else if (arg.type == "vector") {
            code.write(str(boost::format("getVectorOrElse(args, %d, %s)") % idx % evaluated));
        } else if (arg.type == "object") {
            if (arg.defVal == "0" || arg.defVal == "OBJECT_SELF") {
                code.write(str(boost::format("getObjectOrCaller(args, %d, ctx)") % idx));
            } else if (arg.defVal == "1" || arg.defVal == "OBJECT_INVALID" || arg.defVal == "OBJECT_TYPE_INVALID") {
                code.write(str(boost::format("getObjectOrNull(args, %d, ctx)") % idx));
            } else {
                throw NotImplementedException(arg.defVal);
            }
        } else {
            throw NotImplementedException(arg.type);
        }
    } else {
        if (arg.type == "int") {
            code.write(str(boost::format("getInt(args, %d)") % idx));
        } else if (arg.type == "float") {
            code.write(str(boost::format("getFloat(args, %d)") % idx));
        } else if (arg.type == "string") {
            code.write(str(boost::format("getString(args, %d)") % idx));
        } else if (arg.type == "vector") {
            code.write(str(boost::format("getVector(args, %d)") % idx));
        } else if (arg.type == "object") {
            code.write(str(boost::format("getObject(args, %d, ctx)") % idx));
        } else if (arg.type == "object_id") {
            code.write(str(boost::format("getObject(args, %d, ctx)") % idx));
        } else if (arg.type == "effect") {
            code.write(str(boost::format("getEffect(args, %d)") % idx));
        } else if (arg.type == "event") {
            code.write(str(boost::format("getEvent(args, %d)") % idx));
        } else if (arg.type == "location") {
            code.write(str(boost::format("getLocationArgument(args, %d)") % idx));
        } else if (arg.type == "talent") {
            code.write(str(boost::format("getTalent(args, %d)") % idx));
        } else if (arg.type == "action") {
            code.write(str(boost::format("getAction(args, %d)") % idx));
        } else {
            throw NotImplementedException(arg.type);
        }
    }
}

static void writeReoneRoutineInitArgExpr(int idx,
                                         const FunctionArgument &arg,
                                         const std::map<std::string, Constant> &constants,
                                         TextWriter &code) {
    code.write(kIndent + "auto " + arg.name + " = ");
    writeReoneRoutineGetArgExpr(idx, arg, constants, code);
    code.write(";\n");
}

static void writeReoneRoutineImpl(const Function &func,
                                  const std::map<std::string, Constant> &constants,
                                  TextWriter &code) {
    code.write(str(boost::format("static Variable %s(const std::vector<Variable> &args, const RoutineContext &ctx) {\n") % func.name));
    if (!func.args.empty()) {
        code.write(kIndent + "// Load\n");
    }
    for (size_t i = 0; i < func.args.size(); ++i) {
        auto &arg = func.args[i];
        writeReoneRoutineInitArgExpr(i, arg, constants, code);
    }
    if (!func.args.empty()) {
        code.write("\n");
        code.write(kIndent + "// Transform\n\n");
    }
    code.write(kIndent + "// Execute\n");
    code.write(str(boost::format("%sthrow RoutineNotImplementedException(\"%s\");\n") % kIndent % func.name));
    code.write("}\n\n");
}

static std::string nssTypeToMacro(const std::string &type) {
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

static void writeReoneRegisterRoutinesFunc(const std::string &category,
                                           const std::string &game,
                                           const std::vector<std::tuple<int, Function>> &functions,
                                           TextWriter &code) {
    code.write(str(boost::format("void Routines::register%s%sRoutines() {\n") % category % game));
    for (auto &[idx, func] : functions) {
        auto retType = nssTypeToMacro(func.retType);
        std::vector<std::string> args;
        for (auto &arg : func.args) {
            args.push_back(nssTypeToMacro(arg.type));
        }
        auto argsStr = boost::join(args, ", ");
        code.write(str(boost::format("%sinsert(%d, \"%s\", %s, {%s}, &%s);\n") % kIndent % idx % func.name % retType % argsStr % func.name));
    }
    code.write("}\n\n");
}

static void writeReoneRoutineImplFile(const std::string &category,
                                      const std::map<std::string, Constant> &constants,
                                      const std::vector<std::tuple<int, Function>> &k1functions,
                                      const std::vector<std::tuple<int, Function>> &k2functions,
                                      const std::filesystem::path &path) {
    auto stream = FileOutputStream(path);
    auto code = TextWriter(stream);
    code.write(kCopyrightNotice + "\n\n");
    code.write(str(boost::format(kIncludeFormat + "\n") % "reone/game/script/routine/argutil.h"));
    code.write(str(boost::format(kIncludeFormat + "\n") % "reone/game/script/routine/context.h"));
    code.write(str(boost::format(kIncludeFormat + "\n") % "reone/game/script/routines.h"));
    code.write(str(boost::format(kIncludeFormat + "\n") % "reone/script/routine/exception/notimplemented.h"));
    code.write(str(boost::format(kIncludeFormat + "\n") % "reone/script/variable.h"));
    code.write("\n");
    code.write("#define R_VOID script::VariableType::Void\n");
    code.write("#define R_INT script::VariableType::Int\n");
    code.write("#define R_FLOAT script::VariableType::Float\n");
    code.write("#define R_OBJECT script::VariableType::Object\n");
    code.write("#define R_STRING script::VariableType::String\n");
    code.write("#define R_EFFECT script::VariableType::Effect\n");
    code.write("#define R_EVENT script::VariableType::Event\n");
    code.write("#define R_LOCATION script::VariableType::Location\n");
    code.write("#define R_TALENT script::VariableType::Talent\n");
    code.write("#define R_VECTOR script::VariableType::Vector\n");
    code.write("#define R_ACTION script::VariableType::Action\n");
    code.write("\n");
    code.write("using namespace reone::script;\n");
    code.write("\n");
    code.write("namespace reone {\n\n");
    code.write("namespace game {\n\n");
    for (auto &[_, func] : k2functions) {
        writeReoneRoutineImpl(func, constants, code);
    }
    writeReoneRegisterRoutinesFunc(category, "Kotor", k1functions, code);
    writeReoneRegisterRoutinesFunc(category, "Tsl", k2functions, code);
    code.write("} // namespace game\n\n");
    code.write("} // namespace reone\n\n");
}

void generateRoutines(const std::filesystem::path &k1Dir,
                      const std::filesystem::path &k2Dir,
                      const std::filesystem::path &destDir) {
    auto k1KeyPath = findFileIgnoreCase(k1Dir, "chitin.key");
    if (!k1KeyPath) {
        throw std::runtime_error("KotOR chitin.key file not found");
    }
    auto k1KeyBif = KeyBifResourceProvider(*k1KeyPath);
    k1KeyBif.init();
    auto k1NssBytes = k1KeyBif.findResourceData(ResourceId("nwscript", ResType::Nss));
    if (!k1NssBytes) {
        throw std::runtime_error("KotOR nwscript.nss resource not found");
    }
    auto k1Nss = MemoryInputStream(*k1NssBytes);
    auto [_, k1functions] = parseNwscriptNss(k1Nss);
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

    auto k2KeyPath = findFileIgnoreCase(k2Dir, "chitin.key");
    if (!k2KeyPath) {
        throw std::runtime_error("TSL chitin.key file not found");
    }
    auto k2KeyBif = KeyBifResourceProvider(*k1KeyPath);
    k2KeyBif.init();
    auto k2NssBytes = k2KeyBif.findResourceData(ResourceId("nwscript", ResType::Nss));
    if (!k2NssBytes) {
        throw std::runtime_error("TSL nwscript.nss resource not found");
    }
    auto k2Nss = MemoryInputStream(*k2NssBytes);
    auto [k2constants, k2functions] = parseNwscriptNss(k2Nss);
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

    auto mainCppPath = destDir;
    mainCppPath.append("main.cpp");
    writeReoneRoutineImplFile("Main", k2constants, k1mainFunctions, k2mainFunctions, mainCppPath);

    auto actionCppPath = destDir;
    actionCppPath.append("action.cpp");
    writeReoneRoutineImplFile("Action", k2constants, k1actionFunctions, k2actionFunctions, actionCppPath);

    auto effectCppPath = destDir;
    effectCppPath.append("effect.cpp");
    writeReoneRoutineImplFile("Effect", k2constants, k1effectFunctions, k2effectFunctions, effectCppPath);

    auto minigameCppPath = destDir;
    minigameCppPath.append("minigame.cpp");
    writeReoneRoutineImplFile("Minigame", k2constants, k1minigameFunctions, k2minigameFunctions, minigameCppPath);
}

} // namespace reone
