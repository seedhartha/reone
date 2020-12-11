/*
 * Copyright (c) 2020 The reone project contributors
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

#include "../../resource/gfffile.h"

#include "../blueprint/placeable.h"

#include "spatial.h"

namespace reone {

namespace game {

class ObjectFactory;

class Placeable : public SpatialObject {
public:
    Placeable(uint32_t id, ObjectFactory *objectFactory, scene::SceneGraph *sceneGraph);

    bool isSelectable() const override;

    void load(const resource::GffStruct &gffs);

    bool hasInventory() const;
    bool isUsable() const;

    const std::string &onInvDisturbed() const;

private:
    int  _appearance { 0 };
    bool _hasInventory { false };
    bool _usable { false };

    // Scripts

    std::string _onInvDisturbed;

    // END Scripts

    void loadBlueprint(const resource::GffStruct &gffs);

    friend class PlaceableBlueprint;
};

} // namespace game

} // namespace reone
