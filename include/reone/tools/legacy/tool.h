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

#pragma once

#include "reone/resource/exception/format.h"
#include "reone/system/exception/notimplemented.h"
#include "reone/system/logutil.h"

#include "../types.h"

namespace reone {

class Tool {
public:
    virtual void invoke(
        Operation operation,
        const std::filesystem::path &input,
        const std::filesystem::path &outputDir,
        const std::filesystem::path &gamePath) = 0;

    virtual void invokeBatch(
        Operation operation,
        const std::vector<std::filesystem::path> &input,
        const std::filesystem::path &outputDir,
        const std::filesystem::path &gamePath) {

        throw NotImplementedException("invokeBatch not implemented");
    }

    virtual bool supports(Operation operation, const std::filesystem::path &input) const = 0;

protected:
    void doInvokeBatch(
        const std::vector<std::filesystem::path> &input,
        const std::filesystem::path &outputDir,
        std::function<void(const std::filesystem::path &, const std::filesystem::path &)> block) {

        for (auto &path : input) {
            auto outDir = outputDir;
            if (outDir.empty()) {
                outDir = path.parent_path();
            }
            try {
                block(path, outDir);
            } catch (const resource::FormatException &e) {
                error(boost::format("Error while processing '%s': %s") % path % std::string(e.what()));
            }
        }
    }
};

} // namespace reone
