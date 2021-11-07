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

#include "ssf.h"

#include <boost/property_tree/json_parser.hpp>

#include "../../engine/resource/format/ssfreader.h"
#include "../../engine/resource/format/ssfwriter.h"

using namespace std;

using namespace reone::resource;

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

namespace reone {

namespace tools {

void SsfTool::invoke(Operation operation, const fs::path &target, const fs::path &gamePath, const fs::path &destPath) {
    if (operation == Operation::ToJSON) {
        toJSON(target, destPath);
    } else if (operation == Operation::ToSSF) {
        toSSF(target, destPath);
    }
}

void SsfTool::toJSON(const fs::path &target, const fs::path &destPath) {
    SsfReader ssf;
    ssf.load(target);

    pt::ptree soundsTree;
    for (auto val : ssf.soundSet()) {
        pt::ptree soundTree;
        soundTree.put("", val);
        soundsTree.push_back(make_pair("", soundTree));
    }

    pt::ptree tree;
    tree.add_child("sounds", soundsTree);

    fs::path jsonPath(destPath);
    jsonPath.append(target.filename().string() + ".json");

    pt::write_json(jsonPath.string(), tree);
}

void SsfTool::toSSF(const fs::path &target, const fs::path &destPath) {
    pt::ptree tree;
    pt::read_json(target.string(), tree);

    vector<uint32_t> soundSet;
    for (auto soundTree : tree.get_child("sounds")) {
        soundSet.push_back(soundTree.second.get_value<uint32_t>());
    }

    fs::path ssfPath(destPath);
    ssfPath.append(target.filename().string());
    ssfPath.replace_extension(""); // drop .ssf

    SsfWriter ssf(soundSet);
    ssf.save(ssfPath);
}

bool SsfTool::supports(Operation operation, const fs::path &target) const {
    return (operation == Operation::ToJSON && target.extension() == ".ssf") ||
           (operation == Operation::ToSSF && target.extension() == ".json");
}

} // namespace tools

} // namespace reone
