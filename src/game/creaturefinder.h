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

#pragma once

#include <vector>

#include "enginetype/location.h"
#include "object/creature.h"
#include "types.h"

namespace reone {

namespace game {

class Area;

class CreatureFinder {
public:
    typedef std::vector<std::pair<CreatureType, int>> CriteriaList;

    CreatureFinder(Area *area);

    /**
     * @param nth 0-based index of the creature
     * @return nth nearest creature to the target object, that matches the specified criterias
     */
    std::shared_ptr<Creature> getNearestCreature(const SpatialObject &target, const CriteriaList &criterias, int nth = 0);

    /**
     * @param nth 0-based index of the creature
     * @return nth nearest creature to the location, that matches the specified criterias
     */
    std::shared_ptr<Creature> getNearestCreatureToLocation(const Location &location, const CriteriaList &criterias, int nth = 0);

private:
    Area *_area;

    bool matchesCriterias(const Creature &creature, const CriteriaList &criterias, std::shared_ptr<SpatialObject> target = nullptr) const;
};

} // namespace game

} // namespace reone
