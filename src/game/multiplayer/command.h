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

#include <string>
#include <vector>

#include "../../core/types.h"
#include "../../game/types.h"
#include "../../net/command.h"

namespace reone {

namespace game {

class Command : public net::Command {
public:
    Command() = default;
    Command(uint32_t id, net::CommandType type);

    void load(const ByteArray &data);

    ByteArray bytes() const override;

    // Getters
    const std::string &module() const;
    const std::string &tag() const;
    CreatureRole role() const;
    int appearance() const;
    const std::vector<std::string> &equipment() const;
    const glm::vec3 &position() const;
    float heading() const;
    const std::string &animation() const;
    int animationFlags() const;
    MovementType movementType() const;
    bool talking() const;
    bool open() const;
    uint32_t triggerrer() const;
    const std::string &resRef() const;
    uint32_t replyIndex() const;

private:
    std::string _module;
    std::string _tag;
    CreatureRole _role { CreatureRole::None };
    int _appearance { 0 };
    std::vector<std::string> _equipment;
    glm::vec3 _position { 0.0f };
    float _heading { 0.0f };
    std::string _animation;
    int _animationFlags { 0 };
    float _animationSpeed { 1.0f };
    MovementType _movementType { MovementType::None };
    bool _talking { false };
    bool _open { false };
    uint32_t _triggerrer { 0 };
    std::string _resRef;
    uint32_t _replyIndex { 0 };

    friend class MultiplayerGame;
};

} // namespace game

} // namespace reone
