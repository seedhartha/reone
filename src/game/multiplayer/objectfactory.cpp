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

#include "objectfactory.h"

#include "area.h"
#include "creature.h"
#include "door.h"

using namespace std;

using namespace reone::render;
using namespace reone::resources;

namespace reone {

namespace game {

MultiplayerObjectFactory::MultiplayerObjectFactory(
    GameVersion version,
    MultiplayerMode mode,
    IMultiplayerCallbacks *callbacks,
    const Options &opts
) :
    ObjectFactory(version, opts), _mode(mode), _callbacks(callbacks) {
}

unique_ptr<Area> MultiplayerObjectFactory::newArea() {
    return make_unique<MultiplayerArea>(_counter++, _version, _mode, this, _callbacks);
}

unique_ptr<Creature> MultiplayerObjectFactory::newCreature() {
    return make_unique<MultiplayerCreature>(_counter++, this, _callbacks);
}

unique_ptr<Door> MultiplayerObjectFactory::newDoor() {
    return make_unique<MultiplayerDoor>(_counter++, _callbacks);
}

} // namespace game

} // namespace reone
