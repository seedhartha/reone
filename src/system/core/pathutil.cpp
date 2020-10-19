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

#include "pathutil.h"

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "log.h"

using namespace std;

namespace fs = boost::filesystem;

namespace reone {

fs::path getPathIgnoreCase(const fs::path &basePath, const string &relPath, bool logNotFound) {
    vector<string> tokens;
    boost::split(tokens, relPath, boost::is_any_of("/"), boost::token_compress_on);

    for (auto &entry : fs::directory_iterator(basePath)) {
        string filename(entry.path().filename().string());
        boost::to_lower(filename);

        if (filename == tokens[0]) {
            if (tokens.size() == 1) {
                return entry.path();
            }
            string relPath2(relPath.substr(tokens[0].length() + 1));

            return getPathIgnoreCase(entry.path(), relPath2);
        }
    }
    if (logNotFound) {
        debug(boost::format("Path not found: %s %s") % basePath % relPath);
    }

    return "";
}

} // namespace reone
