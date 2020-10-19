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

#include "door.h"

#include "callbacks.h"

using namespace std;

using namespace reone::game;
using namespace reone::scene;

namespace reone {

namespace mp {

MultiplayerDoor::MultiplayerDoor(uint32_t id, SceneGraph *sceneGraph, IMultiplayerCallbacks *callbacks) :
    Door(id, sceneGraph), _callbacks(callbacks) {
}

void MultiplayerDoor::open(const shared_ptr<Object> &trigerrer) {
    Door::open(trigerrer);
    _callbacks->onDoorOpen(*this, trigerrer);
}

} // namespace mp

} // namespace reone
