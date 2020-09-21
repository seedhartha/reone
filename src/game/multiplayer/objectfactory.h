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

#include "../object/factory.h"

#include "callbacks.h"

namespace reone {

namespace game {

class MultiplayerObjectFactory : public ObjectFactory {
public:
    MultiplayerObjectFactory(
        resources::GameVersion version,
        MultiplayerMode mode,
        IMultiplayerCallbacks *callbacks,
        const Options &opts);

    std::unique_ptr<Area> newArea();
    std::unique_ptr<Creature> newCreature();
    std::unique_ptr<Door> newDoor();

private:
    MultiplayerMode _mode { MultiplayerMode::None };
    IMultiplayerCallbacks *_callbacks { nullptr };
};

} // namespace game

} // namespace reone
