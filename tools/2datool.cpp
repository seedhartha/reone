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

#include <boost/algorithm/string.hpp>
#include <boost/property_tree/json_parser.hpp>

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
    TwoDaFile twoDa;
    twoDa.load(path);

    pt::ptree tree;
    pt::ptree children;
    shared_ptr<TwoDaTable> table(twoDa.table());
    auto &headers = table->headers();
    auto &rows = table->rows();

    for (size_t i = 0; i < rows.size(); ++i) {
        pt::ptree child;
        child.put("_id", i);

        for (auto &value : table->rows()[i].values()) {
            child.put(value.first, value.second);
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

    auto table = make_unique<TwoDaTable>();

    for (auto &jsonRow : tree.get_child("rows")) {
        TwoDaRow row;
        for (auto &column : jsonRow.second) {
            // Columns starting with an underscore we consider meta and skip
            if (boost::starts_with(column.first, "_")) continue;

            row.add(column.first, column.second.data());
        }
        table->add(move(row));
    }

    vector<string> tokens;
    boost::split(tokens, path.filename().string(), boost::is_any_of("."), boost::token_compress_on);

    fs::path twoDaPath(destPath);
    twoDaPath.append(tokens[0] + ".2da");

    TwoDaWriter writer(move(table));
    writer.save(twoDaPath);
}

bool TwoDaTool::supports(Operation operation, const fs::path &target) const {
    return
        !fs::is_directory(target) &&
        ((target.extension() == ".2da" && operation == Operation::ToJSON) || (target.extension() == ".json" && operation == Operation::To2DA));
}

} // namespace tools

} // namespace reone
