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

#include "tool.h"

#include "reone/system/stream/input.h"
#include "reone/system/stream/output.h"

namespace reone {

class TwoDaTool : public Tool {
public:
    void invoke(
        Operation operation,
        const std::filesystem::path &input,
        const std::filesystem::path &outputDir,
        const std::filesystem::path &gamePath) override;

    void invokeBatch(
        Operation operation,
        const std::vector<std::filesystem::path> &input,
        const std::filesystem::path &outputDir,
        const std::filesystem::path &gamePath) override;

    bool supports(Operation operation, const std::filesystem::path &input) const override;

    void toXML(IInputStream &twoDa, IOutputStream &xml);

private:
    void toXML(const std::filesystem::path &input, const std::filesystem::path &outputDir);
    void to2DA(const std::filesystem::path &input, const std::filesystem::path &outputDir);
};

} // namespace reone
