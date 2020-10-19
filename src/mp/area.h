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

#include "../game/object/area.h"

#include "command.h"
#include "creature.h"
#include "types.h"

namespace reone {

namespace mp {

class MultiplayerArea : public game::Area {
public:
    MultiplayerArea(
        uint32_t id,
        resource::GameVersion version,
        MultiplayerMode mode,
        game::ObjectFactory *objectFactory,
        scene::SceneGraph *sceneGraph,
        const render::GraphicsOptions &opts,
        IMultiplayerCallbacks *callbacks);

    void execute(const Command &cmd);

    const std::shared_ptr<Object> findCreatureByClientTag(const std::string &clientTag) const;

private:
    IMultiplayerCallbacks *_callbacks { nullptr };

    void executeLoadCreature(const Command &cmd);
    void executeSetPlayerRole(const Command &cmd);
    void executeSetObjectTransform(const Command &cmd);
    void executeSetObjectAnimation(const Command &cmd);
    void executeSetCreatureMovementType(const Command &cmd);
    void executeSetCreatureTalking(const Command &cmd);
    void executeSetDoorOpen(const Command &cmd);
};

} // namespace mp

} // namespace reone
