/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "module.h"

#include "area.h"

using namespace reone::render;
using namespace reone::resources;

namespace reone {

namespace game {

MultiplayerModule::MultiplayerModule(
    const std::string &name,
    MultiplayerMode mode,
    GameVersion version,
    const GraphicsOptions &opts,
    IMultiplayerCallbacks *callbacks
) :
    Module(name, version, opts), _callbacks(callbacks) {
}

const std::shared_ptr<Area> MultiplayerModule::makeArea() const {
    return std::shared_ptr<Area>(new MultiplayerArea(_mode, _version, _info.entryArea, _callbacks));
}

} // namespace game

} // namespace reone
