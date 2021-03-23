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

#include "perception.h"

#include <set>
#include <stdexcept>

#include "object/creature.h"
#include "object/area.h"

using namespace std;

namespace reone {

namespace game {

static constexpr float kUpdateInterval = 1.0f; // seconds

Perception::Perception(Area *area) : _area(area) {
    if (!area) {
        throw invalid_argument("area must not be null");
    }
}

void Perception::update(float dt) {
    if (_updateTimer.advance(dt)) {
        doUpdate();
        _updateTimer.reset(kUpdateInterval);
    }
}

void Perception::doUpdate() {
    // For each creature, determine a list of creatures it sees
    ObjectList &creatures = _area->getObjectsByType(ObjectType::Creature);
    for (auto &object : creatures) {
        // Skip dead creatures
        if (object->isDead()) continue;

        auto creature = static_pointer_cast<Creature>(object);
        float sightRange2 = creature->perception().sightRange * creature->perception().sightRange;
        float hearingRange2 = creature->perception().hearingRange * creature->perception().hearingRange;

        for (auto &other : creatures) {
            bool seen = false;
            bool heard = false;

            float distance2 = creature->getDistanceTo2(*object);
            if (distance2 <= sightRange2) {
                // TODO: check line-of-sight
                seen = true;
            }
            if (distance2 <= hearingRange2) {
                heard = true;
            }

            bool wasSeen = creature->perception().seen.count(other) > 0;
            if (!wasSeen && seen) {
                creature->onObjectSeen(other);
            } else if (wasSeen && !seen) {
                creature->onObjectVanished(other);
            }

            bool wasHeard = creature->perception().heard.count(other) > 0;
            if (!wasHeard && heard) {
                creature->onObjectHeard(other);
            } else if (wasHeard && !heard) {
                creature->onObjectInaudible(other);
            }
        }
    }
}

} // namespace game

} // namespace reone
