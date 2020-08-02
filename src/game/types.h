#pragma once

#include <map>

#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

#include "../audio/types.h"
#include "../net/types.h"
#include "../render/types.h"

namespace reone {

namespace game {

enum class ObjectType {
    None,
    Creature,
    Door,
    Placeable,
    Waypoint,
    Trigger
};

enum ObstacleMask {
    kObstacleRoom = 1,
    kObstacleCreature = 2,
    kObstacleDoor = 4,
    kObstaclePlaceable = 8
};

enum class MultiplayerMode {
    None,
    Server,
    Client
};

enum class CreatureRole {
    None,
    PartyLeader,
    PartyMember1,
    PartyMember2
};

enum class MovementType {
    None,
    Walk,
    Run
};

enum class ItemType {
    None,
    Armor,
    RightWeapon
};

enum InventorySlot {
    kInventorySlotHead = 0,
    kInventorySlotBody = 1,
    kInventorySlotHands = 3,
    kInventorySlotRightWeapon = 4,
    kInventorySlotLeftWeapon = 5,
    kInventorySlotLeftArm = 7,
    kInventorySlotRightArm = 8,
    kInventorySlotImplant = 9,
    kInventorySlotBelt = 10,
    kInventorySlotCWeaponL = 14,
    kInventorySlotCWeaponR = 15,
    kInventorySlotCWeaponB = 16,
    kInventorySlotCArmour = 17,
    kInventorySlotRightWeapon2 = 18,
    kInventorySlotLeftWeapon2 = 19
};

struct Options {
    render::GraphicsOptions graphics;
    audio::AudioOptions audio;
    net::NetworkOptions network;
};

struct UpdateContext {
    float deltaTime { 0.0f };
    glm::vec3 cameraPosition { 0.0f };
    glm::mat4 projection { 1.0f };
    glm::mat4 view { 1.0f };
};

struct CreatureState {
    glm::vec3 position { 0.0f };
    float heading { 0.0f };
};

struct DoorState {
    bool open { false };
};

struct AreaState {
    std::map<std::string, CreatureState> creatures;
    std::map<std::string, DoorState> doors;
};

struct GameState {
    std::map<std::string, AreaState> areas;
};

} // namespace game

} // namespace reone
