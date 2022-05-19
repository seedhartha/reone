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

#include "../../common/stream/input.h"

#include "../types.h"

namespace reone {

namespace game {

class VisReader : boost::noncopyable {
public:
    void load(const std::shared_ptr<IInputStream> &in);

    const game::Visibility &visibility() const { return _visibility; }

private:
    game::Visibility _visibility;
    std::string _roomFrom;

    void processLine(const std::string &line);
};

} // namespace game

} // namespace reone
