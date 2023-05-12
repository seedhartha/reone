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

#include "reone/game/visibilities.h"

#include "reone/resource/resources.h"

#include "reone/game/format/visreader.h"

#include "reone/system/stream/bytearrayinput.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace game {

shared_ptr<Visibility> Visibilities::doGet(string resRef) {
    auto data = _resources.get(resRef, ResourceType::Vis);
    if (!data) {
        return nullptr;
    }
    auto stream = ByteArrayInputStream(*data);

    VisReader vis;
    vis.load(stream);

    return make_shared<Visibility>(vis.visibility());
}

} // namespace game

} // namespace reone
