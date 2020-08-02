#pragma once

#include <map>
#include <memory>
#include <string>

#include "../core/types.h"

namespace reone {

namespace resources {

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
    Texture = 3007,
    Mdx = 3008
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
