/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "../object.h"

#include "reone/resource/resref.h"

namespace reone {

namespace resource {

namespace generated {

struct UTP;

struct PlaceablesTwoDA;

} // namespace generated

} // namespace resource

namespace game {

namespace neo {

class Placeable : public SpatialObject {
public:
    Placeable(ObjectId id,
              ObjectTag tag,
              IEventCollector &eventCollector) :
        SpatialObject(
            id,
            std::move(tag),
            ObjectType::Placeable,
            eventCollector) {
    }

    void load(const resource::generated::UTP &utp,
              const resource::generated::PlaceablesTwoDA &placeables);

    const resource::ResRef &modelName() const {
        return _modelName;
    }

private:
    resource::ResRef _modelName;
};

} // namespace neo

} // namespace game

} // namespace reone
