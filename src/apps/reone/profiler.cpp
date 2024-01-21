/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/graphics/context.h"
#include "reone/graphics/di/services.h"
#include "reone/graphics/mesh.h"
#include "reone/graphics/meshregistry.h"
#include "reone/graphics/shaderregistry.h"
#include "reone/graphics/statistic.h"
#include "reone/graphics/textutil.h"
#include "reone/graphics/uniforms.h"
#include "reone/resource/di/services.h"
#include "reone/resource/provider/fonts.h"
#include "reone/system/checkutil.h"
#include "reone/system/clock.h"
#include "reone/system/di/services.h"
#include "reone/system/stringbuilder.h"

using namespace reone::game;
using namespace reone::graphics;

namespace reone {

static constexpr char kFontResRef[] = "fnt_console";
static constexpr float kTextOffset = 3.0f;
static constexpr int kNumTimedFrames = 100;
static constexpr float kFrameTimesScale = 2.0f;

void Profiler::init() {
    checkThat(!_inited, "Must not be initialized");
    _font = _resourceSvc.fonts.get(kFontResRef);
    _inited = true;
}

void Profiler::deinit() {
    if (!_inited) {
        return;
    }
    _font.reset();
    _inited = false;
}

bool Profiler::handle(const input::Event &event) {
    if (event.type != input::EventType::KeyDown) {
        return false;
    }
    bool enabled = _enabled.load(std::memory_order::memory_order_acquire);
    if (event.key.code == input::KeyCode::F5) {
        _enabled.store(!enabled, std::memory_order::memory_order_release);
        return true;
    }
    if (!enabled) {
        return false;
    }
    switch (event.key.code) {
    case input::KeyCode::Key1:
        _fpsTarget = 30.0f;
        return true;
    case input::KeyCode::Key2:
        _fpsTarget = 60.0f;
        return true;
    case input::KeyCode::Key3:
        _fpsTarget = 120.0f;
        return true;
    case input::KeyCode::Key4:
        _fpsTarget = 240.0f;
        return true;
    default:
        return false;
    }
}

void Profiler::update(float dt) {
    if (!_enabled.load(std::memory_order::memory_order_acquire)) {
        return;
    }
}

void Profiler::render() {
    if (!_enabled.load(std::memory_order::memory_order_acquire)) {
        return;
    }
    _graphicsSvc.uniforms.setGlobals([this](auto &globals) {
        globals.reset();
        globals.projection = glm::ortho(
            0.0f, static_cast<float>(_graphicsOpt.width),
            static_cast<float>(_graphicsOpt.height), 0.0f,
            0.0f, 100.0f);
    });
    _graphicsSvc.context.withBlendMode(BlendMode::Normal, [this]() {
        renderBackground();
        int xOffset = 0;
        for (int i = 0; i < _numTimedThreads; ++i) {
            auto &thread = _timedThreads[i];
            std::lock_guard<std::mutex> lock {thread.mutex};
            renderFrameTimes(thread, xOffset);
            xOffset += kNumTimedFrames * kFrameTimesScale + kTextOffset;
        }
        renderStatistic(xOffset);
    });
}

void Profiler::renderBackground() {
    auto &shader = _graphicsSvc.shaderRegistry.get(ShaderProgramId::mvpColor);
    _graphicsSvc.context.useProgram(shader);
    float height = kNumTimedFrames * kFrameTimesScale + 2 * kTextOffset;
    auto transform = glm::scale(glm::vec3(_graphicsOpt.width, height, 1.0f));
    _graphicsSvc.uniforms.setLocals([this, transform](auto &locals) {
        locals.reset();
        locals.model = std::move(transform);
        locals.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        locals.color.a = 0.5f;
    });
    _graphicsSvc.meshRegistry.get(MeshName::quad).draw();
}

void Profiler::renderFrameTimes(const TimedThread &thread, int xOffset) {
    auto &program = _graphicsSvc.shaderRegistry.get(ShaderProgramId::profiler);
    _graphicsSvc.context.useProgram(program);
    for (size_t i = 0; i < 4; ++i) {
        glm::vec3 color;
        if (thread.colors.size() > i) {
            color = thread.colors[i];
        } else {
            color = glm::vec3(1.0f);
        }
        program.setUniform("uSeriesColor" + std::to_string(i + 1), color);
    }

    std::vector<glm::vec4> vecTimes;
    vecTimes.resize(kNumTimedFrames / 4, glm::vec4 {0.0f});
    float oneOverFpsTarget = 1.0f / _fpsTarget;
    for (int slot = 0; slot < 4; ++slot) {
        if (thread.times[slot].size() >= kNumTimedFrames) {
            for (int i = 0; i < kNumTimedFrames / 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    vecTimes[i][j] = thread.times[slot][4 * i + j] / oneOverFpsTarget;
                    vecTimes[i][j] = thread.times[slot][4 * i + j] / oneOverFpsTarget;
                    vecTimes[i][j] = thread.times[slot][4 * i + j] / oneOverFpsTarget;
                    vecTimes[i][j] = thread.times[slot][4 * i + j] / oneOverFpsTarget;
                }
            }
        } else {
            std::memset(&vecTimes[0], 0, vecTimes.size() * sizeof(glm::vec4));
        }
        program.setUniform("uSeriesValues" + std::to_string(slot + 1), vecTimes);
    }

    float size = kNumTimedFrames * kFrameTimesScale;
    auto transform = glm::scale(
        glm::translate(glm::vec3 {kTextOffset + xOffset, kTextOffset, 0.0f}),
        glm::vec3 {size, size, 1.0f});
    _graphicsSvc.uniforms.setLocals([this, transform](auto &locals) {
        locals.reset();
        locals.model = std::move(transform);
    });
    _graphicsSvc.meshRegistry.get(MeshName::quad).draw();

    auto targetTime = str(boost::format("%.04fs (%dfps)") % oneOverFpsTarget % static_cast<int>(_fpsTarget));
    _font->render(
        targetTime,
        glm::vec3 {kTextOffset + xOffset, kTextOffset, 0.0f},
        glm::vec3 {1.0f},
        TextGravity::RightBottom);

    auto halfTargetTime = str(boost::format("%.04fs (%dfps)") % (0.5f * oneOverFpsTarget) % (2 * static_cast<int>(_fpsTarget)));
    _font->render(
        halfTargetTime,
        glm::vec3 {kTextOffset + xOffset, kTextOffset + 0.5f * size, 0.0f},
        glm::vec3 {0.5f},
        TextGravity::RightBottom);
}

void Profiler::renderStatistic(int xOffset) {
    auto text = str(boost::format("%d draw calls") % _graphicsSvc.statistic.numDrawCalls());
    _font->render(
        text,
        glm::vec3 {kTextOffset + xOffset, kTextOffset, 0.0f},
        glm::vec3 {1.0f},
        TextGravity::RightBottom);
}

void Profiler::reserveThread(std::string name, std::vector<glm::vec3> colors) {
    if (_nameToTimedThread.count(name) > 0) {
        return;
    }
    checkLessOrEqual("timed thread count", _numTimedThreads, kMaxTimedThreads);
    _timedThreads[_numTimedThreads].name = std::move(name);
    _timedThreads[_numTimedThreads].colors = std::move(colors);
    auto &reserved = _timedThreads[_numTimedThreads];
    _nameToTimedThread.insert({reserved.name, reserved});
    ++_numTimedThreads;
}

void Profiler::measure(const std::string &threadName,
                       int timeIndex,
                       const std::function<void()> &block) {
    uint64_t before = _systemSvc.clock.micros();
    block();
    uint64_t after = _systemSvc.clock.micros();
    if (!_enabled.load(std::memory_order::memory_order_acquire)) {
        return;
    }
    checkThat(0 <= timeIndex && timeIndex < 4, "timeIndex must be between 0 and 3");
    checkThat(_nameToTimedThread.count(threadName) > 0, "Timed thread must be reserved");
    auto &thread = _nameToTimedThread.at(threadName).get();
    std::lock_guard<std::mutex> lock {thread.mutex};
    auto &times = thread.times[timeIndex];
    if (times.size() == kNumTimedFrames) {
        times.pop_front();
    }
    times.push_back((after - before) / 1e6f);
}

} // namespace reone
