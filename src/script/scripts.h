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

#pragma once

#include <string>
#include <memory>
#include <unordered_map>

#include "../resource/types.h"

namespace reone {

namespace script {

class ScriptProgram;

class Scripts {
public:
    static Scripts &instance();

    void invalidateCache();

    std::shared_ptr<ScriptProgram> get(const std::string &resRef);

private:
    std::unordered_map<std::string, std::shared_ptr<ScriptProgram>> _cache;

    Scripts() = default;
    Scripts(const Scripts &) = delete;
    Scripts &operator=(const Scripts &) = delete;

    std::shared_ptr<ScriptProgram> doGet(const std::string &resRef);
};

} // namespace script

} // namespace reone
