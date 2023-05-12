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

#include "clock.h"

namespace reone {

struct SystemServices {
    IClock &clock;

    SystemServices(IClock &clock) :
        clock(clock) {
    }

    Clock &defaultClock() {
        auto casted = dynamic_cast<Clock *>(&clock);
        if (!casted) {
            throw std::logic_error("Illegal Clock implementation");
        }
        return *casted;
    }
};

} // namespace reone
