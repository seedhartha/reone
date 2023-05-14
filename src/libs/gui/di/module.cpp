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

#include "reone/gui/di/module.h"

using namespace std;

namespace reone {

namespace gui {

void GUIModule::init() {
    _guis = make_unique<GUIs>(_resource.gffs());
    _services = make_unique<GUIServices>(*_guis);
}

void GUIModule::deinit() {
    _services.reset();
    _guis.reset();
}

} // namespace gui

} // namespace reone
