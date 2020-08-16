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

#include "../object/creature.h"

namespace reone {

namespace game {

class IMultiplayerCallbacks;

class MultiplayerCreature : public Creature {
public:
    MultiplayerCreature(uint32_t id, IMultiplayerCallbacks *callbacks);

    void setClientTag(const std::string &clientTag);

    bool isControlled() const;
    const std::string &clientTag() const;

private:
    IMultiplayerCallbacks *_callbacks { nullptr };
    std::string _clientTag;

    void animate(const std::string &anim, int flags, float speed) override;
    void updateTransform() override;
    void setMovementType(MovementType type) override;
    void setTalking(bool talking) override;
};

} // namespace game

} // namespace reone
