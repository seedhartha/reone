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

#include "reone/game/neo/object/door.h"

#include "reone/resource/parser/2da/genericdoors.h"
#include "reone/resource/parser/gff/utd.h"
#include "reone/system/exception/validation.h"

using namespace reone::resource;
using namespace reone::resource::generated;

namespace reone {

namespace game {

namespace neo {

void Door::load(const UTD &utd,
                const GenericdoorsTwoDA &genericDoors) {
    const auto &genericDoorsRow = genericDoors.rows[utd.GenericType];
    _modelName = genericDoorsRow.modelname;
    setState(ObjectState::Loaded);
}

} // namespace neo

} // namespace game

} // namespace reone
