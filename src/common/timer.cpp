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

#include "timer.h"

#include "glm/common.hpp"

namespace reone {

Timer::Timer(float timeout) {
    reset(timeout);
}

void Timer::reset(float timeout) {
    _time = timeout;
}

bool Timer::advance(float secs) {
    _time = glm::max(0.0f, _time - secs);
    return isTimedOut();
}

bool Timer::isTimedOut() const {
    return _time == 0.0f;
}

void Timer::cancel() {
    _time = 0.0f;
}

} // namespace reone
