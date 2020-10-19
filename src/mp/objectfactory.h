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

#include "../game/object/objectfactory.h"

#include "callbacks.h"
#include "types.h"

namespace reone {

namespace mp {

class MultiplayerObjectFactory : public game::ObjectFactory {
public:
    MultiplayerObjectFactory(
        resource::GameVersion version,
        MultiplayerMode mode,
        scene::SceneGraph *sceneGraph,
        IMultiplayerCallbacks *callbacks,
        const render::GraphicsOptions &opts);

    std::unique_ptr<game::Area> newArea();
    std::unique_ptr<game::Creature> newCreature();
    std::unique_ptr<game::Door> newDoor();

private:
    MultiplayerMode _mode { MultiplayerMode::None };
    IMultiplayerCallbacks *_callbacks { nullptr };
};

} // namespace mp

} // namespace reone
