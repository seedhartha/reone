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

#include <memory>
#include <string>
#include <vector>

#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

#include "../audio/types.h"
#include "../net/types.h"
#include "../render/texture.h"
#include "../render/types.h"
#include "../resource/types.h"

#include "rp/types.h"

namespace reone {

namespace game {

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

enum class GameScreen {
    None,
    MainMenu,
    Loading,
    CharacterGeneration,
    InGame,
    Dialog,
    Container,
    Equipment,
    PartySelection
};

enum class ObjectType {
    None,
    Module,
    Area,
    Creature,
    Door,
    Placeable,
    Waypoint,
    Trigger,
    Item
};

enum class MovementType {
    None,
    Walk,
    Run
};

enum class CameraType {
    FirstPerson,
    ThirdPerson
};

class CreatureBlueprint;

struct Options {
    render::GraphicsOptions graphics;
    audio::AudioOptions audio;
    net::NetworkOptions network;
};

struct CreatureConfiguration {
    std::shared_ptr<CreatureBlueprint> blueprint;
    Gender gender { Gender::Male };
    ClassType clazz { ClassType::Soldier };
    int appearance { 0 };
    std::vector<std::string> equipment;

    bool operator==(const CreatureConfiguration &other) {
        return
            blueprint == other.blueprint &&
            gender == other.gender &&
            clazz == other.clazz &&
            appearance == other.appearance &&
            equipment == other.equipment;
    }
};

struct UpdateContext {
    float deltaTime { 0.0f };
    glm::vec3 cameraPosition { 0.0f };
    glm::mat4 projection { 1.0f };
    glm::mat4 view { 1.0f };
};

struct HudContext {
    std::vector<std::shared_ptr<render::Texture>> partyPortraits;
};

struct SelectionContext {
    bool hasHilighted { false };
    bool hasSelected { false };
    glm::vec3 hilightedScreenCoords { 0.0f };
    glm::vec3 selectedScreenCoords { 0.0f };
};

struct DebugObject {
    std::string tag;
    std::string text;
    glm::vec3 screenCoords { 0.0f };
};

struct DebugContext {
    std::vector<DebugObject> objects;
};

struct GuiContext {
    HudContext hud;
    SelectionContext selection;
    DebugContext debug;
};

struct Portrait {
    std::string resRef;
    std::shared_ptr<render::Texture> image;
    int appearanceNumber { 0 };
    int appearanceS { 0 };
    int appearanceL { 0 };
};

struct CameraStyle {
    float distance { 0.0f };
    float pitch { 0.0f };
    float height { 0.0f };
    float viewAngle { 0.0f };
};

} // namespace game

} // namespace reone
