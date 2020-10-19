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

#include "../../system/resource/blueprint/placeable.h"
#include "../../system/resource/gfffile.h"

#include "spatial.h"

namespace reone {

namespace game {

class ObjectFactory;

class Placeable : public SpatialObject {
public:
    Placeable(uint32_t id, ObjectFactory *objectFactory, scene::SceneGraph *sceneGraph);

    void load(const resource::GffStruct &gffs);

    const resource::PlaceableBlueprint &blueprint() const;

private:
    ObjectFactory *_objectFactory { nullptr };
    std::shared_ptr<resource::PlaceableBlueprint> _blueprint;

    void loadBlueprint(const std::string &resRef);
};

} // namespace game

} // namespace reone
