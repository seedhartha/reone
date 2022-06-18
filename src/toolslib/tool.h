/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#pragma once

#include "types.h"

#include "../../common/exception/notimplemented.h"

namespace reone {

class ITool {
public:
    virtual void invoke(
        Operation operation,
        const boost::filesystem::path &input,
        const boost::filesystem::path &outputDir,
        const boost::filesystem::path &gamePath) = 0;

    virtual void invokeAll(
        Operation operation,
        const std::vector<boost::filesystem::path> &input,
        const boost::filesystem::path &outputDir,
        const boost::filesystem::path &gamePath) {
    
        throw NotImplementedException("Batch tool invocation not implemented");
    }

    virtual bool supports(Operation operation, const boost::filesystem::path &input) const = 0;

    bool supportsAll(Operation operation, const std::vector<boost::filesystem::path> &input) const {
        for (auto &path : input) {
            if (!supports(operation, path)) {
                return false;
            }
        }
        return true;
    }
};

} // namespace reone
