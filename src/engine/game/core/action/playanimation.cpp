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

#include "playanimation.h"

#include "../../../scene/animproperties.h"

#include "../object/spatial.h"

using namespace std;

using namespace reone::scene;

namespace reone {

namespace game {

void PlayAnimationAction::execute(Object &actor, float dt) {
    auto spatial = dynamic_cast<SpatialObject *>(&actor);
    if (!spatial) {
        complete();
        return;
    }

    string animName(spatial->getAnimationName(_anim));
    if (_playing) {
        if (spatial->getActiveAnimationName() != animName) {
            complete();
        }
        return;
    }

    AnimationProperties properties;
    properties.speed = _speed;
    properties.duration = _durationSeconds;

    spatial->playAnimation(_anim, move(properties));

    _playing = true;
}

} // namespace game

} // namespace reone
