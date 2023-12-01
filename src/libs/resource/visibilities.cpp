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

#include "reone/resource/visibilities.h"

#include "reone/resource/format/visreader.h"
#include "reone/resource/resources.h"
#include "reone/system/stream/memoryinput.h"

namespace reone {

namespace resource {

std::shared_ptr<Visibility> Visibilities::doGet(std::string resRef) {
    auto res = _resources.find(ResourceId(resRef, ResType::Vis));
    if (!res) {
        return nullptr;
    }
    auto stream = MemoryInputStream(res->data);

    VisReader vis;
    vis.load(stream);

    return std::make_shared<Visibility>(vis.visibility());
}

} // namespace resource

} // namespace reone
