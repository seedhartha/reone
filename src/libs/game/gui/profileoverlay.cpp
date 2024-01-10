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

#include "reone/game/gui/profileoverlay.h"

#include "reone/game/di/services.h"
#include "reone/graphics/context.h"
#include "reone/graphics/di/services.h"
#include "reone/graphics/mesh.h"
#include "reone/graphics/meshregistry.h"
#include "reone/graphics/shaderregistry.h"
#include "reone/graphics/textutil.h"
#include "reone/graphics/uniforms.h"
#include "reone/resource/provider/fonts.h"
#include "reone/system/checkutil.h"
#include "reone/system/clock.h"
#include "reone/system/di/services.h"
#include "reone/system/stringbuilder.h"

using namespace reone::graphics;

namespace reone {

namespace game {

static constexpr char kFontResRef[] = "fnt_console";
static constexpr float kTextOffset = 3.0f;
static constexpr int kNumTimedFrames = 100;
static constexpr float kFrameTimesScale = 2.0f;

static constexpr float kFPSRefreshDelay = 1.0f;  // seconds
static constexpr float kFPSRefreshPeriod = 5.0f; // seconds

struct FrameTimesUniforms {
    glm::vec4 inputTimes[kNumTimedFrames / 4] {glm::vec4 {0.0f}};
    glm::vec4 updateTimes[kNumTimedFrames / 4] {glm::vec4 {0.0f}};
    glm::vec4 renderTimes[kNumTimedFrames / 4] {glm::vec4 {0.0f}};
};

void ProfileOverlay::init() {
    checkThat(!_inited, "Must not be initialized");
    _font = _services.resource.fonts.get(kFontResRef);

    FrameTimesUniforms defFrameTimes;
    _frameTimesUBO = std::make_shared<UniformBuffer>();
    _frameTimesUBO->setData(&defFrameTimes, sizeof(FrameTimesUniforms));
    _frameTimesUBO->init();

    _inited = true;
}

void ProfileOverlay::deinit() {
    if (!_inited) {
        return;
    }
    _frameTimesUBO.reset();
    _inited = false;
}

bool ProfileOverlay::handle(const input::Event &event) {
    if (event.type == input::EventType::KeyDown && event.key.code == input::KeyCode::F5) {
        _enabled = !_enabled;
        if (_enabled) {
            _frameTimes.clear();
            _percentilesTimer.reset(kFPSRefreshDelay);
        }
        return true;
    }
    return false;
}

void ProfileOverlay::update(float dt) {
    if (!_enabled) {
        return;
    }
    _percentilesTimer.update(dt);
    if (!_percentilesTimer.elapsed()) {
        return;
    }
    if (_frameTimes.size() == kNumTimedFrames) {
        std::vector<float> totalTimes;
        totalTimes.reserve(kNumTimedFrames);
        for (const auto &times : _frameTimes) {
            totalTimes.push_back(times.total);
        }
        std::sort(totalTimes.begin(), totalTimes.end());
        _p99FrameTime = totalTimes[98];
        _p95FrameTime = totalTimes[94];
    }
    _percentilesTimer.reset(kFPSRefreshPeriod);
}

void ProfileOverlay::render() {
    if (!_enabled) {
        return;
    }
    _services.graphics.context.withBlending(BlendMode::Normal, [this]() {
        renderBackground();
        renderFrameTimes();
        renderText();
    });
}

void ProfileOverlay::renderBackground() {
    float height = kNumTimedFrames * kFrameTimesScale + 2 * kTextOffset;
    glm::mat4 transform {1.0f};
    transform = glm::scale(transform, glm::vec3(_options.graphics.width, height, 1.0f));
    _services.graphics.uniforms.setLocals([this, transform](auto &locals) {
        locals.reset();
        locals.model = std::move(transform);
        locals.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        locals.color.a = 0.5f;
    });
    auto &shader = _services.graphics.shaderRegistry.get(ShaderProgramId::mvpColor);
    _services.graphics.context.useProgram(shader);
    _services.graphics.meshRegistry.get(MeshName::quad).draw();
}

void ProfileOverlay::renderFrameTimes() {
    if (_frameTimes.size() < kNumTimedFrames) {
        return;
    }
    float size = kNumTimedFrames * kFrameTimesScale;
    glm::mat4 transform {1.0f};
    transform = glm::translate(
        transform,
        glm::vec3 {kTextOffset, kTextOffset, 0.0f});
    transform = glm::scale(transform, glm::vec3 {size, size, 1.0f});
    _services.graphics.uniforms.setLocals([this, transform](auto &locals) {
        locals.reset();
        locals.model = std::move(transform);
    });

    FrameTimesUniforms uniforms;
    for (int i = 0; i < kNumTimedFrames / 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            uniforms.inputTimes[i][j] = _frameTimes[4 * i + j].input;
            uniforms.updateTimes[i][j] = _frameTimes[4 * i + j].update;
            uniforms.renderTimes[i][j] = _frameTimes[4 * i + j].render;
        }
    }
    _frameTimesUBO->bind(UniformBlockBindingPoints::frameTimes);
    _frameTimesUBO->setData(&uniforms, sizeof(FrameTimesUniforms));

    auto &shader = _services.graphics.shaderRegistry.get(ShaderProgramId::profiler);
    _services.graphics.context.useProgram(shader);
    _services.graphics.meshRegistry.get(MeshName::quad).draw();
}

void ProfileOverlay::renderText() {
    StringBuilder text;
    if (_p99FrameTime > 0.0f && _p95FrameTime > 0.0f) {
        text.append(str(boost::format("p99: %.03f %d") % _p99FrameTime % static_cast<int>(1.0f / _p99FrameTime)));
        text.append("\n");
        text.append(str(boost::format("p95: %.03f %d") % _p95FrameTime % static_cast<int>(1.0f / _p95FrameTime)));
    }
    std::vector<std::string> lines;
    boost::split(lines, text.string(), boost::is_any_of("\n"));
    float y = kTextOffset;
    for (const auto &line : lines) {
        _font->render(
            line,
            glm::vec3(2 * kTextOffset + kNumTimedFrames * kFrameTimesScale, y, 0.0f),
            glm::vec3(1.0f),
            TextGravity::RightBottom);
        y += _font->height() + kTextOffset;
    }
}

bool ProfileOverlay::timeInput(std::function<bool()> block) {
    uint64_t before = _services.system.clock.ticks();
    auto result = block();
    uint64_t after = _services.system.clock.ticks();
    _inputTime = (after - before) / 1000.0f;
    return result;
}

void ProfileOverlay::timeUpdate(std::function<void()> block) {
    uint64_t before = _services.system.clock.ticks();
    block();
    uint64_t after = _services.system.clock.ticks();
    _updateTime = (after - before) / 1000.0f;
}

void ProfileOverlay::timeRender(std::function<void()> block) {
    uint64_t before = _services.system.clock.ticks();
    block();
    uint64_t after = _services.system.clock.ticks();
    _renderTime = (after - before) / 1000.0f;
}

void ProfileOverlay::onFrameEnded() {
    if (!_enabled) {
        return;
    }
    if (_frameTimes.size() == kNumTimedFrames) {
        _frameTimes.pop_front();
    }
    _frameTimes.emplace_back(_inputTime, _updateTime, _renderTime);
}

} // namespace game

} // namespace reone
