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

#include "profiler.h"

#include "../../common/logutil.h"

#include "../profiler.h"

namespace reone {

namespace game {

static constexpr float kFlushInterval = 1.0f;

void ProfilerGui::init() {
}

void ProfilerGui::update(float delta) {
    if (_flushTimer.advance(delta)) {
        debug(boost::format("FPS: average=%u, 0.1%%=%u") % _profiler.averageFps() % _profiler.onePercentLowFps());
    
        auto &inputTimes = _profiler.inputTimes();
        auto &updateTimes = _profiler.updateTimes();
        auto &renderTimes = _profiler.renderTimes();
        debug(boost::format("Last frame times: input=%.04f, update=%.04f, render=%.04f") % inputTimes.back() % updateTimes.back() % renderTimes.back());

        _flushTimer.setTimeout(kFlushInterval);
    }
}

} // namespace game

} // namespace reone
