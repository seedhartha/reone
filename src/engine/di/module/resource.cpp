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

#include "resource.h"

using namespace std;

using namespace reone::resource;

namespace reone {

void ResourceModule::init() {
    _resources = make_unique<Resources>();
    _strings = make_unique<Strings>();
    _twoDas = make_unique<TwoDas>(*_resources);
    _gffs = make_unique<Gffs>(*_resources);

    _strings->init(_gamePath);
}

void ResourceModule::deinit() {
    _gffs.reset();
    _twoDas.reset();
    _strings.reset();
    _resources.reset();
}

} // namespace reone
