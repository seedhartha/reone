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

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

#include "../../resource/gfffile.h"

namespace reone {

namespace game {

class ObjectFactory;
class Placeable;

class PlaceableBlueprint {
public:
    PlaceableBlueprint(const std::string &resRef, const std::shared_ptr<resource::GffStruct> &utp);

    void load(Placeable &placeable);

    const std::string &resRef() const;

private:
    std::string _resRef;
    std::shared_ptr<resource::GffStruct> _utp;

    PlaceableBlueprint(const PlaceableBlueprint &) = delete;
    PlaceableBlueprint &operator=(const PlaceableBlueprint &) = delete;

    void loadItems(Placeable &placeable);
    void loadScripts(Placeable &placeable);
};

} // namespace game

} // namespace reone
