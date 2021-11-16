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

#include "2das.h"

#include "../common/streamutil.h"

#include "format/2dareader.h"
#include "resources.h"

using namespace std;
using namespace std::placeholders;

namespace reone {

namespace resource {

TwoDas::TwoDas(Resources &resources) :
    MemoryCache(bind(&TwoDas::doGet, this, _1)),
    _resources(resources) {
}

shared_ptr<TwoDA> TwoDas::doGet(const string &resRef) {
    auto raw = _resources.getRaw(resRef, ResourceType::TwoDa);
    if (!raw) {
        return nullptr;
    }
    TwoDaReader twoDa;
    twoDa.load(wrap(raw));
    return twoDa.twoDa();
}

} // namespace resource

} // namespace reone
