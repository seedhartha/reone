/*
 * Copyright (c) 2020-2021 The reone project contributors
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

#include "kotor.h"

#include "../../audio/files.h"
#include "../../audio/player.h"
#include "../../common/collectionutil.h"
#include "../../common/exception/validation.h"
#include "../../common/logutil.h"
#include "../../common/pathutil.h"
#include "../../common/streamutil.h"
#include "../../common/streamwriter.h"
#include "../../graphics/features.h"
#include "../../graphics/format/tgawriter.h"
#include "../../graphics/lips.h"
#include "../../graphics/models.h"
#include "../../graphics/pbribl.h"
#include "../../graphics/renderbuffer.h"
#include "../../graphics/textures.h"
#include "../../graphics/walkmeshes.h"
#include "../../graphics/window.h"
#include "../../gui/gui.h"
#include "../../movie/format/bikreader.h"
#include "../../resource/2das.h"
#include "../../resource/format/erfreader.h"
#include "../../resource/format/erfwriter.h"
#include "../../resource/format/gffwriter.h"
#include "../../resource/gffs.h"
#include "../../resource/resources.h"
#include "../../scene/pipeline/world.h"
#include "../../script/scripts.h"

#include "../game/combat.h"
#include "../game/cursors.h"
#include "../game/location.h"
#include "../game/object/factory.h"
#include "../game/party.h"
#include "../game/services.h"
#include "../game/soundsets.h"
#include "../game/surfaces.h"

#include "gui/loadscreen.h"
#include "script/routine/declarations.h"

#define VT_V script::VariableType::Void
#define VT_I script::VariableType::Int
#define VT_F script::VariableType::Float
#define VT_O script::VariableType::Object
#define VT_S script::VariableType::String
#define VT_EFFECT script::VariableType::Effect
#define VT_EVENT script::VariableType::Event
#define VT_LOCATION script::VariableType::Location
#define VT_TALENT script::VariableType::Talent
#define VT_VECTOR script::VariableType::Vector
#define VT_ACTION script::VariableType::Action

using namespace std;

using namespace reone::audio;
using namespace reone::game;
using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;
using namespace reone::scene;
using namespace reone::script;

namespace fs = boost::filesystem;

namespace reone {

namespace kotor {

static constexpr char kPatchFilename[] = "patch.erf";
static constexpr char kWavesDirectoryName[] = "streamwaves";
static constexpr char kExeFilename[] = "swkotor.exe";
static constexpr char kModulesDirectoryName[] = "modules";

static constexpr char kBlueprintResRefCarth[] = "p_carth";
static constexpr char kBlueprintResRefBastila[] = "p_bastilla";

static const vector<string> g_nonTransientLipFiles {"global.mod", "localization.mod"};

static bool g_conversationsEnabled = true;

KotOR::KotOR(
    fs::path path,
    Options options,
    Services &services) :
    Game(
        move(path),
        move(options),
        services) {
    _screen = GameScreen::MainMenu;

    _mainMenuMusicResRef = "mus_theme_cult";
    _charGenMusicResRef = "mus_theme_rep";
    _charGenLoadScreenResRef = "load_chargen";

    _guiColorBase = glm::vec3(0.0f, 0.639216f, 0.952941f);
    _guiColorHilight = glm::vec3(0.980392f, 1.0f, 0.0f);
    _guiColorDisabled = glm::vec3(0.0f, 0.349020f, 0.549020f);

    _map.setArrowResRef("mm_barrow");
}

void KotOR::initResourceProviders() {
    _services.resources.indexKeyFile(getPathIgnoreCase(_path, kKeyFilename));
    _services.resources.indexErfFile(getPathIgnoreCase(_path, kPatchFilename));

    fs::path texPacksPath(getPathIgnoreCase(_path, kTexturePackDirectoryName));
    _services.resources.indexErfFile(getPathIgnoreCase(texPacksPath, kGUITexturePackFilename));
    _services.resources.indexErfFile(getPathIgnoreCase(texPacksPath, kTexturePackFilename));

    _services.resources.indexDirectory(getPathIgnoreCase(_path, kMusicDirectoryName));
    _services.resources.indexDirectory(getPathIgnoreCase(_path, kSoundsDirectoryName));
    _services.resources.indexDirectory(getPathIgnoreCase(_path, kWavesDirectoryName));

    fs::path lipsPath(getPathIgnoreCase(_path, kLipsDirectoryName));
    for (auto &filename : g_nonTransientLipFiles) {
        _services.resources.indexErfFile(getPathIgnoreCase(lipsPath, filename));
    }

    _services.resources.indexDirectory(getPathIgnoreCase(_path, kOverrideDirectoryName));
    _services.resources.indexExeFile(getPathIgnoreCase(_path, kExeFilename));
}

void KotOR::loadModuleNames() {
    fs::path modules(getPathIgnoreCase(_path, kModulesDirectoryName));
    if (modules.empty()) {
        throw ValidationException("Modules directory not found");
    }
    for (auto &entry : fs::directory_iterator(modules)) {
        string filename(boost::to_lower_copy(entry.path().filename().string()));
        if (boost::ends_with(filename, ".mod") || (boost::ends_with(filename, ".rim") && !boost::ends_with(filename, "_s.rim"))) {
            string moduleName(boost::to_lower_copy(filename.substr(0, filename.size() - 4)));
            _moduleNames.insert(move(moduleName));
        }
    }
}

void KotOR::start() {
    openMainMenu();

    if (!_options.module.empty()) {
        onModuleSelected(_options.module);
    } else {
        playVideo("legal");
    }
}

void KotOR::loadModuleResources(const string &moduleName) {
    _services.twoDas.invalidate();
    _services.gffs.invalidate();
    _services.resources.clearTransientProviders();

    fs::path modulesPath(getPathIgnoreCase(_path, kModulesDirectoryName));
    if (modulesPath.empty()) {
        throw ValidationException("Modules directory not found");
    }

    fs::path modPath(getPathIgnoreCase(modulesPath, moduleName + ".mod"));
    if (!modPath.empty()) {
        _services.resources.indexErfFile(getPathIgnoreCase(modulesPath, moduleName + ".mod", false));
    } else {
        _services.resources.indexRimFile(getPathIgnoreCase(modulesPath, moduleName + ".rim"));
        _services.resources.indexRimFile(getPathIgnoreCase(modulesPath, moduleName + "_s.rim"));
    }

    fs::path lipsPath(getPathIgnoreCase(_path, kLipsDirectoryName));
    if (!lipsPath.empty()) {
        _services.resources.indexErfFile(getPathIgnoreCase(lipsPath, moduleName + "_loc.mod"));
    }

    if (isTSL()) {
        _services.resources.indexErfFile(getPathIgnoreCase(modulesPath, moduleName + "_dlg.erf"));
    }
}

void KotOR::onModuleSelected(const string &module) {
    _mainMenu->onModuleSelected(module);
}

void KotOR::drawHUD() {
    _hud->draw();
}

CameraType KotOR::getConversationCamera(int &cameraId) const {
    return _conversation->getCamera(cameraId);
}

void KotOR::setBarkBubbleText(string text, float duration) {
    _hud->barkBubble().setBarkText(text, duration);
}

void KotOR::loadInGameMenus() {
    if (!_hud) {
        loadHUD();
    }
    if (!_inGame) {
        loadInGame();
    }
    if (!_dialog) {
        loadDialog();
    }
    if (!_computer) {
        loadComputer();
    }
    if (!_container) {
        loadContainer();
    }
    if (!_partySelect) {
        loadPartySelection();
    }
}

void KotOR::loadMainMenu() {
    _mainMenu = make_unique<MainMenu>(*this, _services);
    _mainMenu->load();
}

void KotOR::loadHUD() {
    _hud = make_unique<HUD>(*this, _services);
    _hud->load();
}

void KotOR::loadDialog() {
    _dialog = make_unique<DialogGUI>(*this, _services);
    _dialog->load();
}

void KotOR::loadComputer() {
    _computer = make_unique<ComputerGUI>(*this, _services);
    _computer->load();
}

void KotOR::loadContainer() {
    _container = make_unique<ContainerGUI>(*this, _services);
    _container->load();
}

void KotOR::loadPartySelection() {
    _partySelect = make_unique<PartySelection>(*this, _services);
    _partySelect->load();
}

void KotOR::loadSaveLoad() {
    _saveLoad = make_unique<SaveLoad>(*this, _services);
    _saveLoad->load();
}

void KotOR::loadLoadingScreen() {
    _loadScreen = make_unique<LoadingScreen>(*this, _services);
    static_cast<LoadingScreen *>(_loadScreen.get())->load();
}

void KotOR::loadCharacterGeneration() {
    _charGen = make_unique<CharacterGeneration>(*this, _services);
    _charGen->load();
}

void KotOR::loadInGame() {
    _inGame = make_unique<InGameMenu>(*this, _services);
    _inGame->load();
}

void KotOR::openMainMenu() {
    if (!_mainMenu) {
        loadMainMenu();
    }
    if (!_saveLoad) {
        loadSaveLoad();
    }
    playMusic(_mainMenuMusicResRef);
    changeScreen(GameScreen::MainMenu);
}

void KotOR::openInGame() {
    changeScreen(GameScreen::InGame);
}

void KotOR::openInGameMenu(InGameMenuTab tab) {
    // Take a screenshot to be used in SaveLoad menu
    _services.window.clear();
    _services.worldRenderPipeline.setTakeScreenshot(true);
    _services.worldRenderPipeline.render();

    setCursorType(CursorType::Default);
    switch (tab) {
    case InGameMenuTab::Equipment:
        _inGame->openEquipment();
        break;
    case InGameMenuTab::Inventory:
        _inGame->openInventory();
        break;
    case InGameMenuTab::Character:
        _inGame->openCharacter();
        break;
    case InGameMenuTab::Abilities:
        _inGame->openAbilities();
        break;
    case InGameMenuTab::Messages:
        _inGame->openMessages();
        break;
    case InGameMenuTab::Journal:
        _inGame->openJournal();
        break;
    case InGameMenuTab::Map:
        _inGame->openMap();
        break;
    case InGameMenuTab::Options:
        _inGame->openOptions();
        break;
    default:
        break;
    }
    changeScreen(GameScreen::InGameMenu);
}

void KotOR::openContainer(const shared_ptr<SpatialObject> &container) {
    stopMovement();
    setRelativeMouseMode(false);
    setCursorType(CursorType::Default);
    _container->open(container);
    changeScreen(GameScreen::Container);
}

void KotOR::openPartySelection(const PartySelectionContext &ctx) {
    stopMovement();
    setRelativeMouseMode(false);
    setCursorType(CursorType::Default);
    _partySelect->prepare(ctx);
    changeScreen(GameScreen::PartySelection);
}

void KotOR::openSaveLoad(SaveLoadMode mode) {
    setRelativeMouseMode(false);
    setCursorType(CursorType::Default);
    _saveLoad->setMode(mode);
    _saveLoad->refresh();
    changeScreen(GameScreen::SaveLoad);
}

void KotOR::openLevelUp() {
    setRelativeMouseMode(false);
    setCursorType(CursorType::Default);
    _charGen->startLevelUp();
    changeScreen(GameScreen::CharacterGeneration);
}

void KotOR::startCharacterGeneration() {
    withLoadingScreen(_charGenLoadScreenResRef, [this]() {
        if (!_charGen) {
            loadCharacterGeneration();
        }
        _loadScreen->setProgress(100);
        drawAll();
        playMusic(_charGenMusicResRef);
        changeScreen(GameScreen::CharacterGeneration);
    });
}

void KotOR::startDialog(const shared_ptr<SpatialObject> &owner, const string &resRef) {
    if (!g_conversationsEnabled)
        return;

    shared_ptr<GffStruct> dlg(_services.gffs.get(resRef, ResourceType::Dlg));
    if (!dlg) {
        warn("Game: conversation not found: " + resRef);
        return;
    }

    stopMovement();
    setRelativeMouseMode(false);
    setCursorType(CursorType::Default);
    changeScreen(GameScreen::Conversation);

    auto dialog = make_shared<Dialog>(resRef, _services.strings);
    dialog->load(*dlg);

    bool computerConversation = dialog->conversationType() == ConversationType::Computer;
    _conversation = computerConversation ? _computer.get() : static_cast<Conversation *>(_dialog.get());
    _conversation->start(dialog, owner);
}

void KotOR::resumeConversation() {
    _conversation->resume();
}

void KotOR::pauseConversation() {
    _conversation->pause();
}

void KotOR::changeScreen(GameScreen screen) {
    GUI *gui = getScreenGUI();
    if (gui) {
        gui->resetFocus();
    }
    _screen = screen;
}

GUI *KotOR::getScreenGUI() const {
    switch (_screen) {
    case GameScreen::MainMenu:
        return _mainMenu.get();
    case GameScreen::Loading:
        return static_cast<LoadingScreen *>(_loadScreen.get());
    case GameScreen::CharacterGeneration:
        return _charGen.get();
    case GameScreen::InGame:
        return _cameraType == CameraType::ThirdPerson ? _hud.get() : nullptr;
    case GameScreen::InGameMenu:
        return _inGame.get();
    case GameScreen::Conversation:
        return _conversation;
    case GameScreen::Container:
        return _container.get();
    case GameScreen::PartySelection:
        return _partySelect.get();
    case GameScreen::SaveLoad:
        return _saveLoad.get();
    default:
        return nullptr;
    }
}

void KotOR::getDefaultPartyMembers(string &member1, string &member2, string &member3) const {
    member1 = kBlueprintResRefCarth;
    member2 = kBlueprintResRefBastila;
    member3.clear();

    Game::getDefaultPartyMembers(member1, member2, member3);
}

void fillScriptRoutinesKotOR(IRoutines &routines) {
    routines.add("Random", VT_I, {VT_I}, &routine::random);
    routines.add("PrintString", VT_V, {VT_S}, &routine::printString);
    routines.add("PrintFloat", VT_V, {VT_F, VT_I, VT_I}, &routine::printFloat);
    routines.add("FloatToString", VT_S, {VT_F, VT_I, VT_I}, &routine::floatToString);
    routines.add("PrintInteger", VT_V, {VT_I}, &routine::printInteger);
    routines.add("PrintObject", VT_V, {VT_O}, &routine::printObject);
    routines.add("AssignCommand", VT_V, {VT_O, VT_ACTION}, &routine::assignCommand);
    routines.add("DelayCommand", VT_V, {VT_F, VT_ACTION}, &routine::delayCommand);
    routines.add("ExecuteScript", VT_V, {VT_S, VT_O, VT_I}, &routine::executeScript);
    routines.add("ClearAllActions", VT_V, {}, &routine::clearAllActions);
    routines.add("SetFacing", VT_V, {VT_F}, &routine::setFacing);
    routines.add("SwitchPlayerCharacter", VT_I, {VT_I}, &routine::switchPlayerCharacter);
    routines.add("SetTime", VT_V, {VT_I, VT_I, VT_I, VT_I}, &routine::setTime);
    routines.add("SetPartyLeader", VT_I, {VT_I}, &routine::setPartyLeader);
    routines.add("SetAreaUnescapable", VT_V, {VT_I}, &routine::setAreaUnescapable);
    routines.add("GetAreaUnescapable", VT_I, {}, &routine::getAreaUnescapable);
    routines.add("GetTimeHour", VT_I, {}, &routine::getTimeHour);
    routines.add("GetTimeMinute", VT_I, {}, &routine::getTimeMinute);
    routines.add("GetTimeSecond", VT_I, {}, &routine::getTimeSecond);
    routines.add("GetTimeMillisecond", VT_I, {}, &routine::getTimeMillisecond);
    routines.add("ActionRandomWalk", VT_V, {}, &routine::actionRandomWalk);
    routines.add("ActionMoveToLocation", VT_V, {VT_LOCATION, VT_I}, &routine::actionMoveToLocation);
    routines.add("ActionMoveToObject", VT_V, {VT_O, VT_I, VT_F}, &routine::actionMoveToObject);
    routines.add("ActionMoveAwayFromObject", VT_V, {VT_O, VT_I, VT_F}, &routine::actionMoveAwayFromObject);
    routines.add("GetArea", VT_O, {VT_O}, &routine::getArea);
    routines.add("GetEnteringObject", VT_O, {}, &routine::getEnteringObject);
    routines.add("GetExitingObject", VT_O, {}, &routine::getExitingObject);
    routines.add("GetPosition", VT_VECTOR, {VT_O}, &routine::getPosition);
    routines.add("GetFacing", VT_F, {VT_O}, &routine::getFacing);
    routines.add("GetItemPossessor", VT_O, {VT_O}, &routine::getItemPossessor);
    routines.add("GetItemPossessedBy", VT_O, {VT_O, VT_S}, &routine::getItemPossessedBy);
    routines.add("CreateItemOnObject", VT_O, {VT_S, VT_O, VT_I}, &routine::createItemOnObject);
    routines.add("ActionEquipItem", VT_V, {VT_O, VT_I, VT_I}, &routine::actionEquipItem);
    routines.add("ActionUnequipItem", VT_V, {VT_O, VT_I}, &routine::actionUnequipItem);
    routines.add("ActionPickUpItem", VT_V, {VT_O}, &routine::actionPickUpItem);
    routines.add("ActionPutDownItem", VT_V, {VT_O}, &routine::actionPutDownItem);
    routines.add("GetLastAttacker", VT_O, {VT_O}, &routine::getLastAttacker);
    routines.add("ActionAttack", VT_V, {VT_O, VT_I}, &routine::actionAttack);
    routines.add("GetNearestCreature", VT_O, {VT_I, VT_I, VT_O, VT_I, VT_I, VT_I, VT_I, VT_I}, &routine::getNearestCreature);
    routines.add("ActionSpeakString", VT_V, {VT_S, VT_I}, &routine::actionSpeakString);
    routines.add("ActionPlayAnimation", VT_V, {VT_I, VT_F, VT_F}, &routine::actionPlayAnimation);
    routines.add("GetDistanceToObject", VT_F, {VT_O}, &routine::getDistanceToObject);
    routines.add("GetIsObjectValid", VT_I, {VT_O}, &routine::getIsObjectValid);
    routines.add("ActionOpenDoor", VT_V, {VT_O}, &routine::actionOpenDoor);
    routines.add("ActionCloseDoor", VT_V, {VT_O}, &routine::actionCloseDoor);
    routines.add("SetCameraFacing", VT_V, {VT_F}, &routine::setCameraFacing);
    routines.add("PlaySound", VT_V, {VT_S}, &routine::playSound);
    routines.add("GetSpellTargetObject", VT_O, {}, &routine::getSpellTargetObject);
    routines.add("ActionCastSpellAtObject", VT_V, {VT_I, VT_O, VT_I, VT_I, VT_I, VT_I, VT_I}, &routine::actionCastSpellAtObject);
    routines.add("GetCurrentHitPoints", VT_I, {VT_O}, &routine::getCurrentHitPoints);
    routines.add("GetMaxHitPoints", VT_I, {VT_O}, &routine::getMaxHitPoints);
    routines.add("EffectAssuredHit", VT_EFFECT, {}, &routine::effectAssuredHit);
    routines.add("GetLastItemEquipped", VT_O, {}, &routine::getLastItemEquipped);
    routines.add("GetSubScreenID", VT_I, {}, &routine::getSubScreenID);
    routines.add("CancelCombat", VT_V, {VT_O}, &routine::cancelCombat);
    routines.add("GetCurrentForcePoints", VT_I, {VT_O}, &routine::getCurrentForcePoints);
    routines.add("GetMaxForcePoints", VT_I, {VT_O}, &routine::getMaxForcePoints);
    routines.add("PauseGame", VT_V, {VT_I}, &routine::pauseGame);
    routines.add("SetPlayerRestrictMode", VT_V, {VT_I}, &routine::setPlayerRestrictMode);
    routines.add("GetStringLength", VT_I, {VT_S}, &routine::getStringLength);
    routines.add("GetStringUpperCase", VT_S, {VT_S}, &routine::getStringUpperCase);
    routines.add("GetStringLowerCase", VT_S, {VT_S}, &routine::getStringLowerCase);
    routines.add("GetStringRight", VT_S, {VT_S, VT_I}, &routine::getStringRight);
    routines.add("GetStringLeft", VT_S, {VT_S, VT_I}, &routine::getStringLeft);
    routines.add("InsertString", VT_S, {VT_S, VT_S, VT_I}, &routine::insertString);
    routines.add("GetSubString", VT_S, {VT_S, VT_I, VT_I}, &routine::getSubString);
    routines.add("FindSubString", VT_I, {VT_S, VT_S}, &routine::findSubString);
    routines.add("fabs", VT_F, {VT_F}, &routine::fabs);
    routines.add("cos", VT_F, {VT_F}, &routine::cos);
    routines.add("sin", VT_F, {VT_F}, &routine::sin);
    routines.add("tan", VT_F, {VT_F}, &routine::tan);
    routines.add("acos", VT_F, {VT_F}, &routine::acos);
    routines.add("asin", VT_F, {VT_F}, &routine::asin);
    routines.add("atan", VT_F, {VT_F}, &routine::atan);
    routines.add("log", VT_F, {VT_F}, &routine::log);
    routines.add("pow", VT_F, {VT_F, VT_F}, &routine::pow);
    routines.add("sqrt", VT_F, {VT_F}, &routine::sqrt);
    routines.add("abs", VT_I, {VT_I}, &routine::abs);
    routines.add("EffectHeal", VT_EFFECT, {VT_I}, &routine::effectHeal);
    routines.add("EffectDamage", VT_EFFECT, {VT_I, VT_I, VT_I}, &routine::effectDamage);
    routines.add("EffectAbilityIncrease", VT_EFFECT, {VT_I, VT_I}, &routine::effectAbilityIncrease);
    routines.add("EffectDamageResistance", VT_EFFECT, {VT_I, VT_I, VT_I}, &routine::effectDamageResistance);
    routines.add("EffectResurrection", VT_EFFECT, {}, &routine::effectResurrection);
    routines.add("GetPlayerRestrictMode", VT_I, {VT_O}, &routine::getPlayerRestrictMode);
    routines.add("GetCasterLevel", VT_I, {VT_O}, &routine::getCasterLevel);
    routines.add("GetFirstEffect", VT_EFFECT, {VT_O}, &routine::getFirstEffect);
    routines.add("GetNextEffect", VT_EFFECT, {VT_O}, &routine::getNextEffect);
    routines.add("RemoveEffect", VT_V, {VT_O, VT_EFFECT}, &routine::removeEffect);
    routines.add("GetIsEffectValid", VT_I, {VT_EFFECT}, &routine::getIsEffectValid);
    routines.add("GetEffectDurationType", VT_I, {VT_EFFECT}, &routine::getEffectDurationType);
    routines.add("GetEffectSubType", VT_I, {VT_EFFECT}, &routine::getEffectSubType);
    routines.add("GetEffectCreator", VT_O, {VT_EFFECT}, &routine::getEffectCreator);
    routines.add("IntToString", VT_S, {VT_I}, &routine::intToString);
    routines.add("GetFirstObjectInArea", VT_O, {VT_O, VT_I}, &routine::getFirstObjectInArea);
    routines.add("GetNextObjectInArea", VT_O, {VT_O, VT_I}, &routine::getNextObjectInArea);
    routines.add("d2", VT_I, {VT_I}, &routine::d2);
    routines.add("d3", VT_I, {VT_I}, &routine::d3);
    routines.add("d4", VT_I, {VT_I}, &routine::d4);
    routines.add("d6", VT_I, {VT_I}, &routine::d6);
    routines.add("d8", VT_I, {VT_I}, &routine::d8);
    routines.add("d10", VT_I, {VT_I}, &routine::d10);
    routines.add("d12", VT_I, {VT_I}, &routine::d12);
    routines.add("d20", VT_I, {VT_I}, &routine::d20);
    routines.add("d100", VT_I, {VT_I}, &routine::d100);
    routines.add("VectorMagnitude", VT_F, {VT_VECTOR}, &routine::vectorMagnitude);
    routines.add("GetMetaMagicFeat", VT_I, {}, &routine::getMetaMagicFeat);
    routines.add("GetObjectType", VT_I, {VT_O}, &routine::getObjectType);
    routines.add("GetRacialType", VT_I, {VT_O}, &routine::getRacialType);
    routines.add("FortitudeSave", VT_I, {VT_O, VT_I, VT_I, VT_O}, &routine::fortitudeSave);
    routines.add("ReflexSave", VT_I, {VT_O, VT_I, VT_I, VT_O}, &routine::reflexSave);
    routines.add("WillSave", VT_I, {VT_O, VT_I, VT_I, VT_O}, &routine::willSave);
    routines.add("GetSpellSaveDC", VT_I, {}, &routine::getSpellSaveDC);
    routines.add("MagicalEffect", VT_EFFECT, {VT_EFFECT}, &routine::magicalEffect);
    routines.add("SupernaturalEffect", VT_EFFECT, {VT_EFFECT}, &routine::supernaturalEffect);
    routines.add("ExtraordinaryEffect", VT_EFFECT, {VT_EFFECT}, &routine::extraordinaryEffect);
    routines.add("EffectACIncrease", VT_EFFECT, {VT_I, VT_I, VT_I}, &routine::effectACIncrease);
    routines.add("GetAC", VT_I, {VT_O, VT_I}, &routine::getAC);
    routines.add("EffectSavingThrowIncrease", VT_EFFECT, {VT_I, VT_I, VT_I}, &routine::effectSavingThrowIncrease);
    routines.add("EffectAttackIncrease", VT_EFFECT, {VT_I, VT_I}, &routine::effectAttackIncrease);
    routines.add("EffectDamageReduction", VT_EFFECT, {VT_I, VT_I, VT_I}, &routine::effectDamageReduction);
    routines.add("EffectDamageIncrease", VT_EFFECT, {VT_I, VT_I}, &routine::effectDamageIncrease);
    routines.add("RoundsToSeconds", VT_F, {VT_I}, &routine::roundsToSeconds);
    routines.add("HoursToSeconds", VT_F, {VT_I}, &routine::hoursToSeconds);
    routines.add("TurnsToSeconds", VT_F, {VT_I}, &routine::turnsToSeconds);
    routines.add("SoundObjectSetFixedVariance", VT_V, {VT_O, VT_F}, &routine::soundObjectSetFixedVariance);
    routines.add("GetGoodEvilValue", VT_I, {VT_O}, &routine::getGoodEvilValue);
    routines.add("GetPartyMemberCount", VT_I, {}, &routine::getPartyMemberCount);
    routines.add("GetAlignmentGoodEvil", VT_I, {VT_O}, &routine::getAlignmentGoodEvil);
    routines.add("GetFirstObjectInShape", VT_O, {VT_I, VT_F, VT_LOCATION, VT_I, VT_I, VT_VECTOR}, &routine::getFirstObjectInShape);
    routines.add("GetNextObjectInShape", VT_O, {VT_I, VT_F, VT_LOCATION, VT_I, VT_I, VT_VECTOR}, &routine::getNextObjectInShape);
    routines.add("EffectEntangle", VT_EFFECT, {}, &routine::effectEntangle);
    routines.add("SignalEvent", VT_V, {VT_O, VT_EVENT}, &routine::signalEvent);
    routines.add("EventUserDefined", VT_EVENT, {VT_I}, &routine::eventUserDefined);
    routines.add("EffectDeath", VT_EFFECT, {VT_I, VT_I}, &routine::effectDeath);
    routines.add("EffectKnockdown", VT_EFFECT, {}, &routine::effectKnockdown);
    routines.add("ActionGiveItem", VT_V, {VT_O, VT_O}, &routine::actionGiveItem);
    routines.add("ActionTakeItem", VT_V, {VT_O, VT_O}, &routine::actionTakeItem);
    routines.add("VectorNormalize", VT_VECTOR, {VT_VECTOR}, &routine::vectorNormalize);
    routines.add("GetItemStackSize", VT_I, {VT_O}, &routine::getItemStackSize);
    routines.add("GetAbilityScore", VT_I, {VT_O, VT_I}, &routine::getAbilityScore);
    routines.add("GetIsDead", VT_I, {VT_O}, &routine::getIsDead);
    routines.add("PrintVector", VT_V, {VT_VECTOR, VT_I}, &routine::printVector);
    routines.add("Vector", VT_VECTOR, {VT_F, VT_F, VT_F}, &routine::vectorCreate);
    routines.add("SetFacingPoint", VT_V, {VT_VECTOR}, &routine::setFacingPoint);
    routines.add("AngleToVector", VT_VECTOR, {VT_F}, &routine::angleToVector);
    routines.add("VectorToAngle", VT_F, {VT_VECTOR}, &routine::vectorToAngle);
    routines.add("TouchAttackMelee", VT_I, {VT_O, VT_I}, &routine::touchAttackMelee);
    routines.add("TouchAttackRanged", VT_I, {VT_O, VT_I}, &routine::touchAttackRanged);
    routines.add("EffectParalyze", VT_EFFECT, {}, &routine::effectParalyze);
    routines.add("EffectSpellImmunity", VT_EFFECT, {VT_I}, &routine::effectSpellImmunity);
    routines.add("SetItemStackSize", VT_V, {VT_O, VT_I}, &routine::setItemStackSize);
    routines.add("GetDistanceBetween", VT_F, {VT_O, VT_O}, &routine::getDistanceBetween);
    routines.add("SetReturnStrref", VT_V, {VT_I, VT_I, VT_I}, &routine::setReturnStrref);
    routines.add("EffectForceJump", VT_EFFECT, {VT_O, VT_I}, &routine::effectForceJump);
    routines.add("EffectSleep", VT_EFFECT, {}, &routine::effectSleep);
    routines.add("GetItemInSlot", VT_O, {VT_I, VT_O}, &routine::getItemInSlot);
    routines.add("EffectTemporaryForcePoints", VT_EFFECT, {VT_I}, &routine::effectTemporaryForcePoints);
    routines.add("EffectConfused", VT_EFFECT, {}, &routine::effectConfused);
    routines.add("EffectFrightened", VT_EFFECT, {}, &routine::effectFrightened);
    routines.add("EffectChoke", VT_EFFECT, {}, &routine::effectChoke);
    routines.add("SetGlobalString", VT_V, {VT_S, VT_S}, &routine::setGlobalString);
    routines.add("EffectStunned", VT_EFFECT, {}, &routine::effectStunned);
    routines.add("SetCommandable", VT_V, {VT_I, VT_O}, &routine::setCommandable);
    routines.add("GetCommandable", VT_I, {VT_O}, &routine::getCommandable);
    routines.add("EffectRegenerate", VT_EFFECT, {VT_I, VT_F}, &routine::effectRegenerate);
    routines.add("EffectMovementSpeedIncrease", VT_EFFECT, {VT_I}, &routine::effectMovementSpeedIncrease);
    routines.add("GetHitDice", VT_I, {VT_O}, &routine::getHitDice);
    routines.add("ActionForceFollowObject", VT_V, {VT_O, VT_F}, &routine::actionForceFollowObject);
    routines.add("GetTag", VT_S, {VT_O}, &routine::getTag);
    routines.add("ResistForce", VT_I, {VT_O, VT_O}, &routine::resistForce);
    routines.add("GetEffectType", VT_I, {VT_EFFECT}, &routine::getEffectType);
    routines.add("EffectAreaOfEffect", VT_EFFECT, {VT_I, VT_S, VT_S, VT_S}, &routine::effectAreaOfEffect);
    routines.add("GetFactionEqual", VT_I, {VT_O, VT_O}, &routine::getFactionEqual);
    routines.add("ChangeFaction", VT_V, {VT_O, VT_O}, &routine::changeFaction);
    routines.add("GetIsListening", VT_I, {VT_O}, &routine::getIsListening);
    routines.add("SetListening", VT_V, {VT_O, VT_I}, &routine::setListening);
    routines.add("SetListenPattern", VT_V, {VT_O, VT_S, VT_I}, &routine::setListenPattern);
    routines.add("TestStringAgainstPattern", VT_I, {VT_S, VT_S}, &routine::testStringAgainstPattern);
    routines.add("GetMatchedSubstring", VT_S, {VT_I}, &routine::getMatchedSubstring);
    routines.add("GetMatchedSubstringsCount", VT_I, {}, &routine::getMatchedSubstringsCount);
    routines.add("EffectVisualEffect", VT_EFFECT, {VT_I, VT_I}, &routine::effectVisualEffect);
    routines.add("GetFactionWeakestMember", VT_O, {VT_O, VT_I}, &routine::getFactionWeakestMember);
    routines.add("GetFactionStrongestMember", VT_O, {VT_O, VT_I}, &routine::getFactionStrongestMember);
    routines.add("GetFactionMostDamagedMember", VT_O, {VT_O, VT_I}, &routine::getFactionMostDamagedMember);
    routines.add("GetFactionLeastDamagedMember", VT_O, {VT_O, VT_I}, &routine::getFactionLeastDamagedMember);
    routines.add("GetFactionGold", VT_I, {VT_O}, &routine::getFactionGold);
    routines.add("GetFactionAverageReputation", VT_I, {VT_O, VT_O}, &routine::getFactionAverageReputation);
    routines.add("GetFactionAverageGoodEvilAlignment", VT_I, {VT_O}, &routine::getFactionAverageGoodEvilAlignment);
    routines.add("SoundObjectGetFixedVariance", VT_F, {VT_O}, &routine::soundObjectGetFixedVariance);
    routines.add("GetFactionAverageLevel", VT_I, {VT_O}, &routine::getFactionAverageLevel);
    routines.add("GetFactionAverageXP", VT_I, {VT_O}, &routine::getFactionAverageXP);
    routines.add("GetFactionMostFrequentClass", VT_I, {VT_O}, &routine::getFactionMostFrequentClass);
    routines.add("GetFactionWorstAC", VT_O, {VT_O, VT_I}, &routine::getFactionWorstAC);
    routines.add("GetFactionBestAC", VT_O, {VT_O, VT_I}, &routine::getFactionBestAC);
    routines.add("GetGlobalString", VT_S, {VT_S}, &routine::getGlobalString);
    routines.add("GetListenPatternNumber", VT_I, {}, &routine::getListenPatternNumber);
    routines.add("ActionJumpToObject", VT_V, {VT_O, VT_I}, &routine::actionJumpToObject);
    routines.add("GetWaypointByTag", VT_O, {VT_S}, &routine::getWaypointByTag);
    routines.add("GetTransitionTarget", VT_O, {VT_O}, &routine::getTransitionTarget);
    routines.add("EffectLinkEffects", VT_EFFECT, {VT_EFFECT, VT_EFFECT}, &routine::effectLinkEffects);
    routines.add("GetObjectByTag", VT_O, {VT_S, VT_I}, &routine::getObjectByTag);
    routines.add("AdjustAlignment", VT_V, {VT_O, VT_I, VT_I}, &routine::adjustAlignment);
    routines.add("ActionWait", VT_V, {VT_F}, &routine::actionWait);
    routines.add("SetAreaTransitionBMP", VT_V, {VT_I, VT_S}, &routine::setAreaTransitionBMP);
    routines.add("ActionStartConversation", VT_V, {VT_O, VT_S, VT_I, VT_I, VT_I, VT_S, VT_S, VT_S, VT_S, VT_S, VT_S, VT_I}, &routine::actionStartConversation);
    routines.add("ActionPauseConversation", VT_V, {}, &routine::actionPauseConversation);
    routines.add("ActionResumeConversation", VT_V, {}, &routine::actionResumeConversation);
    routines.add("EffectBeam", VT_EFFECT, {VT_I, VT_O, VT_I, VT_I}, &routine::effectBeam);
    routines.add("GetReputation", VT_I, {VT_O, VT_O}, &routine::getReputation);
    routines.add("AdjustReputation", VT_V, {VT_O, VT_O, VT_I}, &routine::adjustReputation);
    routines.add("GetModuleFileName", VT_S, {}, &routine::getModuleFileName);
    routines.add("GetGoingToBeAttackedBy", VT_O, {VT_O}, &routine::getGoingToBeAttackedBy);
    routines.add("EffectForceResistanceIncrease", VT_EFFECT, {VT_I}, &routine::effectForceResistanceIncrease);
    routines.add("GetLocation", VT_LOCATION, {VT_O}, &routine::getLocation);
    routines.add("ActionJumpToLocation", VT_V, {VT_LOCATION}, &routine::actionJumpToLocation);
    routines.add("Location", VT_LOCATION, {VT_VECTOR, VT_F}, &routine::location);
    routines.add("ApplyEffectAtLocation", VT_V, {VT_I, VT_EFFECT, VT_LOCATION, VT_F}, &routine::applyEffectAtLocation);
    routines.add("GetIsPC", VT_I, {VT_O}, &routine::getIsPC);
    routines.add("FeetToMeters", VT_F, {VT_F}, &routine::feetToMeters);
    routines.add("YardsToMeters", VT_F, {VT_F}, &routine::yardsToMeters);
    routines.add("ApplyEffectToObject", VT_V, {VT_I, VT_EFFECT, VT_O, VT_F}, &routine::applyEffectToObject);
    routines.add("SpeakString", VT_V, {VT_S, VT_I}, &routine::speakString);
    routines.add("GetSpellTargetLocation", VT_LOCATION, {}, &routine::getSpellTargetLocation);
    routines.add("GetPositionFromLocation", VT_VECTOR, {VT_LOCATION}, &routine::getPositionFromLocation);
    routines.add("EffectBodyFuel", VT_EFFECT, {}, &routine::effectBodyFuel);
    routines.add("GetFacingFromLocation", VT_F, {VT_LOCATION}, &routine::getFacingFromLocation);
    routines.add("GetNearestCreatureToLocation", VT_O, {VT_I, VT_I, VT_LOCATION, VT_I, VT_I, VT_I, VT_I, VT_I}, &routine::getNearestCreatureToLocation);
    routines.add("GetNearestObject", VT_O, {VT_I, VT_O, VT_I}, &routine::getNearestObject);
    routines.add("GetNearestObjectToLocation", VT_O, {VT_I, VT_LOCATION, VT_I}, &routine::getNearestObjectToLocation);
    routines.add("GetNearestObjectByTag", VT_O, {VT_S, VT_O, VT_I}, &routine::getNearestObjectByTag);
    routines.add("IntToFloat", VT_F, {VT_I}, &routine::intToFloat);
    routines.add("FloatToInt", VT_I, {VT_F}, &routine::floatToInt);
    routines.add("StringToInt", VT_I, {VT_S}, &routine::stringToInt);
    routines.add("StringToFloat", VT_F, {VT_S}, &routine::stringToFloat);
    routines.add("ActionCastSpellAtLocation", VT_V, {VT_I, VT_LOCATION, VT_I, VT_I, VT_I, VT_I}, &routine::actionCastSpellAtLocation);
    routines.add("GetIsEnemy", VT_I, {VT_O, VT_O}, &routine::getIsEnemy);
    routines.add("GetIsFriend", VT_I, {VT_O, VT_O}, &routine::getIsFriend);
    routines.add("GetIsNeutral", VT_I, {VT_O, VT_O}, &routine::getIsNeutral);
    routines.add("GetPCSpeaker", VT_O, {}, &routine::getPCSpeaker);
    routines.add("GetStringByStrRef", VT_S, {VT_I}, &routine::getStringByStrRef);
    routines.add("ActionSpeakStringByStrRef", VT_V, {VT_I, VT_I}, &routine::actionSpeakStringByStrRef);
    routines.add("DestroyObject", VT_V, {VT_O, VT_F, VT_I, VT_F}, &routine::destroyObject);
    routines.add("GetModule", VT_O, {}, &routine::getModule);
    routines.add("CreateObject", VT_O, {VT_I, VT_S, VT_LOCATION, VT_I}, &routine::createObject);
    routines.add("EventSpellCastAt", VT_EVENT, {VT_O, VT_I, VT_I}, &routine::eventSpellCastAt);
    routines.add("GetLastSpellCaster", VT_O, {}, &routine::getLastSpellCaster);
    routines.add("GetLastSpell", VT_I, {}, &routine::getLastSpell);
    routines.add("GetUserDefinedEventNumber", VT_I, {}, &routine::getUserDefinedEventNumber);
    routines.add("GetSpellId", VT_I, {}, &routine::getSpellId);
    routines.add("RandomName", VT_S, {}, &routine::randomName);
    routines.add("EffectPoison", VT_EFFECT, {VT_I}, &routine::effectPoison);
    routines.add("GetLoadFromSaveGame", VT_I, {}, &routine::getLoadFromSaveGame);
    routines.add("EffectAssuredDeflection", VT_EFFECT, {VT_I}, &routine::effectAssuredDeflection);
    routines.add("GetName", VT_S, {VT_O}, &routine::getName);
    routines.add("GetLastSpeaker", VT_O, {}, &routine::getLastSpeaker);
    routines.add("BeginConversation", VT_I, {VT_S, VT_O}, &routine::beginConversation);
    routines.add("GetLastPerceived", VT_O, {}, &routine::getLastPerceived);
    routines.add("GetLastPerceptionHeard", VT_I, {}, &routine::getLastPerceptionHeard);
    routines.add("GetLastPerceptionInaudible", VT_I, {}, &routine::getLastPerceptionInaudible);
    routines.add("GetLastPerceptionSeen", VT_I, {}, &routine::getLastPerceptionSeen);
    routines.add("GetLastClosedBy", VT_O, {}, &routine::getLastClosedBy);
    routines.add("GetLastPerceptionVanished", VT_I, {}, &routine::getLastPerceptionVanished);
    routines.add("GetFirstInPersistentObject", VT_O, {VT_O, VT_I, VT_I}, &routine::getFirstInPersistentObject);
    routines.add("GetNextInPersistentObject", VT_O, {VT_O, VT_I, VT_I}, &routine::getNextInPersistentObject);
    routines.add("GetAreaOfEffectCreator", VT_O, {VT_O}, &routine::getAreaOfEffectCreator);
    routines.add("ShowLevelUpGUI", VT_I, {}, &routine::showLevelUpGUI);
    routines.add("SetItemNonEquippable", VT_V, {VT_O, VT_I}, &routine::setItemNonEquippable);
    routines.add("GetButtonMashCheck", VT_I, {}, &routine::getButtonMashCheck);
    routines.add("SetButtonMashCheck", VT_V, {VT_I}, &routine::setButtonMashCheck);
    routines.add("EffectForcePushTargeted", VT_EFFECT, {VT_LOCATION, VT_I}, &routine::effectForcePushTargeted);
    routines.add("EffectHaste", VT_EFFECT, {}, &routine::effectHaste);
    routines.add("GiveItem", VT_V, {VT_O, VT_O}, &routine::giveItem);
    routines.add("ObjectToString", VT_S, {VT_O}, &routine::objectToString);
    routines.add("EffectImmunity", VT_EFFECT, {VT_I}, &routine::effectImmunity);
    routines.add("GetIsImmune", VT_I, {VT_O, VT_I, VT_O}, &routine::getIsImmune);
    routines.add("EffectDamageImmunityIncrease", VT_EFFECT, {VT_I, VT_I}, &routine::effectDamageImmunityIncrease);
    routines.add("GetEncounterActive", VT_I, {VT_O}, &routine::getEncounterActive);
    routines.add("SetEncounterActive", VT_V, {VT_I, VT_O}, &routine::setEncounterActive);
    routines.add("GetEncounterSpawnsMax", VT_I, {VT_O}, &routine::getEncounterSpawnsMax);
    routines.add("SetEncounterSpawnsMax", VT_V, {VT_I, VT_O}, &routine::setEncounterSpawnsMax);
    routines.add("GetEncounterSpawnsCurrent", VT_I, {VT_O}, &routine::getEncounterSpawnsCurrent);
    routines.add("SetEncounterSpawnsCurrent", VT_V, {VT_I, VT_O}, &routine::setEncounterSpawnsCurrent);
    routines.add("GetModuleItemAcquired", VT_O, {}, &routine::getModuleItemAcquired);
    routines.add("GetModuleItemAcquiredFrom", VT_O, {}, &routine::getModuleItemAcquiredFrom);
    routines.add("SetCustomToken", VT_V, {VT_I, VT_S}, &routine::setCustomToken);
    routines.add("GetHasFeat", VT_I, {VT_I, VT_O}, &routine::getHasFeat);
    routines.add("GetHasSkill", VT_I, {VT_I, VT_O}, &routine::getHasSkill);
    routines.add("ActionUseFeat", VT_V, {VT_I, VT_O}, &routine::actionUseFeat);
    routines.add("ActionUseSkill", VT_V, {VT_I, VT_O, VT_I, VT_O}, &routine::actionUseSkill);
    routines.add("GetObjectSeen", VT_I, {VT_O, VT_O}, &routine::getObjectSeen);
    routines.add("GetObjectHeard", VT_I, {VT_O, VT_O}, &routine::getObjectHeard);
    routines.add("GetLastPlayerDied", VT_O, {}, &routine::getLastPlayerDied);
    routines.add("GetModuleItemLost", VT_O, {}, &routine::getModuleItemLost);
    routines.add("GetModuleItemLostBy", VT_O, {}, &routine::getModuleItemLostBy);
    routines.add("ActionDoCommand", VT_V, {VT_ACTION}, &routine::actionDoCommand);
    routines.add("EventConversation", VT_EVENT, {}, &routine::eventConversation);
    routines.add("SetEncounterDifficulty", VT_V, {VT_I, VT_O}, &routine::setEncounterDifficulty);
    routines.add("GetEncounterDifficulty", VT_I, {VT_O}, &routine::getEncounterDifficulty);
    routines.add("GetDistanceBetweenLocations", VT_F, {VT_LOCATION, VT_LOCATION}, &routine::getDistanceBetweenLocations);
    routines.add("GetReflexAdjustedDamage", VT_I, {VT_I, VT_O, VT_I, VT_I, VT_O}, &routine::getReflexAdjustedDamage);
    routines.add("PlayAnimation", VT_V, {VT_I, VT_F, VT_F}, &routine::playAnimation);
    routines.add("TalentSpell", VT_TALENT, {VT_I}, &routine::talentSpell);
    routines.add("TalentFeat", VT_TALENT, {VT_I}, &routine::talentFeat);
    routines.add("TalentSkill", VT_TALENT, {VT_I}, &routine::talentSkill);
    routines.add("GetHasSpellEffect", VT_I, {VT_I, VT_O}, &routine::getHasSpellEffect);
    routines.add("GetEffectSpellId", VT_I, {VT_EFFECT}, &routine::getEffectSpellId);
    routines.add("GetCreatureHasTalent", VT_I, {VT_TALENT, VT_O}, &routine::getCreatureHasTalent);
    routines.add("GetCreatureTalentRandom", VT_TALENT, {VT_I, VT_O, VT_I}, &routine::getCreatureTalentRandom);
    routines.add("GetCreatureTalentBest", VT_TALENT, {VT_I, VT_I, VT_O, VT_I, VT_I, VT_I}, &routine::getCreatureTalentBest);
    routines.add("ActionUseTalentOnObject", VT_V, {VT_TALENT, VT_O}, &routine::actionUseTalentOnObject);
    routines.add("ActionUseTalentAtLocation", VT_V, {VT_TALENT, VT_LOCATION}, &routine::actionUseTalentAtLocation);
    routines.add("GetGoldPieceValue", VT_I, {VT_O}, &routine::getGoldPieceValue);
    routines.add("GetIsPlayableRacialType", VT_I, {VT_O}, &routine::getIsPlayableRacialType);
    routines.add("JumpToLocation", VT_V, {VT_LOCATION}, &routine::jumpToLocation);
    routines.add("EffectTemporaryHitpoints", VT_EFFECT, {VT_I}, &routine::effectTemporaryHitpoints);
    routines.add("GetSkillRank", VT_I, {VT_I, VT_O}, &routine::getSkillRank);
    routines.add("GetAttackTarget", VT_O, {VT_O}, &routine::getAttackTarget);
    routines.add("GetLastAttackType", VT_I, {VT_O}, &routine::getLastAttackType);
    routines.add("GetLastAttackMode", VT_I, {VT_O}, &routine::getLastAttackMode);
    routines.add("GetDistanceBetween2D", VT_F, {VT_O, VT_O}, &routine::getDistanceBetween2D);
    routines.add("GetIsInCombat", VT_I, {VT_O}, &routine::getIsInCombat);
    routines.add("GetLastAssociateCommand", VT_I, {VT_O}, &routine::getLastAssociateCommand);
    routines.add("GiveGoldToCreature", VT_V, {VT_O, VT_I}, &routine::giveGoldToCreature);
    routines.add("SetIsDestroyable", VT_V, {VT_I, VT_I, VT_I}, &routine::setIsDestroyable);
    routines.add("SetLocked", VT_V, {VT_O, VT_I}, &routine::setLocked);
    routines.add("GetLocked", VT_I, {VT_O}, &routine::getLocked);
    routines.add("GetClickingObject", VT_O, {}, &routine::getClickingObject);
    routines.add("SetAssociateListenPatterns", VT_V, {VT_O}, &routine::setAssociateListenPatterns);
    routines.add("GetLastWeaponUsed", VT_O, {VT_O}, &routine::getLastWeaponUsed);
    routines.add("ActionInteractObject", VT_V, {VT_O}, &routine::actionInteractObject);
    routines.add("GetLastUsedBy", VT_O, {}, &routine::getLastUsedBy);
    routines.add("GetAbilityModifier", VT_I, {VT_I, VT_O}, &routine::getAbilityModifier);
    routines.add("GetIdentified", VT_I, {VT_O}, &routine::getIdentified);
    routines.add("SetIdentified", VT_V, {VT_O, VT_I}, &routine::setIdentified);
    routines.add("GetDistanceBetweenLocations2D", VT_F, {VT_LOCATION, VT_LOCATION}, &routine::getDistanceBetweenLocations2D);
    routines.add("GetDistanceToObject2D", VT_F, {VT_O}, &routine::getDistanceToObject2D);
    routines.add("GetBlockingDoor", VT_O, {}, &routine::getBlockingDoor);
    routines.add("GetIsDoorActionPossible", VT_I, {VT_O, VT_I}, &routine::getIsDoorActionPossible);
    routines.add("DoDoorAction", VT_V, {VT_O, VT_I}, &routine::doDoorAction);
    routines.add("GetFirstItemInInventory", VT_O, {VT_O}, &routine::getFirstItemInInventory);
    routines.add("GetNextItemInInventory", VT_O, {VT_O}, &routine::getNextItemInInventory);
    routines.add("GetClassByPosition", VT_I, {VT_I, VT_O}, &routine::getClassByPosition);
    routines.add("GetLevelByPosition", VT_I, {VT_I, VT_O}, &routine::getLevelByPosition);
    routines.add("GetLevelByClass", VT_I, {VT_I, VT_O}, &routine::getLevelByClass);
    routines.add("GetDamageDealtByType", VT_I, {VT_I}, &routine::getDamageDealtByType);
    routines.add("GetTotalDamageDealt", VT_I, {}, &routine::getTotalDamageDealt);
    routines.add("GetLastDamager", VT_O, {}, &routine::getLastDamager);
    routines.add("GetLastDisarmed", VT_O, {}, &routine::getLastDisarmed);
    routines.add("GetLastDisturbed", VT_O, {}, &routine::getLastDisturbed);
    routines.add("GetLastLocked", VT_O, {}, &routine::getLastLocked);
    routines.add("GetLastUnlocked", VT_O, {}, &routine::getLastUnlocked);
    routines.add("EffectSkillIncrease", VT_EFFECT, {VT_I, VT_I}, &routine::effectSkillIncrease);
    routines.add("GetInventoryDisturbType", VT_I, {}, &routine::getInventoryDisturbType);
    routines.add("GetInventoryDisturbItem", VT_O, {}, &routine::getInventoryDisturbItem);
    routines.add("ShowUpgradeScreen", VT_V, {VT_O}, &routine::showUpgradeScreen);
    routines.add("VersusAlignmentEffect", VT_EFFECT, {VT_EFFECT, VT_I, VT_I}, &routine::versusAlignmentEffect);
    routines.add("VersusRacialTypeEffect", VT_EFFECT, {VT_EFFECT, VT_I}, &routine::versusRacialTypeEffect);
    routines.add("VersusTrapEffect", VT_EFFECT, {VT_EFFECT}, &routine::versusTrapEffect);
    routines.add("GetGender", VT_I, {VT_O}, &routine::getGender);
    routines.add("GetIsTalentValid", VT_I, {VT_TALENT}, &routine::getIsTalentValid);
    routines.add("ActionMoveAwayFromLocation", VT_V, {VT_LOCATION, VT_I, VT_F}, &routine::actionMoveAwayFromLocation);
    routines.add("GetAttemptedAttackTarget", VT_O, {}, &routine::getAttemptedAttackTarget);
    routines.add("GetTypeFromTalent", VT_I, {VT_TALENT}, &routine::getTypeFromTalent);
    routines.add("GetIdFromTalent", VT_I, {VT_TALENT}, &routine::getIdFromTalent);
    routines.add("PlayPazaak", VT_V, {VT_I, VT_S, VT_I, VT_I, VT_O}, &routine::playPazaak);
    routines.add("GetLastPazaakResult", VT_I, {}, &routine::getLastPazaakResult);
    routines.add("DisplayFeedBackText", VT_V, {VT_O, VT_I}, &routine::displayFeedBackText);
    routines.add("AddJournalQuestEntry", VT_V, {VT_S, VT_I, VT_I}, &routine::addJournalQuestEntry);
    routines.add("RemoveJournalQuestEntry", VT_V, {VT_S}, &routine::removeJournalQuestEntry);
    routines.add("GetJournalEntry", VT_I, {VT_S}, &routine::getJournalEntry);
    routines.add("PlayRumblePattern", VT_I, {VT_I}, &routine::playRumblePattern);
    routines.add("StopRumblePattern", VT_I, {VT_I}, &routine::stopRumblePattern);
    routines.add("EffectDamageForcePoints", VT_EFFECT, {VT_I}, &routine::effectDamageForcePoints);
    routines.add("EffectHealForcePoints", VT_EFFECT, {VT_I}, &routine::effectHealForcePoints);
    routines.add("SendMessageToPC", VT_V, {VT_O, VT_S}, &routine::sendMessageToPC);
    routines.add("GetAttemptedSpellTarget", VT_O, {}, &routine::getAttemptedSpellTarget);
    routines.add("GetLastOpenedBy", VT_O, {}, &routine::getLastOpenedBy);
    routines.add("GetHasSpell", VT_I, {VT_I, VT_O}, &routine::getHasSpell);
    routines.add("OpenStore", VT_V, {VT_O, VT_O, VT_I, VT_I}, &routine::openStore);
    routines.add("ActionSurrenderToEnemies", VT_V, {}, &routine::actionSurrenderToEnemies);
    routines.add("GetFirstFactionMember", VT_O, {VT_O, VT_I}, &routine::getFirstFactionMember);
    routines.add("GetNextFactionMember", VT_O, {VT_O, VT_I}, &routine::getNextFactionMember);
    routines.add("ActionForceMoveToLocation", VT_V, {VT_LOCATION, VT_I, VT_F}, &routine::actionForceMoveToLocation);
    routines.add("ActionForceMoveToObject", VT_V, {VT_O, VT_I, VT_F, VT_F}, &routine::actionForceMoveToObject);
    routines.add("GetJournalQuestExperience", VT_I, {VT_S}, &routine::getJournalQuestExperience);
    routines.add("JumpToObject", VT_V, {VT_O, VT_I}, &routine::jumpToObject);
    routines.add("SetMapPinEnabled", VT_V, {VT_O, VT_I}, &routine::setMapPinEnabled);
    routines.add("EffectHitPointChangeWhenDying", VT_EFFECT, {VT_F}, &routine::effectHitPointChangeWhenDying);
    routines.add("PopUpGUIPanel", VT_V, {VT_O, VT_I}, &routine::popUpGUIPanel);
    routines.add("AddMultiClass", VT_V, {VT_I, VT_O}, &routine::addMultiClass);
    routines.add("GetIsLinkImmune", VT_I, {VT_O, VT_EFFECT}, &routine::getIsLinkImmune);
    routines.add("EffectDroidStun", VT_EFFECT, {}, &routine::effectDroidStun);
    routines.add("EffectForcePushed", VT_EFFECT, {}, &routine::effectForcePushed);
    routines.add("GiveXPToCreature", VT_V, {VT_O, VT_I}, &routine::giveXPToCreature);
    routines.add("SetXP", VT_V, {VT_O, VT_I}, &routine::setXP);
    routines.add("GetXP", VT_I, {VT_O}, &routine::getXP);
    routines.add("IntToHexString", VT_S, {VT_I}, &routine::intToHexString);
    routines.add("GetBaseItemType", VT_I, {VT_O}, &routine::getBaseItemType);
    routines.add("GetItemHasItemProperty", VT_I, {VT_O, VT_I}, &routine::getItemHasItemProperty);
    routines.add("ActionEquipMostDamagingMelee", VT_V, {VT_O, VT_I}, &routine::actionEquipMostDamagingMelee);
    routines.add("ActionEquipMostDamagingRanged", VT_V, {VT_O}, &routine::actionEquipMostDamagingRanged);
    routines.add("GetItemACValue", VT_I, {VT_O}, &routine::getItemACValue);
    routines.add("EffectForceResisted", VT_EFFECT, {VT_O}, &routine::effectForceResisted);
    routines.add("ExploreAreaForPlayer", VT_V, {VT_O, VT_O}, &routine::exploreAreaForPlayer);
    routines.add("ActionEquipMostEffectiveArmor", VT_V, {}, &routine::actionEquipMostEffectiveArmor);
    routines.add("GetIsDay", VT_I, {}, &routine::getIsDay);
    routines.add("GetIsNight", VT_I, {}, &routine::getIsNight);
    routines.add("GetIsDawn", VT_I, {}, &routine::getIsDawn);
    routines.add("GetIsDusk", VT_I, {}, &routine::getIsDusk);
    routines.add("GetIsEncounterCreature", VT_I, {VT_O}, &routine::getIsEncounterCreature);
    routines.add("GetLastPlayerDying", VT_O, {}, &routine::getLastPlayerDying);
    routines.add("GetStartingLocation", VT_LOCATION, {}, &routine::getStartingLocation);
    routines.add("ChangeToStandardFaction", VT_V, {VT_O, VT_I}, &routine::changeToStandardFaction);
    routines.add("SoundObjectPlay", VT_V, {VT_O}, &routine::soundObjectPlay);
    routines.add("SoundObjectStop", VT_V, {VT_O}, &routine::soundObjectStop);
    routines.add("SoundObjectSetVolume", VT_V, {VT_O, VT_I}, &routine::soundObjectSetVolume);
    routines.add("SoundObjectSetPosition", VT_V, {VT_O, VT_VECTOR}, &routine::soundObjectSetPosition);
    routines.add("SpeakOneLinerConversation", VT_V, {VT_S, VT_O}, &routine::speakOneLinerConversation);
    routines.add("GetGold", VT_I, {VT_O}, &routine::getGold);
    routines.add("GetLastRespawnButtonPresser", VT_O, {}, &routine::getLastRespawnButtonPresser);
    routines.add("EffectForceFizzle", VT_EFFECT, {}, &routine::effectForceFizzle);
    routines.add("SetLightsaberPowered", VT_V, {VT_O, VT_I, VT_I, VT_I}, &routine::setLightsaberPowered);
    routines.add("GetIsWeaponEffective", VT_I, {VT_O, VT_I}, &routine::getIsWeaponEffective);
    routines.add("GetLastSpellHarmful", VT_I, {}, &routine::getLastSpellHarmful);
    routines.add("EventActivateItem", VT_EVENT, {VT_O, VT_LOCATION, VT_O}, &routine::eventActivateItem);
    routines.add("MusicBackgroundPlay", VT_V, {VT_O}, &routine::musicBackgroundPlay);
    routines.add("MusicBackgroundStop", VT_V, {VT_O}, &routine::musicBackgroundStop);
    routines.add("MusicBackgroundSetDelay", VT_V, {VT_O, VT_I}, &routine::musicBackgroundSetDelay);
    routines.add("MusicBackgroundChangeDay", VT_V, {VT_O, VT_I}, &routine::musicBackgroundChangeDay);
    routines.add("MusicBackgroundChangeNight", VT_V, {VT_O, VT_I}, &routine::musicBackgroundChangeNight);
    routines.add("MusicBattlePlay", VT_V, {VT_O}, &routine::musicBattlePlay);
    routines.add("MusicBattleStop", VT_V, {VT_O}, &routine::musicBattleStop);
    routines.add("MusicBattleChange", VT_V, {VT_O, VT_I}, &routine::musicBattleChange);
    routines.add("AmbientSoundPlay", VT_V, {VT_O}, &routine::ambientSoundPlay);
    routines.add("AmbientSoundStop", VT_V, {VT_O}, &routine::ambientSoundStop);
    routines.add("AmbientSoundChangeDay", VT_V, {VT_O, VT_I}, &routine::ambientSoundChangeDay);
    routines.add("AmbientSoundChangeNight", VT_V, {VT_O, VT_I}, &routine::ambientSoundChangeNight);
    routines.add("GetLastKiller", VT_O, {}, &routine::getLastKiller);
    routines.add("GetSpellCastItem", VT_O, {}, &routine::getSpellCastItem);
    routines.add("GetItemActivated", VT_O, {}, &routine::getItemActivated);
    routines.add("GetItemActivator", VT_O, {}, &routine::getItemActivator);
    routines.add("GetItemActivatedTargetLocation", VT_LOCATION, {}, &routine::getItemActivatedTargetLocation);
    routines.add("GetItemActivatedTarget", VT_O, {}, &routine::getItemActivatedTarget);
    routines.add("GetIsOpen", VT_I, {VT_O}, &routine::getIsOpen);
    routines.add("TakeGoldFromCreature", VT_V, {VT_I, VT_O, VT_I}, &routine::takeGoldFromCreature);
    routines.add("GetIsInConversation", VT_I, {VT_O}, &routine::getIsInConversation);
    routines.add("EffectAbilityDecrease", VT_EFFECT, {VT_I, VT_I}, &routine::effectAbilityDecrease);
    routines.add("EffectAttackDecrease", VT_EFFECT, {VT_I, VT_I}, &routine::effectAttackDecrease);
    routines.add("EffectDamageDecrease", VT_EFFECT, {VT_I, VT_I}, &routine::effectDamageDecrease);
    routines.add("EffectDamageImmunityDecrease", VT_EFFECT, {VT_I, VT_I}, &routine::effectDamageImmunityDecrease);
    routines.add("EffectACDecrease", VT_EFFECT, {VT_I, VT_I, VT_I}, &routine::effectACDecrease);
    routines.add("EffectMovementSpeedDecrease", VT_EFFECT, {VT_I}, &routine::effectMovementSpeedDecrease);
    routines.add("EffectSavingThrowDecrease", VT_EFFECT, {VT_I, VT_I, VT_I}, &routine::effectSavingThrowDecrease);
    routines.add("EffectSkillDecrease", VT_EFFECT, {VT_I, VT_I}, &routine::effectSkillDecrease);
    routines.add("EffectForceResistanceDecrease", VT_EFFECT, {VT_I}, &routine::effectForceResistanceDecrease);
    routines.add("GetPlotFlag", VT_I, {VT_O}, &routine::getPlotFlag);
    routines.add("SetPlotFlag", VT_V, {VT_O, VT_I}, &routine::setPlotFlag);
    routines.add("EffectInvisibility", VT_EFFECT, {VT_I}, &routine::effectInvisibility);
    routines.add("EffectConcealment", VT_EFFECT, {VT_I}, &routine::effectConcealment);
    routines.add("EffectForceShield", VT_EFFECT, {VT_I}, &routine::effectForceShield);
    routines.add("EffectDispelMagicAll", VT_EFFECT, {VT_I}, &routine::effectDispelMagicAll);
    routines.add("SetDialogPlaceableCamera", VT_V, {VT_I}, &routine::setDialogPlaceableCamera);
    routines.add("GetSoloMode", VT_I, {}, &routine::getSoloMode);
    routines.add("EffectDisguise", VT_EFFECT, {VT_I}, &routine::effectDisguise);
    routines.add("GetMaxStealthXP", VT_I, {}, &routine::getMaxStealthXP);
    routines.add("EffectTrueSeeing", VT_EFFECT, {}, &routine::effectTrueSeeing);
    routines.add("EffectSeeInvisible", VT_EFFECT, {}, &routine::effectSeeInvisible);
    routines.add("EffectTimeStop", VT_EFFECT, {}, &routine::effectTimeStop);
    routines.add("SetMaxStealthXP", VT_V, {VT_I}, &routine::setMaxStealthXP);
    routines.add("EffectBlasterDeflectionIncrease", VT_EFFECT, {VT_I}, &routine::effectBlasterDeflectionIncrease);
    routines.add("EffectBlasterDeflectionDecrease", VT_EFFECT, {VT_I}, &routine::effectBlasterDeflectionDecrease);
    routines.add("EffectHorrified", VT_EFFECT, {}, &routine::effectHorrified);
    routines.add("EffectSpellLevelAbsorption", VT_EFFECT, {VT_I, VT_I, VT_I}, &routine::effectSpellLevelAbsorption);
    routines.add("EffectDispelMagicBest", VT_EFFECT, {VT_I}, &routine::effectDispelMagicBest);
    routines.add("GetCurrentStealthXP", VT_I, {}, &routine::getCurrentStealthXP);
    routines.add("GetNumStackedItems", VT_I, {VT_O}, &routine::getNumStackedItems);
    routines.add("SurrenderToEnemies", VT_V, {}, &routine::surrenderToEnemies);
    routines.add("EffectMissChance", VT_EFFECT, {VT_I}, &routine::effectMissChance);
    routines.add("SetCurrentStealthXP", VT_V, {VT_I}, &routine::setCurrentStealthXP);
    routines.add("GetCreatureSize", VT_I, {VT_O}, &routine::getCreatureSize);
    routines.add("AwardStealthXP", VT_V, {VT_O}, &routine::awardStealthXP);
    routines.add("GetStealthXPEnabled", VT_I, {}, &routine::getStealthXPEnabled);
    routines.add("SetStealthXPEnabled", VT_V, {VT_I}, &routine::setStealthXPEnabled);
    routines.add("ActionUnlockObject", VT_V, {VT_O}, &routine::actionUnlockObject);
    routines.add("ActionLockObject", VT_V, {VT_O}, &routine::actionLockObject);
    routines.add("EffectModifyAttacks", VT_EFFECT, {VT_I}, &routine::effectModifyAttacks);
    routines.add("GetLastTrapDetected", VT_O, {VT_O}, &routine::getLastTrapDetected);
    routines.add("EffectDamageShield", VT_EFFECT, {VT_I, VT_I, VT_I}, &routine::effectDamageShield);
    routines.add("GetNearestTrapToObject", VT_O, {VT_O, VT_I}, &routine::getNearestTrapToObject);
    routines.add("GetAttemptedMovementTarget", VT_O, {}, &routine::getAttemptedMovementTarget);
    routines.add("GetBlockingCreature", VT_O, {VT_O}, &routine::getBlockingCreature);
    routines.add("GetFortitudeSavingThrow", VT_I, {VT_O}, &routine::getFortitudeSavingThrow);
    routines.add("GetWillSavingThrow", VT_I, {VT_O}, &routine::getWillSavingThrow);
    routines.add("GetReflexSavingThrow", VT_I, {VT_O}, &routine::getReflexSavingThrow);
    routines.add("GetChallengeRating", VT_F, {VT_O}, &routine::getChallengeRating);
    routines.add("GetFoundEnemyCreature", VT_O, {VT_O}, &routine::getFoundEnemyCreature);
    routines.add("GetMovementRate", VT_I, {VT_O}, &routine::getMovementRate);
    routines.add("GetSubRace", VT_I, {VT_O}, &routine::getSubRace);
    routines.add("GetStealthXPDecrement", VT_I, {}, &routine::getStealthXPDecrement);
    routines.add("SetStealthXPDecrement", VT_V, {VT_I}, &routine::setStealthXPDecrement);
    routines.add("DuplicateHeadAppearance", VT_V, {VT_O, VT_O}, &routine::duplicateHeadAppearance);
    routines.add("ActionCastFakeSpellAtObject", VT_V, {VT_I, VT_O, VT_I}, &routine::actionCastFakeSpellAtObject);
    routines.add("ActionCastFakeSpellAtLocation", VT_V, {VT_I, VT_LOCATION, VT_I}, &routine::actionCastFakeSpellAtLocation);
    routines.add("CutsceneAttack", VT_V, {VT_O, VT_I, VT_I, VT_I}, &routine::cutsceneAttack);
    routines.add("SetCameraMode", VT_V, {VT_O, VT_I}, &routine::setCameraMode);
    routines.add("SetLockOrientationInDialog", VT_V, {VT_O, VT_I}, &routine::setLockOrientationInDialog);
    routines.add("SetLockHeadFollowInDialog", VT_V, {VT_O, VT_I}, &routine::setLockHeadFollowInDialog);
    routines.add("CutsceneMove", VT_V, {VT_O, VT_VECTOR, VT_I}, &routine::cutsceneMove);
    routines.add("EnableVideoEffect", VT_V, {VT_I}, &routine::enableVideoEffect);
    routines.add("StartNewModule", VT_V, {VT_S, VT_S, VT_S, VT_S, VT_S, VT_S, VT_S, VT_S}, &routine::startNewModule);
    routines.add("DisableVideoEffect", VT_V, {}, &routine::disableVideoEffect);
    routines.add("GetWeaponRanged", VT_I, {VT_O}, &routine::getWeaponRanged);
    routines.add("DoSinglePlayerAutoSave", VT_V, {}, &routine::doSinglePlayerAutoSave);
    routines.add("GetGameDifficulty", VT_I, {}, &routine::getGameDifficulty);
    routines.add("GetUserActionsPending", VT_I, {}, &routine::getUserActionsPending);
    routines.add("RevealMap", VT_V, {VT_VECTOR, VT_I}, &routine::revealMap);
    routines.add("SetTutorialWindowsEnabled", VT_V, {VT_I}, &routine::setTutorialWindowsEnabled);
    routines.add("ShowTutorialWindow", VT_V, {VT_I}, &routine::showTutorialWindow);
    routines.add("StartCreditSequence", VT_V, {VT_I}, &routine::startCreditSequence);
    routines.add("IsCreditSequenceInProgress", VT_I, {}, &routine::isCreditSequenceInProgress);

    routines.add("SWMG_SetLateralAccelerationPerSecond", VT_V, {VT_F}, &routine::minigame::setLateralAccelerationPerSecond);
    routines.add("SWMG_GetLateralAccelerationPerSecond", VT_F, {}, &routine::minigame::getLateralAccelerationPerSecond);

    routines.add("GetCurrentAction", VT_I, {VT_O}, &routine::getCurrentAction);
    routines.add("GetDifficultyModifier", VT_F, {}, &routine::getDifficultyModifier);
    routines.add("GetAppearanceType", VT_I, {VT_O}, &routine::getAppearanceType);
    routines.add("FloatingTextStrRefOnCreature", VT_V, {VT_I, VT_O, VT_I}, &routine::floatingTextStrRefOnCreature);
    routines.add("FloatingTextStringOnCreature", VT_V, {VT_S, VT_O, VT_I}, &routine::floatingTextStringOnCreature);
    routines.add("GetTrapDisarmable", VT_I, {VT_O}, &routine::getTrapDisarmable);
    routines.add("GetTrapDetectable", VT_I, {VT_O}, &routine::getTrapDetectable);
    routines.add("GetTrapDetectedBy", VT_I, {VT_O, VT_O}, &routine::getTrapDetectedBy);
    routines.add("GetTrapFlagged", VT_I, {VT_O}, &routine::getTrapFlagged);
    routines.add("GetTrapBaseType", VT_I, {VT_O}, &routine::getTrapBaseType);
    routines.add("GetTrapOneShot", VT_I, {VT_O}, &routine::getTrapOneShot);
    routines.add("GetTrapCreator", VT_O, {VT_O}, &routine::getTrapCreator);
    routines.add("GetTrapKeyTag", VT_S, {VT_O}, &routine::getTrapKeyTag);
    routines.add("GetTrapDisarmDC", VT_I, {VT_O}, &routine::getTrapDisarmDC);
    routines.add("GetTrapDetectDC", VT_I, {VT_O}, &routine::getTrapDetectDC);
    routines.add("GetLockKeyRequired", VT_I, {VT_O}, &routine::getLockKeyRequired);
    routines.add("GetLockKeyTag", VT_I, {VT_O}, &routine::getLockKeyTag);
    routines.add("GetLockLockable", VT_I, {VT_O}, &routine::getLockLockable);
    routines.add("GetLockUnlockDC", VT_I, {VT_O}, &routine::getLockUnlockDC);
    routines.add("GetLockLockDC", VT_I, {VT_O}, &routine::getLockLockDC);
    routines.add("GetPCLevellingUp", VT_O, {}, &routine::getPCLevellingUp);
    routines.add("GetHasFeatEffect", VT_I, {VT_I, VT_O}, &routine::getHasFeatEffect);
    routines.add("SetPlaceableIllumination", VT_V, {VT_O, VT_I}, &routine::setPlaceableIllumination);
    routines.add("GetPlaceableIllumination", VT_I, {VT_O}, &routine::getPlaceableIllumination);
    routines.add("GetIsPlaceableObjectActionPossible", VT_I, {VT_O, VT_I}, &routine::getIsPlaceableObjectActionPossible);
    routines.add("DoPlaceableObjectAction", VT_V, {VT_O, VT_I}, &routine::doPlaceableObjectAction);
    routines.add("GetFirstPC", VT_O, {}, &routine::getFirstPC);
    routines.add("GetNextPC", VT_O, {}, &routine::getNextPC);
    routines.add("SetTrapDetectedBy", VT_I, {VT_O, VT_O}, &routine::setTrapDetectedBy);
    routines.add("GetIsTrapped", VT_I, {VT_O}, &routine::getIsTrapped);
    routines.add("SetEffectIcon", VT_EFFECT, {VT_EFFECT, VT_I}, &routine::setEffectIcon);
    routines.add("FaceObjectAwayFromObject", VT_V, {VT_O, VT_O}, &routine::faceObjectAwayFromObject);
    routines.add("PopUpDeathGUIPanel", VT_V, {VT_O, VT_I, VT_I, VT_I, VT_S}, &routine::popUpDeathGUIPanel);
    routines.add("SetTrapDisabled", VT_V, {VT_O}, &routine::setTrapDisabled);
    routines.add("GetLastHostileActor", VT_O, {VT_O}, &routine::getLastHostileActor);
    routines.add("ExportAllCharacters", VT_V, {}, &routine::exportAllCharacters);
    routines.add("MusicBackgroundGetDayTrack", VT_I, {VT_O}, &routine::musicBackgroundGetDayTrack);
    routines.add("MusicBackgroundGetNightTrack", VT_I, {VT_O}, &routine::musicBackgroundGetNightTrack);
    routines.add("WriteTimestampedLogEntry", VT_V, {VT_S}, &routine::writeTimestampedLogEntry);
    routines.add("GetModuleName", VT_S, {}, &routine::getModuleName);
    routines.add("GetFactionLeader", VT_O, {VT_O}, &routine::getFactionLeader);

    routines.add("SWMG_SetSpeedBlurEffect", VT_V, {VT_I, VT_F}, &routine::minigame::setSpeedBlurEffect);

    routines.add("EndGame", VT_V, {VT_I}, &routine::endGame);
    routines.add("GetRunScriptVar", VT_I, {}, &routine::getRunScriptVar);
    routines.add("GetCreatureMovmentType", VT_I, {VT_O}, &routine::getCreatureMovmentType);
    routines.add("AmbientSoundSetDayVolume", VT_V, {VT_O, VT_I}, &routine::ambientSoundSetDayVolume);
    routines.add("AmbientSoundSetNightVolume", VT_V, {VT_O, VT_I}, &routine::ambientSoundSetNightVolume);
    routines.add("MusicBackgroundGetBattleTrack", VT_I, {VT_O}, &routine::musicBackgroundGetBattleTrack);
    routines.add("GetHasInventory", VT_I, {VT_O}, &routine::getHasInventory);
    routines.add("GetStrRefSoundDuration", VT_F, {VT_I}, &routine::getStrRefSoundDuration);
    routines.add("AddToParty", VT_V, {VT_O, VT_O}, &routine::addToParty);
    routines.add("RemoveFromParty", VT_V, {VT_O}, &routine::removeFromParty);
    routines.add("AddPartyMember", VT_I, {VT_I, VT_O}, &routine::addPartyMember);
    routines.add("RemovePartyMember", VT_I, {VT_I}, &routine::removePartyMember);
    routines.add("IsObjectPartyMember", VT_I, {VT_O}, &routine::isObjectPartyMember);
    routines.add("GetPartyMemberByIndex", VT_O, {VT_I}, &routine::getPartyMemberByIndex);
    routines.add("GetGlobalBoolean", VT_I, {VT_S}, &routine::getGlobalBoolean);
    routines.add("SetGlobalBoolean", VT_V, {VT_S, VT_I}, &routine::setGlobalBoolean);
    routines.add("GetGlobalNumber", VT_I, {VT_S}, &routine::getGlobalNumber);
    routines.add("SetGlobalNumber", VT_V, {VT_S, VT_I}, &routine::setGlobalNumber);
    routines.add("AurPostString", VT_V, {VT_S, VT_I, VT_I, VT_F}, &routine::aurPostString);

    routines.add("SWMG_GetLastEvent", VT_S, {}, &routine::minigame::getLastEvent);
    routines.add("SWMG_GetLastEventModelName", VT_S, {}, &routine::minigame::getLastEventModelName);
    routines.add("SWMG_GetObjectByName", VT_O, {VT_S}, &routine::minigame::getObjectByName);
    routines.add("SWMG_PlayAnimation", VT_V, {VT_O, VT_S, VT_I, VT_I, VT_I}, &routine::minigame::playAnimation);
    routines.add("SWMG_GetLastBulletHitDamage", VT_I, {}, &routine::minigame::getLastBulletHitDamage);
    routines.add("SWMG_GetLastBulletHitTarget", VT_I, {}, &routine::minigame::getLastBulletHitTarget);
    routines.add("SWMG_GetLastBulletHitShooter", VT_O, {}, &routine::minigame::getLastBulletHitShooter);
    routines.add("SWMG_AdjustFollowerHitPoints", VT_I, {VT_O, VT_I, VT_I}, &routine::minigame::adjustFollowerHitPoints);
    routines.add("SWMG_OnBulletHit", VT_V, {}, &routine::minigame::onBulletHit);
    routines.add("SWMG_OnObstacleHit", VT_V, {}, &routine::minigame::onObstacleHit);
    routines.add("SWMG_GetLastFollowerHit", VT_O, {}, &routine::minigame::getLastFollowerHit);
    routines.add("SWMG_GetLastObstacleHit", VT_O, {}, &routine::minigame::getLastObstacleHit);
    routines.add("SWMG_GetLastBulletFiredDamage", VT_I, {}, &routine::minigame::getLastBulletFiredDamage);
    routines.add("SWMG_GetLastBulletFiredTarget", VT_I, {}, &routine::minigame::getLastBulletFiredTarget);
    routines.add("SWMG_GetObjectName", VT_S, {VT_O}, &routine::minigame::getObjectName);
    routines.add("SWMG_OnDeath", VT_V, {}, &routine::minigame::onDeath);
    routines.add("SWMG_IsFollower", VT_I, {VT_O}, &routine::minigame::isFollower);
    routines.add("SWMG_IsPlayer", VT_I, {VT_O}, &routine::minigame::isPlayer);
    routines.add("SWMG_IsEnemy", VT_I, {VT_O}, &routine::minigame::isEnemy);
    routines.add("SWMG_IsTrigger", VT_I, {VT_O}, &routine::minigame::isTrigger);
    routines.add("SWMG_IsObstacle", VT_I, {VT_O}, &routine::minigame::isObstacle);
    routines.add("SWMG_SetFollowerHitPoints", VT_V, {VT_O, VT_I}, &routine::minigame::setFollowerHitPoints);
    routines.add("SWMG_OnDamage", VT_V, {}, &routine::minigame::onDamage);
    routines.add("SWMG_GetLastHPChange", VT_I, {}, &routine::minigame::getLastHPChange);
    routines.add("SWMG_RemoveAnimation", VT_V, {VT_O, VT_S}, &routine::minigame::removeAnimation);
    routines.add("SWMG_GetCameraNearClip", VT_F, {}, &routine::minigame::getCameraNearClip);
    routines.add("SWMG_GetCameraFarClip", VT_F, {}, &routine::minigame::getCameraFarClip);
    routines.add("SWMG_SetCameraClip", VT_V, {VT_F, VT_F}, &routine::minigame::setCameraClip);
    routines.add("SWMG_GetPlayer", VT_O, {}, &routine::minigame::getPlayer);
    routines.add("SWMG_GetEnemyCount", VT_I, {}, &routine::minigame::getEnemyCount);
    routines.add("SWMG_GetEnemy", VT_O, {VT_I}, &routine::minigame::getEnemy);
    routines.add("SWMG_GetObstacleCount", VT_I, {}, &routine::minigame::testObstacleCount);
    routines.add("SWMG_GetObstacle", VT_O, {VT_I}, &routine::minigame::testObstacle);
    routines.add("SWMG_GetHitPoints", VT_I, {VT_O}, &routine::minigame::getHitPoints);
    routines.add("SWMG_GetMaxHitPoints", VT_I, {VT_O}, &routine::minigame::getMaxHitPoints);
    routines.add("SWMG_SetMaxHitPoints", VT_V, {VT_O, VT_I}, &routine::minigame::setMaxHitPoints);
    routines.add("SWMG_GetSphereRadius", VT_F, {VT_O}, &routine::minigame::getSphereRadius);
    routines.add("SWMG_SetSphereRadius", VT_V, {VT_O, VT_F}, &routine::minigame::setSphereRadius);
    routines.add("SWMG_GetNumLoops", VT_I, {VT_O}, &routine::minigame::getNumLoops);
    routines.add("SWMG_SetNumLoops", VT_V, {VT_O, VT_I}, &routine::minigame::setNumLoops);
    routines.add("SWMG_GetPosition", VT_VECTOR, {VT_O}, &routine::minigame::getPosition);
    routines.add("SWMG_GetGunBankCount", VT_I, {VT_O}, &routine::minigame::getGunBankCount);
    routines.add("SWMG_GetGunBankBulletModel", VT_S, {VT_O, VT_I}, &routine::minigame::getGunBankBulletModel);
    routines.add("SWMG_GetGunBankGunModel", VT_S, {VT_O, VT_I}, &routine::minigame::getGunBankGunModel);
    routines.add("SWMG_GetGunBankDamage", VT_I, {VT_O, VT_I}, &routine::minigame::getGunBankDamage);
    routines.add("SWMG_GetGunBankTimeBetweenShots", VT_F, {VT_O, VT_I}, &routine::minigame::getGunBankTimeBetweenShots);
    routines.add("SWMG_GetGunBankLifespan", VT_F, {VT_O, VT_I}, &routine::minigame::getGunBankLifespan);
    routines.add("SWMG_GetGunBankSpeed", VT_F, {VT_O, VT_I}, &routine::minigame::getGunBankSpeed);
    routines.add("SWMG_GetGunBankTarget", VT_I, {VT_O, VT_I}, &routine::minigame::getGunBankTarget);
    routines.add("SWMG_SetGunBankBulletModel", VT_V, {VT_O, VT_I, VT_S}, &routine::minigame::setGunBankBulletModel);
    routines.add("SWMG_SetGunBankGunModel", VT_V, {VT_O, VT_I, VT_S}, &routine::minigame::setGunBankGunModel);
    routines.add("SWMG_SetGunBankDamage", VT_V, {VT_O, VT_I, VT_I}, &routine::minigame::setGunBankDamage);
    routines.add("SWMG_SetGunBankTimeBetweenShots", VT_V, {VT_O, VT_I, VT_F}, &routine::minigame::setGunBankTimeBetweenShots);
    routines.add("SWMG_SetGunBankLifespan", VT_V, {VT_O, VT_I, VT_F}, &routine::minigame::setGunBankLifespan);
    routines.add("SWMG_SetGunBankSpeed", VT_V, {VT_O, VT_I, VT_F}, &routine::minigame::setGunBankSpeed);
    routines.add("SWMG_SetGunBankTarget", VT_V, {VT_O, VT_I, VT_I}, &routine::minigame::setGunBankTarget);
    routines.add("SWMG_GetLastBulletHitPart", VT_S, {}, &routine::minigame::getLastBulletHitPart);
    routines.add("SWMG_IsGunBankTargetting", VT_I, {VT_O, VT_I}, &routine::minigame::isGunBankTargetting);
    routines.add("SWMG_GetPlayerOffset", VT_VECTOR, {}, &routine::minigame::getPlayerOffset);
    routines.add("SWMG_GetPlayerInvincibility", VT_F, {}, &routine::minigame::getPlayerInvincibility);
    routines.add("SWMG_GetPlayerSpeed", VT_F, {}, &routine::minigame::getPlayerSpeed);
    routines.add("SWMG_GetPlayerMinSpeed", VT_F, {}, &routine::minigame::getPlayerMinSpeed);
    routines.add("SWMG_GetPlayerAccelerationPerSecond", VT_F, {}, &routine::minigame::getPlayerAccelerationPerSecond);
    routines.add("SWMG_GetPlayerTunnelPos", VT_VECTOR, {}, &routine::minigame::getPlayerTunnelPos);
    routines.add("SWMG_SetPlayerOffset", VT_V, {VT_VECTOR}, &routine::minigame::setPlayerOffset);
    routines.add("SWMG_SetPlayerInvincibility", VT_V, {VT_F}, &routine::minigame::setPlayerInvincibility);
    routines.add("SWMG_SetPlayerSpeed", VT_V, {VT_F}, &routine::minigame::setPlayerSpeed);
    routines.add("SWMG_SetPlayerMinSpeed", VT_V, {VT_F}, &routine::minigame::setPlayerMinSpeed);
    routines.add("SWMG_SetPlayerAccelerationPerSecond", VT_V, {VT_F}, &routine::minigame::setPlayerAccelerationPerSecond);
    routines.add("SWMG_SetPlayerTunnelPos", VT_V, {VT_VECTOR}, &routine::minigame::setPlayerTunnelPos);
    routines.add("SWMG_GetPlayerTunnelNeg", VT_VECTOR, {}, &routine::minigame::getPlayerTunnelNeg);
    routines.add("SWMG_SetPlayerTunnelNeg", VT_V, {VT_VECTOR}, &routine::minigame::setPlayerTunnelNeg);
    routines.add("SWMG_GetPlayerOrigin", VT_VECTOR, {}, &routine::minigame::getPlayerOrigin);
    routines.add("SWMG_SetPlayerOrigin", VT_V, {VT_VECTOR}, &routine::minigame::setPlayerOrigin);
    routines.add("SWMG_GetGunBankHorizontalSpread", VT_F, {VT_O, VT_I}, &routine::minigame::getGunBankHorizontalSpread);
    routines.add("SWMG_GetGunBankVerticalSpread", VT_F, {VT_O, VT_I}, &routine::minigame::getGunBankVerticalSpread);
    routines.add("SWMG_GetGunBankSensingRadius", VT_F, {VT_O, VT_I}, &routine::minigame::getGunBankSensingRadius);
    routines.add("SWMG_GetGunBankInaccuracy", VT_F, {VT_O, VT_I}, &routine::minigame::getGunBankInaccuracy);
    routines.add("SWMG_SetGunBankHorizontalSpread", VT_V, {VT_O, VT_I, VT_F}, &routine::minigame::setGunBankHorizontalSpread);
    routines.add("SWMG_SetGunBankVerticalSpread", VT_V, {VT_O, VT_I, VT_F}, &routine::minigame::setGunBankVerticalSpread);
    routines.add("SWMG_SetGunBankSensingRadius", VT_V, {VT_O, VT_I, VT_F}, &routine::minigame::setGunBankSensingRadius);
    routines.add("SWMG_SetGunBankInaccuracy", VT_V, {VT_O, VT_I, VT_F}, &routine::minigame::setGunBankInaccuracy);
    routines.add("SWMG_GetIsInvulnerable", VT_I, {VT_O}, &routine::minigame::getIsInvulnerable);
    routines.add("SWMG_StartInvulnerability", VT_V, {VT_O}, &routine::minigame::startInvulnerability);
    routines.add("SWMG_GetPlayerMaxSpeed", VT_F, {}, &routine::minigame::getPlayerMaxSpeed);
    routines.add("SWMG_SetPlayerMaxSpeed", VT_V, {VT_F}, &routine::minigame::setPlayerMaxSpeed);

    routines.add("AddJournalWorldEntry", VT_V, {VT_I, VT_S, VT_S}, &routine::addJournalWorldEntry);
    routines.add("AddJournalWorldEntryStrref", VT_V, {VT_I, VT_I}, &routine::addJournalWorldEntryStrref);
    routines.add("BarkString", VT_V, {VT_O, VT_I}, &routine::barkString);
    routines.add("DeleteJournalWorldAllEntries", VT_V, {}, &routine::deleteJournalWorldAllEntries);
    routines.add("DeleteJournalWorldEntry", VT_V, {VT_I}, &routine::deleteJournalWorldEntry);
    routines.add("DeleteJournalWorldEntryStrref", VT_V, {VT_I}, &routine::deleteJournalWorldEntryStrref);
    routines.add("EffectForceDrain", VT_EFFECT, {VT_I}, &routine::effectForceDrain);
    routines.add("EffectPsychicStatic", VT_EFFECT, {}, &routine::effectPsychicStatic);
    routines.add("PlayVisualAreaEffect", VT_V, {VT_I, VT_LOCATION}, &routine::playVisualAreaEffect);
    routines.add("SetJournalQuestEntryPicture", VT_V, {VT_S, VT_O, VT_I, VT_I, VT_I}, &routine::setJournalQuestEntryPicture);
    routines.add("GetLocalBoolean", VT_I, {VT_O, VT_I}, &routine::getLocalBoolean);
    routines.add("SetLocalBoolean", VT_V, {VT_O, VT_I, VT_I}, &routine::setLocalBoolean);
    routines.add("GetLocalNumber", VT_I, {VT_O, VT_I}, &routine::getLocalNumber);
    routines.add("SetLocalNumber", VT_V, {VT_O, VT_I, VT_I}, &routine::setLocalNumber);

    routines.add("SWMG_GetSoundFrequency", VT_I, {VT_O, VT_I}, &routine::minigame::getSoundFrequency);
    routines.add("SWMG_SetSoundFrequency", VT_V, {VT_O, VT_I, VT_I}, &routine::minigame::setSoundFrequency);
    routines.add("SWMG_GetSoundFrequencyIsRandom", VT_I, {VT_O, VT_I}, &routine::minigame::getSoundFrequencyIsRandom);
    routines.add("SWMG_SetSoundFrequencyIsRandom", VT_V, {VT_O, VT_I, VT_I}, &routine::minigame::setSoundFrequencyIsRandom);
    routines.add("SWMG_GetSoundVolume", VT_I, {VT_O, VT_I}, &routine::minigame::getSoundVolume);
    routines.add("SWMG_SetSoundVolume", VT_V, {VT_O, VT_I, VT_I}, &routine::minigame::setSoundVolume);

    routines.add("SoundObjectGetPitchVariance", VT_F, {VT_O}, &routine::soundObjectGetPitchVariance);
    routines.add("SoundObjectSetPitchVariance", VT_V, {VT_O, VT_F}, &routine::soundObjectSetPitchVariance);
    routines.add("SoundObjectGetVolume", VT_I, {VT_O}, &routine::soundObjectGetVolume);
    routines.add("GetGlobalLocation", VT_LOCATION, {VT_S}, &routine::getGlobalLocation);
    routines.add("SetGlobalLocation", VT_V, {VT_S, VT_LOCATION}, &routine::setGlobalLocation);
    routines.add("AddAvailableNPCByObject", VT_I, {VT_I, VT_O}, &routine::addAvailableNPCByObject);
    routines.add("RemoveAvailableNPC", VT_I, {VT_I}, &routine::removeAvailableNPC);
    routines.add("IsAvailableCreature", VT_I, {VT_I}, &routine::isAvailableCreature);
    routines.add("AddAvailableNPCByTemplate", VT_I, {VT_I, VT_S}, &routine::addAvailableNPCByTemplate);
    routines.add("SpawnAvailableNPC", VT_O, {VT_I, VT_LOCATION}, &routine::spawnAvailableNPC);
    routines.add("IsNPCPartyMember", VT_I, {VT_I}, &routine::isNPCPartyMember);
    routines.add("ActionBarkString", VT_V, {VT_I}, &routine::actionBarkString);
    routines.add("GetIsConversationActive", VT_I, {}, &routine::getIsConversationActive);
    routines.add("EffectLightsaberThrow", VT_EFFECT, {VT_O, VT_O, VT_O, VT_I}, &routine::effectLightsaberThrow);
    routines.add("EffectWhirlWind", VT_EFFECT, {}, &routine::effectWhirlWind);
    routines.add("GetPartyAIStyle", VT_I, {}, &routine::getPartyAIStyle);
    routines.add("GetNPCAIStyle", VT_I, {VT_O}, &routine::getNPCAIStyle);
    routines.add("SetPartyAIStyle", VT_V, {VT_I}, &routine::setPartyAIStyle);
    routines.add("SetNPCAIStyle", VT_V, {VT_O, VT_I}, &routine::setNPCAIStyle);
    routines.add("SetNPCSelectability", VT_V, {VT_I, VT_I}, &routine::setNPCSelectability);
    routines.add("GetNPCSelectability", VT_I, {VT_I}, &routine::getNPCSelectability);
    routines.add("ClearAllEffects", VT_V, {}, &routine::clearAllEffects);
    routines.add("GetLastConversation", VT_S, {}, &routine::getLastConversation);
    routines.add("ShowPartySelectionGUI", VT_V, {VT_S, VT_I, VT_I}, &routine::showPartySelectionGUI);
    routines.add("GetStandardFaction", VT_I, {VT_O}, &routine::getStandardFaction);
    routines.add("GivePlotXP", VT_V, {VT_S, VT_I}, &routine::givePlotXP);
    routines.add("GetMinOneHP", VT_I, {VT_O}, &routine::getMinOneHP);
    routines.add("SetMinOneHP", VT_V, {VT_O, VT_I}, &routine::setMinOneHP);

    routines.add("SWMG_GetPlayerTunnelInfinite", VT_VECTOR, {}, &routine::minigame::getPlayerTunnelInfinite);
    routines.add("SWMG_SetPlayerTunnelInfinite", VT_V, {VT_VECTOR}, &routine::minigame::setPlayerTunnelInfinite);

    routines.add("SetGlobalFadeIn", VT_V, {VT_F, VT_F, VT_F, VT_F, VT_F}, &routine::setGlobalFadeIn);
    routines.add("SetGlobalFadeOut", VT_V, {VT_F, VT_F, VT_F, VT_F, VT_F}, &routine::setGlobalFadeOut);
    routines.add("GetLastHostileTarget", VT_O, {VT_O}, &routine::getLastHostileTarget);
    routines.add("GetLastAttackAction", VT_I, {VT_O}, &routine::getLastAttackAction);
    routines.add("GetLastForcePowerUsed", VT_I, {VT_O}, &routine::getLastForcePowerUsed);
    routines.add("GetLastCombatFeatUsed", VT_I, {VT_O}, &routine::getLastCombatFeatUsed);
    routines.add("GetLastAttackResult", VT_I, {VT_O}, &routine::getLastAttackResult);
    routines.add("GetWasForcePowerSuccessful", VT_I, {VT_O}, &routine::getWasForcePowerSuccessful);
    routines.add("GetFirstAttacker", VT_O, {VT_O}, &routine::getFirstAttacker);
    routines.add("GetNextAttacker", VT_O, {VT_O}, &routine::getNextAttacker);
    routines.add("SetFormation", VT_V, {VT_O, VT_O, VT_I, VT_I}, &routine::setFormation);
    routines.add("ActionFollowLeader", VT_V, {}, &routine::actionFollowLeader);
    routines.add("SetForcePowerUnsuccessful", VT_V, {VT_I, VT_O}, &routine::setForcePowerUnsuccessful);
    routines.add("GetIsDebilitated", VT_I, {VT_O}, &routine::getIsDebilitated);
    routines.add("PlayMovie", VT_V, {VT_S}, &routine::playMovie);
    routines.add("SaveNPCState", VT_V, {VT_I}, &routine::saveNPCState);
    routines.add("GetCategoryFromTalent", VT_I, {VT_TALENT}, &routine::getCategoryFromTalent);
    routines.add("SurrenderByFaction", VT_V, {VT_I, VT_I}, &routine::surrenderByFaction);
    routines.add("ChangeFactionByFaction", VT_V, {VT_I, VT_I}, &routine::changeFactionByFaction);
    routines.add("PlayRoomAnimation", VT_V, {VT_S, VT_I}, &routine::playRoomAnimation);
    routines.add("ShowGalaxyMap", VT_V, {VT_I}, &routine::showGalaxyMap);
    routines.add("SetPlanetSelectable", VT_V, {VT_I, VT_I}, &routine::setPlanetSelectable);
    routines.add("GetPlanetSelectable", VT_I, {VT_I}, &routine::getPlanetSelectable);
    routines.add("SetPlanetAvailable", VT_V, {VT_I, VT_I}, &routine::setPlanetAvailable);
    routines.add("GetPlanetAvailable", VT_I, {VT_I}, &routine::getPlanetAvailable);
    routines.add("GetSelectedPlanet", VT_I, {}, &routine::getSelectedPlanet);
    routines.add("SoundObjectFadeAndStop", VT_V, {VT_O, VT_F}, &routine::soundObjectFadeAndStop);
    routines.add("SetAreaFogColor", VT_V, {VT_O, VT_F, VT_F, VT_F}, &routine::setAreaFogColor);
    routines.add("ChangeItemCost", VT_V, {VT_S, VT_F}, &routine::changeItemCost);
    routines.add("GetIsLiveContentAvailable", VT_I, {VT_I}, &routine::getIsLiveContentAvailable);
    routines.add("ResetDialogState", VT_V, {}, &routine::resetDialogState);
    routines.add("SetGoodEvilValue", VT_V, {VT_O, VT_I}, &routine::setGoodEvilValue);
    routines.add("GetIsPoisoned", VT_I, {VT_O}, &routine::getIsPoisoned);
    routines.add("GetSpellTarget", VT_O, {VT_O}, &routine::getSpellTarget);
    routines.add("SetSoloMode", VT_V, {VT_I}, &routine::setSoloMode);
    routines.add("EffectCutSceneHorrified", VT_EFFECT, {}, &routine::effectCutSceneHorrified);
    routines.add("EffectCutSceneParalyze", VT_EFFECT, {}, &routine::effectCutSceneParalyze);
    routines.add("EffectCutSceneStunned", VT_EFFECT, {}, &routine::effectCutSceneStunned);
    routines.add("CancelPostDialogCharacterSwitch", VT_V, {}, &routine::cancelPostDialogCharacterSwitch);
    routines.add("SetMaxHitPoints", VT_V, {VT_O, VT_I}, &routine::setMaxHitPoints);
    routines.add("NoClicksFor", VT_V, {VT_F}, &routine::noClicksFor);
    routines.add("HoldWorldFadeInForDialog", VT_V, {}, &routine::holdWorldFadeInForDialog);
    routines.add("ShipBuild", VT_I, {}, &routine::shipBuild);
    routines.add("SurrenderRetainBuffs", VT_V, {}, &routine::surrenderRetainBuffs);
    routines.add("SuppressStatusSummaryEntry", VT_V, {VT_I}, &routine::suppressStatusSummaryEntry);
    routines.add("GetCheatCode", VT_I, {VT_I}, &routine::getCheatCode);
    routines.add("SetMusicVolume", VT_V, {VT_F}, &routine::setMusicVolume);
    routines.add("CreateItemOnFloor", VT_O, {VT_S, VT_LOCATION, VT_I}, &routine::createItemOnFloor);
    routines.add("SetAvailableNPCId", VT_V, {VT_I, VT_O}, &routine::setAvailableNPCId);
    routines.add("IsMoviePlaying", VT_I, {}, &routine::isMoviePlaying);
    routines.add("QueueMovie", VT_V, {VT_S, VT_I}, &routine::queueMovie);
    routines.add("PlayMovieQueue", VT_V, {VT_I}, &routine::playMovieQueue);
    routines.add("YavinHackCloseDoor", VT_V, {VT_O}, &routine::yavinHackCloseDoor);
}

void KotOR::initScriptRoutines() {
    fillScriptRoutinesKotOR(_routines);
}

} // namespace kotor

} // namespace reone
