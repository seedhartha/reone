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

#include "profileoverlay.h"

#include "../../game/services.h"
#include "../../graphics/context.h"
#include "../../graphics/fonts.h"
#include "../../graphics/mesh.h"
#include "../../graphics/meshes.h"
#include "../../graphics/shaders.h"
#include "../../graphics/textutil.h"
#include "../../graphics/window.h"

using namespace std;

using namespace reone::graphics;

namespace reone {

namespace kotor {

static constexpr int kFrameWidth = 125;
static constexpr char kFontResRef[] = "fnt_console";
static constexpr float kRefreshInterval = 1.0f; // seconds

void ProfileOverlay::init() {
    _refreshTimer.setTimeout(kRefreshInterval);
    _frequency = SDL_GetPerformanceFrequency();
    _counter = SDL_GetPerformanceCounter();
    _font = _services.fonts.get(kFontResRef);
}

bool ProfileOverlay::handle(const SDL_Event &event) {
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_F5) {
        _enabled = !_enabled;
        return true;
    }
    return false;
}

void ProfileOverlay::update(float dt) {
    if (!_enabled)
        return;

    uint64_t counter = SDL_GetPerformanceCounter();
    float frametime = static_cast<float>((counter - _counter) / static_cast<double>(_frequency));
    _frametimes.push_back(frametime);

    if (_refreshTimer.advance(dt)) {
        calculateFPS();
        _frametimes.clear();
        _refreshTimer.setTimeout(kRefreshInterval);
    }

    _counter = counter;
}

void ProfileOverlay::calculateFPS() {
    if (_frametimes.empty())
        return;

    vector<int> fps;
    for (size_t i = 0; i < _frametimes.size(); ++i) {
        fps.push_back(glm::iround(1.0f / _frametimes[i]));
    }
    sort(fps.begin(), fps.end(), less<int>());

    // Average FPS
    _fps.average = 0;
    for (size_t i = 0; i < fps.size(); ++i) {
        _fps.average += fps[i];
    }
    _fps.average /= static_cast<int>(fps.size());

    // 1% Low FPS
    int numOnePer = glm::max(1, static_cast<int>(fps.size()) / 100);
    _fps.onePerLow = 0;
    for (int i = 0; i < numOnePer; ++i) {
        _fps.onePerLow += fps[i];
    }
    _fps.onePerLow /= numOnePer;
}

void ProfileOverlay::draw() {
    if (!_enabled)
        return;

    drawBackground();
    drawText();
}

void ProfileOverlay::drawBackground() {
    glm::mat4 transform(1.0f);
    transform = glm::scale(transform, glm::vec3(kFrameWidth, 2.0f * _font->height(), 1.0f));

    auto &uniforms = _services.shaders.uniforms();
    uniforms.general.resetLocals();
    uniforms.general.projection = _services.window.getOrthoProjection();
    uniforms.general.model = move(transform);
    uniforms.general.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    uniforms.general.alpha = 0.5f;

    _services.graphicsContext.useShaderProgram(_services.shaders.simpleColor());
    _services.shaders.refreshUniforms();
    _services.meshes.quad().draw();
}

void ProfileOverlay::drawText() {
    stringstream ss;
    ss << "FPS: " << _fps.average << endl;
    ss << "1% Low: " << _fps.onePerLow << endl;

    vector<string> lines(breakText(ss.str(), *_font, kFrameWidth));
    glm::vec3 position(0.0f);

    for (auto &line : lines) {
        _font->draw(line, position, glm::vec3(1.0f), TextGravity::RightBottom);
        position.y += _font->height();
    }
}

} // namespace kotor

} // namespace reone
