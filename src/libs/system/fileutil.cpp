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

#include "reone/system/fileutil.h"

namespace reone {

std::optional<std::filesystem::path> findFileIgnoreCase(const std::filesystem::path &dir, const std::string &relPath) {
    std::vector<std::string> tokens;
    boost::split(tokens, relPath, boost::is_any_of("/"), boost::token_compress_on);

    for (auto &entry : std::filesystem::directory_iterator(dir)) {
        auto filename = boost::to_lower_copy(entry.path().filename().string());
        if (filename == tokens[0]) {
            if (tokens.size() == 1) {
                return entry.path();
            }
            auto subPath = relPath.substr(tokens[0].length() + 1);
            return findFileIgnoreCase(entry.path(), subPath);
        }
    }

    return std::nullopt;
}

} // namespace reone
