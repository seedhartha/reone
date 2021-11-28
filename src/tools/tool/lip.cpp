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

#include "lip.h"

#include <boost/property_tree/json_parser.hpp>

#include "../../graphics/lip/format/lipreader.h"
#include "../../graphics/lip/format/lipwriter.h"

using namespace std;

using namespace reone::graphics;

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

namespace reone {

namespace tools {

void LipTool::invoke(Operation operation, const fs::path &target, const fs::path &gamePath, const fs::path &destPath) {
    if (operation == Operation::ToJSON) {
        toJSON(target, destPath);
    } else if (operation == Operation::ToLIP) {
        toLIP(target, destPath);
    }
}

void LipTool::toJSON(const fs::path &path, const fs::path &destPath) {
    pt::ptree tree;
    pt::ptree keyframes;

    LipReader lip;
    lip.load(path);

    shared_ptr<LipAnimation> animation(lip.animation());
    tree.put("length", animation->length());
    for (auto &keyframe : animation->keyframes()) {
        pt::ptree keyframeTree;
        keyframeTree.put("time", keyframe.time);
        keyframeTree.put("shape", keyframe.shape);
        keyframes.push_back(make_pair("", keyframeTree));
    }
    tree.add_child("keyframes", keyframes);

    fs::path jsonPath(destPath);
    jsonPath.append(path.filename().string() + ".json");

    pt::write_json(jsonPath.string(), tree);
}

void LipTool::toLIP(const fs::path &path, const fs::path &destPath) {
    pt::ptree tree;
    pt::read_json(path.string(), tree);

    float length = tree.get("length", 0.0f);
    vector<LipAnimation::Keyframe> keyframes;
    for (auto &keyframeTree : tree.get_child("keyframes")) {
        LipAnimation::Keyframe keyframe;
        keyframe.time = keyframeTree.second.get("time", 0.0f);
        keyframe.shape = keyframeTree.second.get("shape", 0);
        keyframes.push_back(move(keyframe));
    }
    LipAnimation animation(length, move(keyframes));

    fs::path lipPath(destPath);
    lipPath.append(path.filename().string());
    lipPath.replace_extension(); // remove .json

    LipWriter writer(move(animation));
    writer.save(lipPath);
}

bool LipTool::supports(Operation operation, const fs::path &target) const {
    return !fs::is_directory(target) &&
           ((target.extension() == ".lip" && operation == Operation::ToJSON) || (target.extension() == ".json" && operation == Operation::ToLIP));
}

} // namespace tools

} // namespace reone
