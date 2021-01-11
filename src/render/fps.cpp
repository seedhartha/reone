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

#include "fps.h"

#include "SDL2/SDL.h"

namespace reone {

namespace render {

static const int kMeasurePeriodMillis = 500;

void FpsCounter::reset() {
    _startTicks = SDL_GetTicks();
    _frameCount = 0;
    _hasAverage = false;
}

void FpsCounter::update(float dt) {
    ++_frameCount;

    uint32_t ticks = SDL_GetTicks();
    if (_startTicks == 0) {
        _startTicks = ticks;
        return;
    }

    uint32_t delta = ticks - _startTicks;
    if (delta > kMeasurePeriodMillis) {
        _average = _frameCount / (delta / 1000.0f);
        _hasAverage = true;
    }
}

bool FpsCounter::hasAverage() const {
    return _hasAverage;
}

float FpsCounter::average() const {
    return _average;
}

} // namespace render

} // namespace reone
