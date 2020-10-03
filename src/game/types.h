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

#include <map>
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

namespace reone {

namespace game {

enum class GameScreen {
    None,
    MainMenu,
    Loading,
    ClassSelection,
    QuickOrCustom,
    PortraitSelection,
    InGame,
    Dialog,
    Container,
    Equipment
};

enum class Gender {
    Male = 0,
    Female = 1,
    Both = 2,
    Other = 3,
    None = 4
};

enum class ClassType {
    Soldier = 0,
    Scout = 1,
    Scoundrel = 2,
    JediGuardian = 3,
    JediConsular = 4,
    JediSentinel = 5,
    CombatDroid = 6,
    ExpertDroid = 7,
    Minion = 8
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

enum class CameraType {
    FirstPerson,
    ThirdPerson
};

struct Options {
    render::GraphicsOptions graphics;
    audio::AudioOptions audio;
    net::NetworkOptions network;
};

struct CreatureConfiguration {
    Gender gender { Gender::Male };
    ClassType clazz { ClassType::Soldier };
    int appearance { 0 };
    std::vector<std::string> equipment;
};

struct PartyConfiguration {
    int memberCount { 0 };
    CreatureConfiguration leader;
    CreatureConfiguration member1;
    CreatureConfiguration member2;
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
    PartyConfiguration party;
    std::map<std::string, bool> globalBooleans;
    std::map<std::string, int> globalNumbers;
    std::map<uint32_t, std::map<int, bool>> localBooleans;
    std::map<uint32_t, std::map<int, int>> localNumbers;
};

struct HudContext {
    std::vector<std::shared_ptr<render::Texture>> partyPortraits;
};

struct TargetContext {
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
    TargetContext target;
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

glm::vec3 getBaseColor(resource::GameVersion version);
glm::vec3 getHilightColor(resource::GameVersion version);

} // namespace game

} // namespace reone
