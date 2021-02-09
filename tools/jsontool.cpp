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

#include "../src/common/log.h"
#include "../src/resource/format/2dafile.h"
#include "../src/resource/format/gfffile.h"

using namespace std;

using namespace reone::resource;

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

namespace reone {

namespace tools {

void JsonTool::to2DA(const fs::path &path, const fs::path &destPath) const {
    pt::ptree tree;
    pt::read_json(path.string(), tree);

    if (!tree.count("rows")) {
        warn("JSON: property not found: rows");
        return;
    }

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

void JsonTool::toGFF(const fs::path &path, const fs::path &destPath) const {
    pt::ptree tree;
    pt::read_json(path.string(), tree);
}

} // namespace tools

} // namespace reone
