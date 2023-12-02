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

#include "reone/tools/script/format/pcodereader.h"

#include "reone/script/instrutil.h"
#include "reone/script/routines.h"
#include "reone/system/exception/validation.h"

namespace reone {

namespace script {

void PcodeReader::load() {
    std::vector<std::string> insLines;
    std::map<int, std::string> labelByLineIdx;
    std::map<int, uint32_t> addrByLineIdx;

    std::ifstream pcode(_path);
    std::string line;
    std::smatch what;
    std::regex labelRegex("^([_\\d\\w]+):$");
    uint32_t addr = 13;
    while (getline(pcode, line)) {
        auto addrSepIdx = line.find_first_of("\t");
        if (addrSepIdx != std::string::npos) {
            line = line.substr(addrSepIdx + 1);
        }
        boost::trim(line);
        if (line.empty()) {
            continue;
        }
        int lineIdx = static_cast<int>(insLines.size());
        if (std::regex_match(line, what, labelRegex)) {
            labelByLineIdx[lineIdx] = what[1].str();
            continue;
        }
        addrByLineIdx[lineIdx] = addr;
        addr += getInstructionSize(line);
        insLines.push_back(line);
    }

    std::filesystem::path filename(_path.filename());
    filename.replace_extension(); // drop .pcode
    filename.replace_extension(); // drop .ncs

    _addrByLabel.clear();
    for (auto &pair : labelByLineIdx) {
        _addrByLabel[pair.second] = addrByLineIdx[pair.first];
    }

    _program = std::make_shared<ScriptProgram>(filename.string());
    for (size_t i = 0; i < insLines.size(); ++i) {
        uint32_t insAddr = addrByLineIdx.find(static_cast<int>(i))->second;
        _program->add(parseInstruction(insLines[i], insAddr));
    }
}

int PcodeReader::getInstructionSize(const std::string &line) {
    int result = 2;

    size_t spaceIdx = line.find(" ");
    std::string typeDesc(spaceIdx != std::string::npos ? line.substr(0, spaceIdx) : line);
    std::string argsLine(line.substr(typeDesc.length()));
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

Instruction PcodeReader::parseInstruction(const std::string &line, uint32_t addr) const {
    size_t spaceIdx = line.find(" ");
    std::string typeDesc(spaceIdx != std::string::npos ? line.substr(0, spaceIdx) : line);
    std::string argsLine(line.substr(typeDesc.length()));
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
            const std::string &label = args[0];
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

    return ins;
}

void PcodeReader::applyArguments(const std::string &line, const std::string &restr, int numArgs, const std::function<void(const std::vector<std::string> &)> &fn) const {
    std::smatch what;
    std::regex re(restr);
    if (!std::regex_match(line, what, re)) {
        throw ValidationException(str(boost::format("Arguments line '%s' must match regular expression '%s'") % line % restr));
    }
    std::vector<std::string> args;
    for (int i = 0; i < numArgs; ++i) {
        args.push_back(what[1 + i].str());
    }
    fn(std::move(args));
}

} // namespace script

} // namespace reone
