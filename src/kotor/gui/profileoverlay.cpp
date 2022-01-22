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
static constexpr float kRefreshInterval = 5.0f; // seconds
static constexpr float kTextOffset = 3.0f;

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
    if (!_enabled) {
        return;
    }

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
    if (_frametimes.empty()) {
        return;
    }

    float totalTime = 0.0f;
    for (auto &time : _frametimes) {
        totalTime += time;
    }

    _fps = glm::iround(_frametimes.size() / totalTime);
}

void ProfileOverlay::draw() {
    if (!_enabled) {
        return;
    }

    _services.graphicsContext.withBlending(BlendMode::Normal, [this]() {
        _font->draw(
            to_string(_fps),
            glm::vec3(static_cast<float>(_options.graphics.width) - kTextOffset, static_cast<float>(_options.graphics.height) - kTextOffset, 0.0f),
            glm::vec3(1.0f),
            TextGravity::LeftTop);
    });
}

} // namespace kotor

} // namespace reone
