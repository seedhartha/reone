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

#include <string>
#include <vector>

#include "../../resources/gfffile.h"

namespace reone {

namespace game {

class PlaceableBlueprint {
public:
    PlaceableBlueprint() = default;

    void load(const std::string &resRef, const resources::GffStruct &utp);

    const std::string &tag() const;
    int appearance() const;
    bool hasInventory() const;
    const std::vector<std::string> &items() const;

private:
    std::string _tag;
    int _appearance { 0 };
    bool _hasInventory { false };
    std::vector<std::string> _items;

    PlaceableBlueprint(const PlaceableBlueprint &) = delete;
    PlaceableBlueprint &operator=(const PlaceableBlueprint &) = delete;
};

} // namespace game

} // namespace reone
