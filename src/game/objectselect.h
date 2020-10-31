/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include <cstdint>
#include <vector>

namespace reone {

namespace game {

class Area;
class Party;

class ObjectSelector {
public:
    ObjectSelector(const Area *area, const Party *party);

    void update();
    void selectNext(bool reverse = false);
    void getSelectableObjects(std::vector<uint32_t> &ids) const;
    void selectNearest();
    void hilight(uint32_t objectId);
    void select(uint32_t objectId);

    int hilightedObjectId() const;
    int selectedObjectId() const;

private:
    const Area *_area { nullptr };
    const Party *_party { nullptr };
    int _hilightedObjectId { -1 };
    int _selectedObjectId { -1 };

    ObjectSelector(const ObjectSelector &) = delete;
    ObjectSelector &operator=(const ObjectSelector &) = delete;
};

} // namespace game

} // namespace reone
