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
#include "reone/graphics/textutil.h"
#include "reone/graphics/uniforms.h"
#include "reone/resource/di/services.h"
#include "reone/resource/provider/fonts.h"
#include "reone/system/checkutil.h"
#include "reone/system/clock.h"
#include "reone/system/di/services.h"
#include "reone/system/stringbuilder.h"

using namespace reone::graphics;

namespace reone {

static constexpr char kFontResRef[] = "fnt_console";
static constexpr float kTextOffset = 3.0f;
static constexpr int kNumTimedFrames = 100;
static constexpr float kFrameTimesScale = 2.0f;

static constexpr float kFPSRefreshDelay = 1.0f;  // seconds
static constexpr float kFPSRefreshPeriod = 5.0f; // seconds

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
    if (event.type != input::EventType::KeyDown || event.key.code != input::KeyCode::F5) {
        return false;
    }
    bool enabled = !_enabled;
    _enabled = enabled;
    if (!enabled) {
        return true;
    }
    std::lock_guard<std::mutex> lock {_mutex};
    _inputTimes.clear();
    _updateTimes.clear();
    _renderTimes.clear();
    _percentilesTimer.reset(kFPSRefreshDelay);
    return true;
}

void Profiler::update(float dt) {
    if (!_enabled) {
        return;
    }
    std::lock_guard<std::mutex> lock {_mutex};
    _percentilesTimer.update(dt);
    if (!_percentilesTimer.elapsed()) {
        return;
    }
    if (_renderTimes.size() == kNumTimedFrames) {
        auto renderTimes = _renderTimes;
        std::sort(renderTimes.begin(), renderTimes.end());
        _p99FrameTime = renderTimes[98];
        _p95FrameTime = renderTimes[94];
    }
    _percentilesTimer.reset(kFPSRefreshPeriod);
}

void Profiler::render() {
    if (!_enabled) {
        return;
    }
    std::lock_guard<std::mutex> lock {_mutex};
    _graphicsSvc.uniforms.setGlobals([this](auto &globals) {
        globals.reset();
        globals.projection = glm::ortho(
            0.0f, static_cast<float>(_graphicsOpt.width),
            static_cast<float>(_graphicsOpt.height), 0.0f,
            0.0f, 100.0f);
    });
    _graphicsSvc.context.withBlendMode(BlendMode::Normal, [this]() {
        renderBackground();
        renderFrameTimes();
        renderText();
    });
}

void Profiler::renderBackground() {
    float height = kNumTimedFrames * kFrameTimesScale + 2 * kTextOffset;
    glm::mat4 transform {1.0f};
    transform = glm::scale(transform, glm::vec3(_graphicsOpt.width, height, 1.0f));
    _graphicsSvc.uniforms.setLocals([this, transform](auto &locals) {
        locals.reset();
        locals.model = std::move(transform);
        locals.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        locals.color.a = 0.5f;
    });
    auto &shader = _graphicsSvc.shaderRegistry.get(ShaderProgramId::mvpColor);
    _graphicsSvc.context.useProgram(shader);
    _graphicsSvc.meshRegistry.get(MeshName::quad).draw();
}

void Profiler::renderFrameTimes() {
    if (_inputTimes.size() < kNumTimedFrames ||
        _updateTimes.size() < kNumTimedFrames ||
        _renderTimes.size() < kNumTimedFrames) {
        return;
    }
    std::vector<glm::vec4> inputTimes;
    std::vector<glm::vec4> updateTimes;
    std::vector<glm::vec4> renderTimes;
    inputTimes.resize(kNumTimedFrames / 4, glm::vec4 {0.0f});
    updateTimes.resize(kNumTimedFrames / 4, glm::vec4 {0.0f});
    renderTimes.resize(kNumTimedFrames / 4, glm::vec4 {0.0f});
    for (int i = 0; i < kNumTimedFrames / 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            inputTimes[i][j] = _inputTimes[4 * i + j];
            updateTimes[i][j] = _updateTimes[4 * i + j];
            renderTimes[i][j] = _renderTimes[4 * i + j];
        }
    }
    float size = kNumTimedFrames * kFrameTimesScale;
    glm::mat4 transform {1.0f};
    transform = glm::translate(
        transform,
        glm::vec3 {kTextOffset, kTextOffset, 0.0f});
    transform = glm::scale(transform, glm::vec3 {size, size, 1.0f});
    _graphicsSvc.uniforms.setLocals([this, transform](auto &locals) {
        locals.reset();
        locals.model = std::move(transform);
    });
    auto &program = _graphicsSvc.shaderRegistry.get(ShaderProgramId::profiler);
    _graphicsSvc.context.useProgram(program);
    program.setUniform("uInputTimes", inputTimes);
    program.setUniform("uUpdateTimes", updateTimes);
    program.setUniform("uRenderTimes", renderTimes);
    _graphicsSvc.meshRegistry.get(MeshName::quad).draw();
}

void Profiler::renderText() {
    StringBuilder text;
    if (_p99FrameTime > 0.0f) {
        text.append(str(boost::format("p99: %.03f %d") % _p99FrameTime % static_cast<int>(1.0f / _p99FrameTime)));
    } else {
        text.append("p99: 0 inf");
    }
    text.append("\n");
    if (_p95FrameTime > 0.0f) {
        text.append(str(boost::format("p95: %.03f %d") % _p95FrameTime % static_cast<int>(1.0f / _p95FrameTime)));
    } else {
        text.append("p95: 0 inf");
    }
    std::vector<std::string> lines;
    boost::split(lines, text.string(), boost::is_any_of("\n"));
    float y = kTextOffset;
    for (const auto &line : lines) {
        _font->render(
            line,
            glm::vec3 {2 * kTextOffset + kNumTimedFrames * kFrameTimesScale, y, 0.0f},
            glm::vec3 {1.0f},
            TextGravity::RightBottom);
        y += _font->height() + kTextOffset;
    }
}

void Profiler::timeInput(std::function<void()> block) {
    uint64_t before = _systemSvc.clock.micros();
    block();
    uint64_t after = _systemSvc.clock.micros();
    std::lock_guard<std::mutex> lock {_mutex};
    if (_inputTimes.size() == kNumTimedFrames) {
        _inputTimes.pop_front();
    }
    _inputTimes.push_back((after - before) / 1e6f);
}

void Profiler::timeUpdate(std::function<void()> block) {
    uint64_t before = _systemSvc.clock.micros();
    block();
    uint64_t after = _systemSvc.clock.micros();
    std::lock_guard<std::mutex> lock {_mutex};
    if (_updateTimes.size() == kNumTimedFrames) {
        _updateTimes.pop_front();
    }
    _updateTimes.push_back((after - before) / 1e6f);
}

void Profiler::timeRender(std::function<void()> block) {
    uint64_t before = _systemSvc.clock.micros();
    block();
    uint64_t after = _systemSvc.clock.micros();
    std::lock_guard<std::mutex> lock {_mutex};
    if (_renderTimes.size() == kNumTimedFrames) {
        _renderTimes.pop_front();
    }
    _renderTimes.push_back((after - before) / 1e6f);
}

} // namespace reone
