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

#include "reone/resource/provider/scripts.h"

#include "reone/resource/format/ncsreader.h"
#include "reone/system/stream/memoryinput.h"

using namespace reone::script;

namespace reone {

namespace resource {

std::shared_ptr<ScriptProgram> Scripts::doGet(std::string resRef) {
    auto res = _resources.find(ResourceId(resRef, ResType::Ncs));
    if (!res) {
        return nullptr;
    }
    auto stream = MemoryInputStream(res->data);
    auto reader = NcsReader(stream, resRef);
    reader.load();
    return reader.program();
}

} // namespace resource

} // namespace reone
