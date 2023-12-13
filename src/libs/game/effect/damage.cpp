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

#include "reone/game/effect/damage.h"

#include "reone/system/logutil.h"

#include "reone/game/object.h"

namespace reone {

namespace game {

void DamageEffect::applyTo(Object &object) {
    debug(str(boost::format("Damage taken: %s %d") % object.tag() % _amount));
    object.setCurrentHitPoints(glm::max(object.isMinOneHP() ? 1 : 0, object.currentHitPoints() - _amount));
}

} // namespace game

} // namespace reone
