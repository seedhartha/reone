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

#pragma once

#include "../game/object/door.h"

namespace reone {

namespace mp {

class IMultiplayerCallbacks;

class MultiplayerDoor : public game::Door {
public:
    MultiplayerDoor(uint32_t id, scene::SceneGraph *sceneGraph, IMultiplayerCallbacks *callbacks);

    void open(const std::shared_ptr<game::Object> &trigerrer) override;

private:
    IMultiplayerCallbacks *_callbacks { nullptr };;
};

} // namespace mp

} // namespace reone
