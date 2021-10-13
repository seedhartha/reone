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

#include "2da.h"

#include <boost/property_tree/json_parser.hpp>

#include "../../engine/common/streamwriter.h"
#include "../../engine/resource/2da.h"
#include "../../engine/resource/format/2dareader.h"
#include "../../engine/resource/format/2dawriter.h"

using namespace std;

using namespace reone::resource;

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

namespace reone {

namespace tools {

void TwoDaTool::invoke(Operation operation, const fs::path &target, const fs::path &gamePath, const fs::path &destPath) {
    if (operation == Operation::ToJSON) {
        toJSON(target, destPath);
    } else {
        to2DA(target, destPath);
    }
}

void TwoDaTool::toJSON(const fs::path &path, const fs::path &destPath) {
    TwoDaReader twoDaFile;
    twoDaFile.load(path);

    pt::ptree tree;
    pt::ptree children;
    shared_ptr<TwoDA> twoDa(twoDaFile.twoDa());

    for (int row = 0; row < twoDa->getRowCount(); ++row) {
        pt::ptree child;
        child.put("_id", row);

        for (int col = 0; col < twoDa->getColumnCount(); ++col) {
            child.put(twoDa->columns()[col], twoDa->rows()[row].values[col]);
        }
        children.push_back(make_pair("", child));
    }
    tree.add_child("rows", children);

    fs::path jsonPath(destPath);
    jsonPath.append(path.filename().string() + ".json");

    pt::write_json(jsonPath.string(), tree);
}

void TwoDaTool::to2DA(const fs::path &path, const fs::path &destPath) {
    pt::ptree tree;
    pt::read_json(path.string(), tree);

    auto twoDa = make_unique<TwoDA>();
    bool columnsInited = false;

    for (auto &jsonRow : tree.get_child("rows")) {
        TwoDA::Row row;

        for (auto &prop : jsonRow.second) {
            // Properties with a leading underscore are metadata
            if (boost::starts_with(prop.first, "_"))
                continue;

            if (!columnsInited) {
                twoDa->addColumn(prop.first);
            }
            row.values.push_back(prop.second.data());
        }

        twoDa->add(move(row));
        columnsInited = true;
    }

    vector<string> tokens;
    boost::split(tokens, path.filename().string(), boost::is_any_of("."), boost::token_compress_on);

    fs::path twoDaPath(destPath);
    twoDaPath.append(tokens[0] + ".2da");

    TwoDaWriter writer(move(twoDa));
    writer.save(twoDaPath);
}

bool TwoDaTool::supports(Operation operation, const fs::path &target) const {
    return !fs::is_directory(target) &&
           ((target.extension() == ".2da" && operation == Operation::ToJSON) || (target.extension() == ".json" && operation == Operation::To2DA));
}

} // namespace tools

} // namespace reone
