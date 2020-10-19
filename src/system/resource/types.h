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

#include "../types.h"

namespace reone {

namespace resource {

enum class GameVersion {
    KotOR,
    TheSithLords
};

enum class ResourceType : uint16_t {
    Invalid = 0xffff,
    Bmp = 1,
    Tga = 3,
    Wav = 4,
    PackedLayerTexture = 6,
    Ini = 7,
    Text = 10,
    Model = 2002,
    ScriptSource = 2009,
    CompiledScript = 2010,
    Area = 2012,
    Tileset = 2013,
    ModuleInfo = 2014,
    Creature = 2015,
    Walkmesh = 2016,
    TwoDa = 2017,
    ExtraTextureInfo = 2022,
    GameInstance = 2023,
    Bti = 2024,
    ItemBlueprint = 2025,
    Btc = 2026,
    CreatureBlueprint = 2027,
    Conversation = 2029,
    TilePalette = 2030,
    TriggerBlueprint = 2032,
    Dds = 2033,
    SoundBlueprint = 2035,
    LetterComboProbability = 2036,
    Gff = 2037,
    Faction = 2038,
    EncounterBlueprint = 2040,
    DoorBlueprint = 2042,
    PlaceableBlueprint = 2044,
    DefaultValues = 2045,
    GameInstanceComments = 2046,
    Gui = 2047,
    MerchantBlueprint = 2051,
    DoorWalkmesh = 2052,
    PlaceableWalkmesh = 2053,
    Journal = 2056,
    WaypointBlueprint = 2058,
    SoundSet = 2060,
    ScriptDebugger = 2064,
    PlotManager = 2065,
    PlotWizardBlueprint = 2066,
    AreaLayout = 3000,
    Vis = 3001,
    Path = 3003,
    Lip = 3004,
    Texture = 3007,
    Mdx = 3008,
    Mp3 = 4000
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

typedef std::multimap<std::string, std::string> Visibility;

class IResourceProvider {
public:
    virtual ~IResourceProvider() {
    }

    virtual bool supports(ResourceType type) const = 0;
    virtual std::shared_ptr<ByteArray> find(const std::string &resRef, ResourceType type) = 0;
};

} // namespace resources

} // namespace reone
