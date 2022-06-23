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

namespace reone {

namespace game {

static constexpr int kMaxFrames = 100;
static constexpr int kAnalyzeFpsIntervalMillis = 1000;

class Profiler {
public:
    void init() {
        _perfFreq = SDL_GetPerformanceFrequency();
    }

    void startFrame() {
        _perfCntFrameStart = SDL_GetPerformanceCounter();
    }

    void startInput() {
        _perfCntInputStart = SDL_GetPerformanceCounter();
    }

    void endInput() {
        _perfCntInputEnd = SDL_GetPerformanceCounter();
    }

    void startUpdate() {
        _perfCntUpdateStart = SDL_GetPerformanceCounter();
    }

    void endUpdate() {
        _perfCntUpdateEnd = SDL_GetPerformanceCounter();
    }

    void startRender() {
        _perfCntRenderStart = SDL_GetPerformanceCounter();
    }

    void endRender() {
        _perfCntRenderEnd = SDL_GetPerformanceCounter();
    }

    void endFrame() {
        _perfCntFrameEnd = SDL_GetPerformanceCounter();
        analyze();
    }

    const std::list<float> &frameTimes() const {
        return _frameTimes;
    }

    const std::list<float> &inputTimes() const {
        return _inputTimes;
    }

    const std::list<float> &updateTimes() const {
        return _updateTimes;
    }

    const std::list<float> &renderTimes() const {
        return _renderTimes;
    }

    uint32_t averageFps() const {
        return _averageFps;
    }

    uint32_t onePercentLowFps() const {
        return _onePercentLowFps;
    }

private:
    uint64_t _perfFreq {0};
    uint64_t _perfCntFrameStart {0};
    uint64_t _perfCntInputStart {0};
    uint64_t _perfCntInputEnd {0};
    uint64_t _perfCntUpdateStart {0};
    uint64_t _perfCntUpdateEnd {0};
    uint64_t _perfCntRenderStart {0};
    uint64_t _perfCntRenderEnd {0};
    uint64_t _perfCntFrameEnd {0};

    float _frameTime {0};
    float _inputTime {0};
    float _updateTime {0};
    float _renderTime {0};

    std::list<float> _frameTimes;
    std::list<float> _inputTimes;
    std::list<float> _updateTimes;
    std::list<float> _renderTimes;

    uint32_t _lastFpsTicks {0};

    uint32_t _averageFps {0};
    uint32_t _onePercentLowFps {0};

    void analyze() {
        analyzeFrameTimes();
        analyzeFpsPeriodically();
    }

    void analyzeFrameTimes() {
        _frameTime = getPeriodSeconds(_perfCntFrameStart, _perfCntFrameEnd);
        _inputTime = getPeriodSeconds(_perfCntInputStart, _perfCntInputEnd);
        _updateTime = getPeriodSeconds(_perfCntUpdateStart, _perfCntUpdateEnd);
        _renderTime = getPeriodSeconds(_perfCntRenderStart, _perfCntRenderEnd);

        if (static_cast<int>(_frameTimes.size()) == kMaxFrames) {
            _frameTimes.pop_front();
            _inputTimes.pop_front();
            _updateTimes.pop_front();
            _renderTimes.pop_front();
        }

        _frameTimes.push_back(_frameTime);
        _inputTimes.push_back(_inputTime);
        _updateTimes.push_back(_updateTime);
        _renderTimes.push_back(_renderTime);
    }

    void analyzeFpsPeriodically() {
        uint32_t ticks = SDL_GetTicks();
        if (ticks - _lastFpsTicks > kAnalyzeFpsIntervalMillis) {
            analyzeFps();
            _lastFpsTicks = ticks;
        }
    }

    void analyzeFps() {
        if (static_cast<int>(_frameTimes.size()) != kMaxFrames) {
            return;
        }

        float totalTime = 0.0f;
        for (auto &time : _frameTimes) {
            totalTime += time;
        }
        _averageFps = kMaxFrames / totalTime;

        float greatestTime = std::numeric_limits<float>::min();
        for (auto &time : _frameTimes) {
            if (time > greatestTime) {
                greatestTime = time;
            }
        }
        _onePercentLowFps = 1.0f / greatestTime;
    }

    inline float getPeriodSeconds(uint64_t start, uint64_t end) {
        return (end - start) / static_cast<float>(_perfFreq);
    }
};

} // namespace game

} // namespace reone
