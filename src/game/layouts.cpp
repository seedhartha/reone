/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "reone/game/layouts.h"

#include "reone/common/stream/bytearrayinput.h"
#include "reone/resource/resources.h"

#include "reone/game/format/lytreader.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace game {

shared_ptr<Layout> Layouts::doGet(string resRef) {
    auto data = _resources.get(resRef, ResourceType::Lyt);
    if (!data) {
        return nullptr;
    }
    LytReader lyt;
    lyt.load(make_shared<ByteArrayInputStream>(*data));
    return make_shared<Layout>(lyt.layout());
}

} // namespace game

} // namespace reone
