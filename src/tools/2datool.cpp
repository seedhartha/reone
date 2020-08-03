/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include <boost/property_tree/json_parser.hpp>

#include "../resources/2dafile.h"

using namespace reone::resources;

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

namespace reone {

namespace tools {

void TwoDaTool::convert(const fs::path &path, const fs::path &destPath) const {
    TwoDaFile twoDa;
    twoDa.load(path);

    pt::ptree tree;
    pt::ptree children;
    std::shared_ptr<TwoDaTable> table(twoDa.table());
    auto &headers = table->headers();
    auto &rows = table->rows();

    for (auto &row : rows) {
        auto &values = row.values;
        pt::ptree child;
        for (int i = 0; i < headers.size(); ++i) {
            const std::string &val = values[i];
            child.put(headers[i], val);
        }
        children.push_back(std::make_pair("", child));
    }
    tree.add_child("rows", children);

    fs::path jsonPath(destPath);
    jsonPath.append(path.filename().string() + ".json");

    fs::ofstream json(jsonPath);
    pt::write_json(json, tree);
}

} // namespace tools

} // namespace reone
