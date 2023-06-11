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

#include "reone/system/stream/input.h"
#include "reone/system/stream/output.h"

#include "tool.h"

namespace reone {

class TpcTool : public Tool {
public:
    void invoke(
        Operation operation,
        const boost::filesystem::path &input,
        const boost::filesystem::path &outputDir,
        const boost::filesystem::path &gamePath) override;

    void invokeBatch(
        Operation operation,
        const std::vector<boost::filesystem::path> &input,
        const boost::filesystem::path &outputDir,
        const boost::filesystem::path &gamePath) override;

    bool supports(Operation operation, const boost::filesystem::path &input) const override;

    void toTGA(IInputStream &tpc, IOutputStream &tga, IOutputStream &txi);

private:
    void toTGA(const boost::filesystem::path &path, const boost::filesystem::path &destPath);
};

} // namespace reone
