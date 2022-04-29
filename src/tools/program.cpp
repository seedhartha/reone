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

#include "program.h"

#include "../common/pathutil.h"
#include "../resource/types.h"

#include "tool/2da.h"
#include "tool/audio.h"
#include "tool/erf.h"
#include "tool/gff.h"
#include "tool/keybif.h"
#include "tool/lip.h"
#include "tool/ncs.h"
#include "tool/rim.h"
#include "tool/ssf.h"
#include "tool/tlk.h"
#include "tool/tpc.h"

using namespace std;

using namespace reone::game;
using namespace reone::resource;

namespace fs = boost::filesystem;
namespace po = boost::program_options;

namespace reone {

static const unordered_map<string, Operation> g_operations {
    {"list", Operation::List},
    {"extract", Operation::Extract},
    {"unwrap", Operation::Unwrap},
    {"to-json", Operation::ToJSON},
    {"to-xml", Operation::ToXML},
    {"to-tga", Operation::ToTGA},
    {"to-2da", Operation::To2DA},
    {"to-gff", Operation::ToGFF},
    {"to-rim", Operation::ToRIM},
    {"to-erf", Operation::ToERF},
    {"to-mod", Operation::ToMOD},
    {"to-tlk", Operation::ToTLK},
    {"to-lip", Operation::ToLIP},
    {"to-pcode", Operation::ToPCODE},
    {"to-ncs", Operation::ToNCS},
    {"to-ssf", Operation::ToSSF}};

static fs::path getDestination(const po::variables_map &vars) {
    fs::path result;
    if (vars.count("dest") > 0) {
        result = vars["dest"].as<string>();
    } else if (vars.count("target") > 0) {
        result = fs::path(vars["target"].as<string>()).parent_path();
    } else {
        result = fs::current_path();
    }
    return move(result);
}

int Program::run() {
    initOptions();
    parseOptions();
    loadOptions();
    loadTools();

    switch (_operation) {
    case Operation::None:
        cout << _optsCmdLine << endl;
        break;
    default: {
        auto tool = getTool();
        if (tool) {
            tool->invoke(_operation, _target, _gamePath, _destPath);
        } else {
            cout << "Unable to choose a tool for the specified operation" << endl;
        }
        break;
    }
    }

    return 0;
}

void Program::initOptions() {
    _optsCmdLine.add_options()                                                             //
        ("game", po::value<string>(), "path to game directory")                            //
        ("dest", po::value<string>(), "path to destination directory")                     //
        ("tsl", po::value<bool>()->default_value(false), "is (dis)assembled NCS for TSL?") //
        ("list", "list file contents")("extract", "extract file contents")                 //
        ("unwrap", "unwrap an audio file")                                                 //
        ("to-json", "convert 2DA, TLK, LIP or SSF to JSON")                                //
        ("to-xml", "convert GFF to XML")                                                   //
        ("to-tga", "convert TPC image to TGA")                                             //
        ("to-2da", "convert JSON to 2DA")                                                  //
        ("to-gff", "convert JSON or XML to GFF")                                           //
        ("to-rim", "create RIM archive from directory")                                    //
        ("to-erf", "create ERF archive from directory")                                    //
        ("to-mod", "create MOD archive from directory")                                    //
        ("to-tlk", "convert JSON to TLK")                                                  //
        ("to-lip", "convert JSON to LIP")                                                  //
        ("to-pcode", "convert NCS to PCODE")                                               //
        ("to-ncs", "convert PCODE to NCS")                                                 //
        ("to-ssf", "convert JSON to SSF")                                                  //
        ("target", po::value<string>(), "target name or path to input file");
}

void Program::parseOptions() {
    po::positional_options_description positional;
    positional.add("target", 1);

    po::parsed_options parsedCmdLineOpts = po::command_line_parser(_argc, _argv)
                                               .options(_optsCmdLine)
                                               .allow_unregistered()
                                               .positional(positional)
                                               .run();

    po::store(parsedCmdLineOpts, _variables);
    po::notify(_variables);
}

void Program::loadOptions() {
    _gamePath = _variables.count("game") > 0 ? _variables["game"].as<string>() : fs::current_path();
    _destPath = getDestination(_variables);
    _target = _variables.count("target") > 0 ? _variables["target"].as<string>() : "";

    if (_variables["tsl"].as<bool>()) {
        _gameId = GameID::TSL;
    } else {
        _gameId = GameID::KotOR;
    }

    // Determine operation from program options
    for (auto &operation : g_operations) {
        if (_variables.count(operation.first)) {
            _operation = operation.second;
            break;
        }
    }
}

void Program::loadTools() {
    // Tools are queried in the order of addition, whether they support a
    // particular operation on a particular file, or not. The first tool
    // to return true gets chosen.

    _tools.push_back(make_shared<KeyBifTool>());
    _tools.push_back(make_shared<ErfTool>());
    _tools.push_back(make_shared<RimTool>());
    _tools.push_back(make_shared<TwoDaTool>());
    _tools.push_back(make_shared<TlkTool>());
    _tools.push_back(make_shared<LipTool>());
    _tools.push_back(make_shared<SsfTool>());
    _tools.push_back(make_shared<GffTool>());
    _tools.push_back(make_shared<TpcTool>());
    _tools.push_back(make_shared<AudioTool>());
    _tools.push_back(make_shared<NcsTool>(_gameId));
}

shared_ptr<ITool> Program::getTool() const {
    for (auto &tool : _tools) {
        if (tool->supports(_operation, _target))
            return tool;
    }
    return nullptr;
}

} // namespace reone
