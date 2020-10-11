/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "glm/vec3.hpp"

#include "object/creature.h"

namespace reone {

namespace game {

class Area;

class ActionExecutor {
public:
    ActionExecutor(Area *area);

    void executeActions(Creature &creature, float dt);

private:
    Area *_area { nullptr };

    ActionExecutor(const ActionExecutor &) = delete;
    ActionExecutor &operator=(const ActionExecutor &) = delete;

    bool navigateCreature(Creature &creature, const glm::vec3 &dest, float distance, float dt);
    void advanceCreatureOnPath(Creature &creature, float dt);
    void selectNextPathPoint(Creature::Path &path);
    void updateCreaturePath(Creature &creature, const glm::vec3 &dest);
};

} // namespace game

} // namespace reone
