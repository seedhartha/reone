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
#include "reone/resource/provider/fonts.h"
#include "reone/system/clock.h"
#include "reone/system/di/services.h"

using namespace reone::graphics;

namespace reone {

namespace game {

static constexpr char kFontResRef[] = "fnt_console";

static constexpr float kRefreshDelay = 1.0f;  // seconds
static constexpr float kRefreshPeriod = 5.0f; // seconds

static constexpr int kFrameWidth = 125;
static constexpr float kTextOffset = 3.0f;

void ProfileOverlay::init() {
    _font = _services.resource.fonts.get(kFontResRef);
}

bool ProfileOverlay::handle(const SDL_Event &event) {
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_F5) {
        _enabled = !_enabled;
        if (_enabled) {
            _ticks = _services.system.clock.ticks();
            _refreshTimer.reset(kRefreshDelay);
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

    _refreshTimer.update(dt);
    if (_refreshTimer.elapsed()) {
        uint64_t ticks = _services.system.clock.ticks();
        _fps = static_cast<int>(_numFrames * 1000 / (ticks - _ticks));
        _numFrames = 0;
        _ticks = ticks;
        _refreshTimer.reset(kRefreshPeriod);
    }
}

void ProfileOverlay::render() {
    if (!_enabled) {
        return;
    }

    _services.graphics.context.withBlending(BlendMode::Normal, [this]() {
        _font->render(
            std::to_string(_fps),
            glm::vec3(static_cast<float>(_options.graphics.width) - kTextOffset, static_cast<float>(_options.graphics.height) - kTextOffset, 0.0f),
            glm::vec3(1.0f),
            TextGravity::LeftTop);
    });
}

} // namespace game

} // namespace reone
