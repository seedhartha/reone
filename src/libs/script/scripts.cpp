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

#include "reone/script/scripts.h"

#include "reone/system/stream/memoryinput.h"

#include "reone/script/format/ncsreader.h"

using namespace reone::resource;

namespace reone {

namespace script {

std::shared_ptr<ScriptProgram> Scripts::doGet(std::string resRef) {
    std::shared_ptr<ByteArray> data(_resources.get(resRef, ResourceType::Ncs));
    if (!data)
        return nullptr;

    auto ncs = MemoryInputStream(*data);
    auto reader = NcsReader(resRef);
    reader.load(ncs);

    return reader.program();
}

} // namespace script

} // namespace reone
