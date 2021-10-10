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
#include "../../common/logutil.h"
#include "../../common/pathutil.h"
#include "../../common/streamutil.h"
#include "../../common/streamwriter.h"
#include "../../graphics/features.h"
#include "../../graphics/lip/lips.h"
#include "../../graphics/model/models.h"
#include "../../graphics/pbribl.h"
#include "../../graphics/renderbuffer.h"
#include "../../graphics/texture/format/tgawriter.h"
#include "../../graphics/texture/textures.h"
#include "../../graphics/walkmesh/walkmeshes.h"
#include "../../graphics/window.h"
#include "../../gui/gui.h"
#include "../../resource/format/erfreader.h"
#include "../../resource/format/erfwriter.h"
#include "../../resource/format/gffwriter.h"
#include "../../resource/resources.h"
#include "../../scene/pipeline/world.h"
#include "../../script/scripts.h"
#include "../../video/format/bikreader.h"

#include "../core/combat.h"
#include "../core/cursors.h"
#include "../core/location.h"
#include "../core/object/factory.h"
#include "../core/party.h"
#include "../core/soundsets.h"
#include "../core/surfaces.h"

using namespace std;

using namespace reone::audio;
using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;
using namespace reone::scene;
using namespace reone::script;

namespace fs = boost::filesystem;

namespace reone {

namespace game {

static constexpr char kPatchFilename[] = "patch.erf";
static constexpr char kWavesDirectoryName[] = "streamwaves";
static constexpr char kExeFilename[] = "swkotor.exe";

static vector<string> g_nonTransientLipFiles {"global.mod", "localization.mod"};

static bool g_conversationsEnabled = true;

KotOR::KotOR(
    fs::path path,
    Options options,
    ActionFactory &actionFactory,
    Classes &classes,
    Combat &combat,
    Cursors &cursors,
    EffectFactory &effectFactory,
    Feats &feats,
    FootstepSounds &footstepSounds,
    GUISounds &guiSounds,
    ObjectFactory &objectFactory,
    Party &party,
    Portraits &portraits,
    Reputes &reputes,
    ScriptRunner &scriptRunner,
    Skills &skills,
    SoundSets &soundSets,
    Surfaces &surfaces,
    AudioFiles &audioFiles,
    AudioPlayer &audioPlayer,
    Context &context,
    Features &features,
    Fonts &fonts,
    Lips &lips,
    Materials &materials,
    Meshes &meshes,
    Models &models,
    PBRIBL &pbrIbl,
    Shaders &shaders,
    Textures &textures,
    Walkmeshes &walkmeshes,
    Window &window,
    SceneGraph &sceneGraph,
    WorldRenderPipeline &worldRenderPipeline,
    Scripts &scripts,
    Resources &resources,
    Strings &strings) :
    Game(
        false,
        move(path),
        move(options),
        actionFactory,
        classes,
        combat,
        cursors,
        effectFactory,
        feats,
        footstepSounds,
        guiSounds,
        objectFactory,
        party,
        portraits,
        reputes,
        scriptRunner,
        skills,
        soundSets,
        surfaces,
        audioFiles,
        audioPlayer,
        context,
        features,
        fonts,
        lips,
        materials,
        meshes,
        models,
        pbrIbl,
        shaders,
        textures,
        walkmeshes,
        window,
        sceneGraph,
        worldRenderPipeline,
        scripts,
        resources,
        strings) {
    _mainMenuMusicResRef = "mus_theme_cult";
    _charGenMusicResRef = "mus_theme_rep";
    _charGenLoadScreenResRef = "load_chargen";

    _guiColorBase = glm::vec3(0.0f, 0.639216f, 0.952941f);
    _guiColorHilight = glm::vec3(0.980392f, 1.0f, 0.0f);
    _guiColorDisabled = glm::vec3(0.0f, 0.349020f, 0.549020f);
}

void KotOR::initResourceProviders() {
    _resources.indexKeyFile(getPathIgnoreCase(_path, kKeyFilename));
    _resources.indexErfFile(getPathIgnoreCase(_path, kPatchFilename));

    fs::path texPacksPath(getPathIgnoreCase(_path, kTexturePackDirectoryName));
    _resources.indexErfFile(getPathIgnoreCase(texPacksPath, kGUITexturePackFilename));
    _resources.indexErfFile(getPathIgnoreCase(texPacksPath, kTexturePackFilename));

    _resources.indexDirectory(getPathIgnoreCase(_path, kMusicDirectoryName));
    _resources.indexDirectory(getPathIgnoreCase(_path, kSoundsDirectoryName));
    _resources.indexDirectory(getPathIgnoreCase(_path, kWavesDirectoryName));

    fs::path lipsPath(getPathIgnoreCase(_path, kLipsDirectoryName));
    for (auto &filename : g_nonTransientLipFiles) {
        _resources.indexErfFile(getPathIgnoreCase(lipsPath, filename));
    }

    _resources.indexDirectory(getPathIgnoreCase(_path, kOverrideDirectoryName));
    _resources.indexExeFile(getPathIgnoreCase(_path, kExeFilename));
}

void KotOR::loadModule(const string &name, string entry) {
    info("Load module: " + name);

    withLoadingScreen("load_" + name, [this, &name, &entry]() {
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
        if (!_charGen) {
            loadCharacterGeneration();
        }

        _soundSets.invalidate();
        _textures.invalidateCache();
        _models.invalidateCache();
        _walkmeshes.invalidateCache();
        _lips.invalidate();
        _audioFiles.invalidate();
        _scripts.invalidate();

        loadModuleResources(name);

        if (_module) {
            _module->area()->runOnExitScript();
            _module->area()->unloadParty();
        }

        _loadScreen->setProgress(50);
        drawAll();

        auto maybeModule = _loadedModules.find(name);
        if (maybeModule != _loadedModules.end()) {
            _module = maybeModule->second;
        } else {
            _module = _objectFactory.newModule();

            shared_ptr<GffStruct> ifo(_resources.getGFF("module", ResourceType::Ifo));
            _module->load(name, *ifo, _loadFromSaveGame);

            _loadedModules.insert(make_pair(name, _module));
        }

        _module->loadParty(entry, _loadFromSaveGame);
        _module->area()->fill(_sceneGraph);

        _loadScreen->setProgress(100);
        drawAll();

        string musicName(_module->area()->music());
        playMusic(musicName);

        _ticks = SDL_GetTicks();
        openInGame();
        _loadFromSaveGame = false;
    });
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

void KotOR::loadMainMenu() {
    _mainMenu = make_unique<MainMenu>(
        this,
        _actionFactory,
        _classes,
        _combat,
        _feats,
        _footstepSounds,
        _guiSounds,
        _objectFactory,
        _party,
        _portraits,
        _reputes,
        _scriptRunner,
        _soundSets,
        _surfaces,
        _audioFiles,
        _audioPlayer,
        _context,
        _features,
        _fonts,
        _lips,
        _materials,
        _meshes,
        _models,
        _pbrIbl,
        _shaders,
        _textures,
        _walkmeshes,
        _window,
        _resources,
        _strings);
    _mainMenu->load();
}

void KotOR::loadHUD() {
    _hud = make_unique<HUD>(
        this,
        _actionFactory,
        _classes,
        _combat,
        _feats,
        _footstepSounds,
        _guiSounds,
        _objectFactory,
        _party,
        _portraits,
        _reputes,
        _scriptRunner,
        _skills,
        _soundSets,
        _surfaces,
        _audioFiles,
        _audioPlayer,
        _context,
        _features,
        _fonts,
        _lips,
        _materials,
        _meshes,
        _models,
        _pbrIbl,
        _shaders,
        _textures,
        _walkmeshes,
        _window,
        _resources,
        _strings);
    _hud->load();
}

void KotOR::loadDialog() {
    _dialog = make_unique<DialogGUI>(
        this,
        _actionFactory,
        _classes,
        _combat,
        _feats,
        _footstepSounds,
        _guiSounds,
        _objectFactory,
        _party,
        _portraits,
        _reputes,
        _scriptRunner,
        _soundSets,
        _surfaces,
        _audioFiles,
        _audioPlayer,
        _context,
        _features,
        _fonts,
        _lips,
        _materials,
        _meshes,
        _models,
        _pbrIbl,
        _shaders,
        _textures,
        _walkmeshes,
        _window,
        _resources,
        _strings);
    _dialog->load();
}

void KotOR::loadComputer() {
    _computer = make_unique<ComputerGUI>(
        this,
        _actionFactory,
        _classes,
        _combat,
        _feats,
        _footstepSounds,
        _guiSounds,
        _objectFactory,
        _party,
        _portraits,
        _reputes,
        _scriptRunner,
        _soundSets,
        _surfaces,
        _audioFiles,
        _audioPlayer,
        _context,
        _features,
        _fonts,
        _lips,
        _materials,
        _meshes,
        _models,
        _pbrIbl,
        _shaders,
        _textures,
        _walkmeshes,
        _window,
        _resources,
        _strings);
    _computer->load();
}

void KotOR::loadContainer() {
    _container = make_unique<ContainerGUI>(
        this,
        _actionFactory,
        _classes,
        _combat,
        _feats,
        _footstepSounds,
        _guiSounds,
        _objectFactory,
        _party,
        _portraits,
        _reputes,
        _scriptRunner,
        _soundSets,
        _surfaces,
        _audioFiles,
        _audioPlayer,
        _context,
        _features,
        _fonts,
        _lips,
        _materials,
        _meshes,
        _models,
        _pbrIbl,
        _shaders,
        _textures,
        _walkmeshes,
        _window,
        _resources,
        _strings);
    _container->load();
}

void KotOR::loadPartySelection() {
    _partySelect = make_unique<PartySelection>(
        this,
        _actionFactory,
        _classes,
        _combat,
        _feats,
        _footstepSounds,
        _guiSounds,
        _objectFactory,
        _party,
        _portraits,
        _reputes,
        _scriptRunner,
        _soundSets,
        _surfaces,
        _audioFiles,
        _audioPlayer,
        _context,
        _features,
        _fonts,
        _lips,
        _materials,
        _meshes,
        _models,
        _pbrIbl,
        _shaders,
        _textures,
        _walkmeshes,
        _window,
        _resources,
        _strings);
    _partySelect->load();
}

void KotOR::loadSaveLoad() {
    _saveLoad = make_unique<SaveLoad>(
        this,
        _actionFactory,
        _classes,
        _combat,
        _feats,
        _footstepSounds,
        _guiSounds,
        _objectFactory,
        _party,
        _portraits,
        _reputes,
        _scriptRunner,
        _soundSets,
        _surfaces,
        _audioFiles,
        _audioPlayer,
        _context,
        _features,
        _fonts,
        _lips,
        _materials,
        _meshes,
        _models,
        _pbrIbl,
        _shaders,
        _textures,
        _walkmeshes,
        _window,
        _resources,
        _strings);
    _saveLoad->load();
}

void KotOR::loadLoadingScreen() {
    _loadScreen = make_unique<LoadingScreen>(
        this,
        _actionFactory,
        _classes,
        _combat,
        _feats,
        _footstepSounds,
        _guiSounds,
        _objectFactory,
        _party,
        _portraits,
        _reputes,
        _scriptRunner,
        _soundSets,
        _surfaces,
        _audioFiles,
        _audioPlayer,
        _context,
        _features,
        _fonts,
        _lips,
        _materials,
        _meshes,
        _models,
        _pbrIbl,
        _shaders,
        _textures,
        _walkmeshes,
        _window,
        _resources,
        _strings);
    _loadScreen->load();
}

void KotOR::loadCharacterGeneration() {
    _charGen = make_unique<CharacterGeneration>(
        this,
        _actionFactory,
        _classes,
        _combat,
        _feats,
        _footstepSounds,
        _guiSounds,
        _objectFactory,
        _party,
        _portraits,
        _reputes,
        _scriptRunner,
        _soundSets,
        _surfaces,
        _audioFiles,
        _audioPlayer,
        _context,
        _features,
        _fonts,
        _lips,
        _materials,
        _meshes,
        _models,
        _pbrIbl,
        _shaders,
        _textures,
        _walkmeshes,
        _window,
        _resources,
        _strings);
    _charGen->load();
}

void KotOR::loadInGame() {
    _inGame = make_unique<InGameMenu>(
        this,
        _actionFactory,
        _classes,
        _combat,
        _feats,
        _footstepSounds,
        _guiSounds,
        _objectFactory,
        _party,
        _portraits,
        _reputes,
        _scriptRunner,
        _soundSets,
        _surfaces,
        _audioFiles,
        _audioPlayer,
        _context,
        _features,
        _fonts,
        _lips,
        _materials,
        _meshes,
        _models,
        _pbrIbl,
        _shaders,
        _textures,
        _walkmeshes,
        _window,
        _resources,
        _strings);
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
    _window.clear();
    _worldRenderPipeline.setTakeScreenshot(true);
    _worldRenderPipeline.render();

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

    shared_ptr<GffStruct> dlg(_resources.getGFF(resRef, ResourceType::Dlg));
    if (!dlg) {
        warn("Game: conversation not found: " + resRef);
        return;
    }

    stopMovement();
    setRelativeMouseMode(false);
    setCursorType(CursorType::Default);
    changeScreen(GameScreen::Conversation);

    auto dialog = make_shared<Dialog>(resRef, &_strings);
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
        return _loadScreen.get();
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

void KotOR::withLoadingScreen(const string &imageResRef, const function<void()> &block) {
    if (!_loadScreen) {
        loadLoadingScreen();
    }
    _loadScreen->setImage(imageResRef);
    _loadScreen->setProgress(0);
    changeScreen(GameScreen::Loading);
    drawAll();
    block();
}

} // namespace game

} // namespace reone
