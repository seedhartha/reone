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

#include "reone/resource/format/bifreader.h"
#include "reone/resource/format/keyreader.h"

#include "tool.h"

namespace reone {

class KeyBifTool : public Tool {
public:
    void invoke(
        Operation operation,
        const boost::filesystem::path &input,
        const boost::filesystem::path &outputDir,
        const boost::filesystem::path &gamePath) override;

    bool supports(Operation operation, const boost::filesystem::path &input) const override;

private:
    void listKEY(const resource::KeyReader &key);
    void listBIF(const resource::KeyReader &key, int bifIdx);
    void extractBIF(const resource::KeyReader &key, int bifIdx, const boost::filesystem::path &bifPath, const boost::filesystem::path &destPath);
};

} // namespace reone
