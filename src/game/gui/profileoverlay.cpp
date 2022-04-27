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

#include "../../graphics/context.h"
#include "../../graphics/fonts.h"
#include "../../graphics/mesh.h"
#include "../../graphics/meshes.h"
#include "../../graphics/services.h"
#include "../../graphics/shaders.h"
#include "../../graphics/textutil.h"
#include "../../graphics/window.h"

#include "../services.h"

using namespace std;

using namespace reone::graphics;

namespace reone {

namespace game {

static constexpr char kFontResRef[] = "fnt_console";

static constexpr float kRefreshDelay = 1.0f;  // seconds
static constexpr float kRefreshPeriod = 5.0f; // seconds

static constexpr int kFrameWidth = 125;
static constexpr float kTextOffset = 3.0f;

void ProfileOverlay::init() {
    _frequency = SDL_GetPerformanceFrequency();
    _font = _services.graphics.fonts.get(kFontResRef);
}

bool ProfileOverlay::handle(const SDL_Event &event) {
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_F5) {
        _enabled = !_enabled;
        if (_enabled) {
            _counter = SDL_GetPerformanceCounter();
            _refreshTimer.setTimeout(kRefreshDelay);
        }
        return true;
    }

    return false;
}

void ProfileOverlay::update(float dt) {
    if (!_enabled) {
        return;
    }

    ++_numFrames;

    if (_refreshTimer.advance(dt)) {
        uint64_t counter = SDL_GetPerformanceCounter();
        _fps = static_cast<int>(_numFrames * _frequency / (counter - _counter));
        _numFrames = 0;
        _counter = counter;
        _refreshTimer.setTimeout(kRefreshPeriod);
    }
}

void ProfileOverlay::draw() {
    if (!_enabled) {
        return;
    }

    _services.graphics.context.withBlending(BlendMode::Normal, [this]() {
        _font->draw(
            to_string(_fps),
            glm::vec3(static_cast<float>(_options.graphics.width) - kTextOffset, static_cast<float>(_options.graphics.height) - kTextOffset, 0.0f),
            glm::vec3(1.0f),
            TextGravity::LeftTop);
    });
}

} // namespace game

} // namespace reone
