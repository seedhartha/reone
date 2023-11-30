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

#include "reone/resource/format/ssfreader.h"

#include "reone/resource/format/signutil.h"

namespace reone {

namespace resource {

void SsfReader::load() {
    checkSignature(_ssf, std::string("SSF V1.1", 8));
    uint32_t tableOffset = _ssf.readUint32();
    int entryCount = static_cast<int>((_ssf.length() - tableOffset) / 4);
    _ssf.seek(tableOffset);
    _soundSet = _ssf.readInt32Array(entryCount);
}

} // namespace resource

} // namespace reone
