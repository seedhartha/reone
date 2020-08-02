#pragma once

#include <string>
#include <vector>

#include "../../core/types.h"
#include "../../game/types.h"

namespace reone {

namespace game {

enum class CommandType {
    LoadModule,
    LoadCreature,
    SetPlayerRole,
    SetObjectTransform,
    SetObjectAnimation,
    SetCreatureMovementType,
    SetDoorOpen
};

class Command {
public:
    Command() = default;
    Command(CommandType type);

    void load(const ByteArray &data);
    ByteArray bytes() const;

    // Getters
    CommandType type() const;
    const std::string &module() const;
    uint32_t objectId() const;
    const std::string &tag() const;
    CreatureRole role() const;
    int appearance() const;
    const std::vector<std::string> &equipment() const;
    const glm::vec3 &position() const;
    float heading() const;
    const std::string &animation() const;
    int animationFlags() const;
    MovementType movementType() const;
    bool open() const;
    const std::string &trigerrer() const;

private:
    CommandType _type { CommandType::LoadModule };
    std::string _module;
    uint32_t _objectId { 0 };
    std::string _tag;
    CreatureRole _role { CreatureRole::None };
    int _appearance { 0 };
    std::vector<std::string> _equipment;
    glm::vec3 _position { 0.0f };
    float _heading { 0.0f };
    std::string _animation;
    int _animationFlags { 0 };
    MovementType _movementType { MovementType::None };
    bool _open { false };
    std::string _trigerrer;

    friend class MultiplayerGame;
};

} // namespace game

} // namespace reone
