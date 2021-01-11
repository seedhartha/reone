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

#include <cstdint>

namespace reone {

namespace script {

class ScriptObject {
public:
    virtual ~ScriptObject() = default;

    uint32_t id() const { return _id; }

protected:
    uint32_t _id { 0 };

    ScriptObject(uint32_t id) : _id(id) { }

private:
    ScriptObject(const ScriptObject &) = delete;
    ScriptObject &operator=(const ScriptObject &) = delete;
};

} // namespace script

} // namespae reone
