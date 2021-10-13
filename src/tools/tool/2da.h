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

#pragma once

#include "itool.h"

namespace reone {

namespace tools {

class TwoDaTool : public ITool {
public:
    void invoke(
        Operation operation,
        const boost::filesystem::path &target,
        const boost::filesystem::path &gamePath,
        const boost::filesystem::path &destPath) override;

    bool supports(Operation operation, const boost::filesystem::path &target) const override;

private:
    void toJSON(const boost::filesystem::path &path, const boost::filesystem::path &destPath);
    void to2DA(const boost::filesystem::path &path, const boost::filesystem::path &destPath);
};

} // namespace tools

} // namespace reone
