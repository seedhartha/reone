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

#include "reone/resource/2das.h"

#include "reone/system/stream/bytearrayinput.h"

#include "reone/resource/format/2dareader.h"
#include "reone/resource/resources.h"

using namespace std;
using namespace std::placeholders;

namespace reone {

namespace resource {

shared_ptr<TwoDa> TwoDas::doGet(const string &resRef) {
    auto raw = _resources.get(resRef, ResourceType::TwoDa);
    if (!raw) {
        return nullptr;
    }
    auto stream = ByteArrayInputStream(*raw);

    TwoDaReader twoDa;
    twoDa.load(stream);

    return twoDa.twoDa();
}

} // namespace resource

} // namespace reone
