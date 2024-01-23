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

#include "reone/resource/resref.h"

#include "../object.h"

namespace reone {

namespace resource {

class TwoDA;

namespace generated {

struct UTD;

}

} // namespace resource

namespace game {

namespace neo {

class Door : public SpatialObject {
public:
    Door(ObjectId id, ObjectTag tag) :
        SpatialObject(
            id,
            std::move(tag),
            ObjectType::Door) {
    }

    void load(const resource::generated::UTD &utd,
              const resource::TwoDA &genericDoors);

    const resource::ResRef &modelName() const {
        return _modelName;
    }

private:
    resource::ResRef _modelName;
};

} // namespace neo

} // namespace game

} // namespace reone
