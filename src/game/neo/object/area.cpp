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

#include "area.h"

#include "../../resource/gffs.h"
#include "../../resource/gffstruct.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace game {

namespace neo {

unique_ptr<Area> Area::Loader::load(const std::string &name) {
    info("Loading area " + name);

    auto are = _gffs.get(name, ResourceType::Are);
    auto git = _gffs.get(name, ResourceType::Git);

    return Area::Builder()
        .id(_idSeq.nextObjectId())
        .tag(name)
        .build();
}

} // namespace neo

} // namespace game

} // namespace reone