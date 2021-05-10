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

#include "../common/log.h"

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
        float hearingRange2 = creature->perception().hearingRange * creature->perception().hearingRange;
        float sightRange2 = creature->perception().sightRange * creature->perception().sightRange;

        for (auto &other : creatures) {
            // Skip self
            if (other == object) continue;

            bool heard = false;
            bool seen = false;

            float distance2 = creature->getDistanceTo2(*other);
            if (distance2 <= hearingRange2) {
                heard = true;
            }
            if (distance2 <= sightRange2) {
                seen = _area->isInLineOfSight(*creature, *other);
            }

            // Hearing
            bool wasHeard = creature->perception().heard.count(other) > 0;
            if (!wasHeard && heard) {
                debug(boost::format("Perception: %s heard by %s") % other->tag() % creature->tag(), 2);
                creature->onObjectHeard(other);
            } else if (wasHeard && !heard) {
                debug(boost::format("Perception: %s inaudible to %s") % other->tag() % creature->tag(), 2);
                creature->onObjectInaudible(other);
            }

            // Sight
            bool wasSeen = creature->perception().seen.count(other) > 0;
            if (!wasSeen && seen) {
                debug(boost::format("Perception: %s seen by %s") % other->tag() % creature->tag(), 2);
                creature->onObjectSeen(other);
            } else if (wasSeen && !seen) {
                debug(boost::format("Perception: %s vanished from %s") % other->tag() % creature->tag(), 2);
                creature->onObjectVanished(other);
            }
        }
    }
}

} // namespace game

} // namespace reone
