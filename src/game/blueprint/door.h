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

#include <string>
#include <memory>

#include <boost/noncopyable.hpp>

#include "../../resource/format/gfffile.h"

namespace reone {

namespace game {

class Door;

class DoorBlueprint : boost::noncopyable {
public:
    DoorBlueprint(const std::string &resRef, const std::shared_ptr<resource::GffStruct> &utd);

    void load(Door &door);

    const std::string &resRef() const { return _resRef; }

private:
    std::string _resRef;
    std::shared_ptr<resource::GffStruct> _utd;

    void loadScripts(Door &door);
};

} // namespace game

} // namespace reone
