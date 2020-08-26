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

#include <vector>

#include "../item.h"

#include "spatial.h"

namespace reone {

namespace game {

class Placeable : public SpatialObject {
public:
    Placeable(uint32_t id);

    void load(const resources::GffStruct &gffs);

    const std::vector<std::shared_ptr<Item>> &items() const;

private:
    std::vector<std::shared_ptr<Item>> _items;

    void loadBlueprint(const resources::GffStruct &gffs);
};

} // namespace game

} // namespace reone
