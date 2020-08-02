#include <map>

#include <stdexcept>

#include "types.h"

namespace reone {

namespace resources {

static std::map<ResourceType, std::string> g_extByType = {
    { ResourceType::Bmp, "bmp" },
    { ResourceType::Tga, "tga" },
    { ResourceType::Wav, "wav" },
    { ResourceType::PackedLayerTexture, "plt" },
    { ResourceType::Ini, "init" },
    { ResourceType::Text, "txt" },
    { ResourceType::Model, "mdl" },
    { ResourceType::ScriptSource, "nss" },
    { ResourceType::CompiledScript, "ncs" },
    { ResourceType::Area, "are" },
    { ResourceType::Tileset, "set" },
    { ResourceType::ModuleInfo, "ifo" },
    { ResourceType::Creature, "bic" },
    { ResourceType::Walkmesh, "wok" },
    { ResourceType::TwoDa, "2da" },
    { ResourceType::ExtraTextureInfo, "txi" },
    { ResourceType::GameInstance, "git" },
    { ResourceType::Bti, "bti" },
    { ResourceType::ItemBlueprint, "uti" },
    { ResourceType::Btc, "btc" },
    { ResourceType::CreatureBlueprint, "utc" },
    { ResourceType::Conversation, "dlg" },
    { ResourceType::TilePalette, "itp" },
    { ResourceType::TriggerBlueprint, "utt" },
    { ResourceType::Dds, "dds" },
    { ResourceType::SoundBlueprint, "uts" },
    { ResourceType::LetterComboProbability, "ltr" },
    { ResourceType::Gff, "gff" },
    { ResourceType::Faction, "fac" },
    { ResourceType::EncounterBlueprint, "ute" },
    { ResourceType::DoorBlueprint, "utd" },
    { ResourceType::PlaceableBlueprint, "utp" },
    { ResourceType::DefaultValues, "dft" },
    { ResourceType::GameInstanceComments, "gic" },
    { ResourceType::Gui, "gui" },
    { ResourceType::MerchantBlueprint, "utm" },
    { ResourceType::DoorWalkmesh, "dwk" },
    { ResourceType::PlaceableWalkmesh, "pwk" },
    { ResourceType::Journal, "jrl" },
    { ResourceType::WaypointBlueprint, "utw" },
    { ResourceType::SoundSet, "ssf" },
    { ResourceType::ScriptDebugger, "ndb" },
    { ResourceType::PlotManager, "ptm" },
    { ResourceType::PlotWizardBlueprint, "ptt" },
    { ResourceType::AreaLayout, "lyt" },
    { ResourceType::Vis, "vys" },
    { ResourceType::Texture, "tpc" },
    { ResourceType::Mdx, "mdx" } };

static std::map<std::string, ResourceType> g_typeByExt;

const std::string &getExtByResType(ResourceType type) {
    auto it = g_extByType.find(type);
    if (it != g_extByType.end()) return it->second;

    g_extByType.insert(std::make_pair(type, std::to_string(static_cast<int>(type))));

    return g_extByType[type];
}

ResourceType getResTypeByExt(const std::string &ext) {
    if (g_typeByExt.empty()) {
        for (auto &entry : g_extByType) {
            g_typeByExt.insert(std::make_pair(entry.second, entry.first));
        }
    }
    auto it = g_typeByExt.find(ext);
    if (it == g_typeByExt.end()) {
        throw std::runtime_error("Resource type not found by extension: " + ext);
    }

    return it->second;
}

} // namespace resources

} // namespace reone