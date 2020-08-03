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

#include "program.h"

namespace reone {

namespace script {

class ScriptManager {
public:
    static ScriptManager &instance();

    std::shared_ptr<ScriptProgram> find(const std::string &name);

private:
    ScriptManager() = default;
    ScriptManager(const ScriptManager &) = delete;
    ScriptManager &operator=(const ScriptManager &) = delete;
};

#define ScriptMan script::ScriptManager::instance()

} // namespace script

} // namespace reone
