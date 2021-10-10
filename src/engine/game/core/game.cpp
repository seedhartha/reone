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

#include "game.h"

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

#include "combat.h"
#include "cursors.h"
#include "location.h"
#include "object/factory.h"
#include "party.h"
#include "soundsets.h"
#include "surfaces.h"

using namespace std;

using namespace reone::audio;
using namespace reone::gui;
using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;
using namespace reone::script;
using namespace reone::video;

namespace fs = boost::filesystem;
namespace io = boost::iostreams;

namespace reone {

namespace game {

static constexpr int kNfoBufferSize = 1024;
static constexpr int kScreenBufferSize = 262144;

Game::Game(
    bool tsl,
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
    _tsl(tsl),
    _path(move(path)),
    _options(move(options)),
    _actionFactory(actionFactory),
    _classes(classes),
    _combat(combat),
    _cursors(cursors),
    _effectFactory(effectFactory),
    _feats(feats),
    _footstepSounds(footstepSounds),
    _guiSounds(guiSounds),
    _objectFactory(objectFactory),
    _party(party),
    _portraits(portraits),
    _reputes(reputes),
    _scriptRunner(scriptRunner),
    _skills(skills),
    _soundSets(soundSets),
    _surfaces(surfaces),
    _audioFiles(audioFiles),
    _audioPlayer(audioPlayer),
    _context(context),
    _features(features),
    _fonts(fonts),
    _lips(lips),
    _materials(materials),
    _meshes(meshes),
    _models(models),
    _pbrIbl(pbrIbl),
    _shaders(shaders),
    _textures(textures),
    _walkmeshes(walkmeshes),
    _window(window),
    _sceneGraph(sceneGraph),
    _worldRenderPipeline(worldRenderPipeline),
    _scripts(scripts),
    _resources(resources),
    _strings(strings) {
}

void Game::init() {
    _window.setEventHandler(this);
    _walkmeshes.setWalkableSurfaces(_surfaces.getWalkableSurfaceIndices());

    _console = make_unique<Console>(*this, _effectFactory, _party, _fonts, _meshes, _shaders, _window);
    _console->init();

    _profileOverlay = make_unique<ProfileOverlay>(_fonts, _meshes, _shaders, _window);
    _profileOverlay->init();

    loadModuleNames();
    setCursorType(CursorType::Default);
}

int Game::run() {
    start();
    runMainLoop();

    return 0;
}

void Game::loadModule(const string &name, string entry) {
    info("Load module '" + name + "'");

    withLoadingScreen("load_" + name, [this, &name, &entry]() {
        loadInGameMenus();

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

        if (_loadScreen) {
            _loadScreen->setProgress(50);
        }
        drawAll();

        auto maybeModule = _loadedModules.find(name);
        if (maybeModule != _loadedModules.end()) {
            _module = maybeModule->second;
        } else {
            _module = _objectFactory.newModule();

            shared_ptr<GffStruct> ifo(_resources.getGFF("module", ResourceType::Ifo));
            if (!ifo) {
                return;
            }

            _module->load(name, *ifo, _loadFromSaveGame);
            _loadedModules.insert(make_pair(name, _module));
        }

        _module->loadParty(entry, _loadFromSaveGame);
        _module->area()->fill(_sceneGraph);

        if (_loadScreen) {
            _loadScreen->setProgress(100);
        }
        drawAll();

        string musicName(_module->area()->music());
        playMusic(musicName);

        _ticks = SDL_GetTicks();
        openInGame();
        _loadFromSaveGame = false;
    });
}

void Game::setCursorType(CursorType type) {
    if (_cursorType != type) {
        if (type == CursorType::None) {
            _window.setCursor(nullptr);
        } else {
            _window.setCursor(_cursors.get(type));
        }
        _cursorType = type;
    }
}

void Game::playVideo(const string &name) {
    fs::path path(getPathIgnoreCase(_path, "movies/" + name + ".bik"));
    if (path.empty()) {
        return;
    }

    BikReader bik(path, _context, _meshes, _shaders);
    bik.load();

    _video = bik.video();
    if (!_video) {
        return;
    }

    if (_music) {
        _music->stop();
        _music.reset();
    }
    shared_ptr<AudioStream> audio(_video->audio());
    if (audio) {
        _movieAudio = _audioPlayer.play(audio, AudioType::Movie);
    }
}

void Game::playMusic(const string &resRef) {
    if (_musicResRef == resRef)
        return;

    if (_music) {
        _music->stop();
        _music.reset();
    }
    _musicResRef = resRef;
}

void Game::drawAll() {
    // Compute derived PBR IBL textures from queued environment maps
    _pbrIbl.refresh();

    _window.clear();

    if (_video) {
        _video->draw();
    } else {
        drawWorld();
        drawGUI();
        _window.drawCursor();
    }

    _profileOverlay->draw();
    _window.swapBuffers();
}

void Game::drawWorld() {
    _worldRenderPipeline.render();
}

void Game::toggleInGameCameraType() {
    switch (_cameraType) {
    case CameraType::FirstPerson:
        if (_party.getLeader()) {
            _cameraType = CameraType::ThirdPerson;
        }
        break;
    case CameraType::ThirdPerson: {
        _module->player().stopMovement();
        shared_ptr<Area> area(_module->area());
        auto &thirdPerson = area->getCamera<ThirdPersonCamera>(CameraType::ThirdPerson);
        auto &firstPerson = area->getCamera<FirstPersonCamera>(CameraType::FirstPerson);
        firstPerson.setPosition(thirdPerson.sceneNode()->absoluteTransform()[3]);
        firstPerson.setFacing(thirdPerson.facing());
        _cameraType = CameraType::FirstPerson;
        break;
    }
    default:
        break;
    }

    setRelativeMouseMode(_cameraType == CameraType::FirstPerson);
}

Camera *Game::getActiveCamera() const {
    if (!_module) {
        return nullptr;
    }
    shared_ptr<Area> area(_module->area());
    if (!area) {
        return nullptr;
    }
    return &area->getCamera(_cameraType);
}

shared_ptr<Object> Game::getObjectById(uint32_t id) const {
    switch (id) {
    case kObjectSelf:
        throw invalid_argument("id is invalid");
    case kObjectInvalid:
        return nullptr;
    default:
        return _objectFactory.getObjectById(id);
    }
}

void Game::drawGUI() {
    switch (_screen) {
    case GameScreen::InGame:
        if (_cameraType == CameraType::ThirdPerson) {
            drawHUD();
        }
        if (_console->isOpen()) {
            _console->draw();
        }
        break;

    default: {
        GUI *gui = getScreenGUI();
        if (gui) {
            gui->draw();
            gui->draw3D();
        }
        break;
    }
    }
}

bool Game::isLoadFromSaveGame() const {
    return _loadFromSaveGame;
}

void Game::setLoadFromSaveGame(bool load) {
    _loadFromSaveGame = load;
}

void Game::runMainLoop() {
    _ticks = SDL_GetTicks();

    while (!_quit) {
        _window.processEvents(_quit);

        if (_window.isInFocus()) {
            update();
            drawAll();
        }

        this_thread::yield();
    }
}

void Game::update() {
    float dt = measureFrameTime();

    if (_video) {
        updateVideo(dt);
    } else {
        updateMusic();
    }
    if (!_nextModule.empty()) {
        loadNextModule();
    }
    updateCamera(dt);

    bool updModule = !_video && _module && (_screen == GameScreen::InGame || _screen == GameScreen::Conversation);
    if (updModule && !_paused) {
        _module->update(dt);
        _combat.update(dt);
    }

    GUI *gui = getScreenGUI();
    if (gui) {
        gui->update(dt);
    }
    updateSceneGraph(dt);

    _profileOverlay->update(dt);
}

void Game::updateVideo(float dt) {
    _video->update(dt);

    if (_video->isFinished()) {
        if (_movieAudio) {
            _movieAudio->stop();
            _movieAudio.reset();
        }
        _video.reset();
    }
}

void Game::updateMusic() {
    if (_musicResRef.empty())
        return;

    if (!_music || _music->isStopped()) {
        _music = _audioPlayer.play(_musicResRef, AudioType::Music);
    }
}

void Game::loadNextModule() {
    loadModule(_nextModule, _nextEntry);

    _nextModule.clear();
    _nextEntry.clear();
}

float Game::measureFrameTime() {
    uint32_t ticks = SDL_GetTicks();
    float dt = (ticks - _ticks) / 1000.0f;
    _ticks = ticks;

    return dt * _gameSpeed;
}

void Game::deinit() {
    _audioPlayer.deinit();
    _window.deinit();
}

void Game::quit() {
    _quit = true;
}

void Game::stopMovement() {
    getActiveCamera()->stopMovement();
    _module->player().stopMovement();
}

void Game::scheduleModuleTransition(const string &moduleName, const string &entry) {
    _nextModule = moduleName;
    _nextEntry = entry;
}

void Game::updateCamera(float dt) {
    switch (_screen) {
    case GameScreen::Conversation: {
        int cameraId;
        CameraType cameraType = getConversationCamera(cameraId);
        if (cameraType == CameraType::Static) {
            _module->area()->setStaticCamera(cameraId);
        }
        _cameraType = cameraType;
        break;
    }
    case GameScreen::InGame:
        if (_cameraType != CameraType::FirstPerson && _cameraType != CameraType::ThirdPerson) {
            _cameraType = CameraType::ThirdPerson;
        }
        break;
    default:
        break;
    }
    Camera *camera = getActiveCamera();
    if (camera) {
        camera->update(dt);

        glm::vec3 listenerPosition;
        if (_cameraType == CameraType::ThirdPerson) {
            shared_ptr<Creature> partyLeader(_party.getLeader());
            if (partyLeader) {
                listenerPosition = partyLeader->position() + 1.7f; // TODO: height based on appearance
            }
        } else {
            listenerPosition = camera->sceneNode()->absoluteTransform()[3];
        }
        _audioPlayer.setListenerPosition(listenerPosition);
    }
}

void Game::updateSceneGraph(float dt) {
    const Camera *camera = getActiveCamera();
    if (!camera)
        return;

    // Select a reference node for dynamic lighting
    shared_ptr<SceneNode> lightingRefNode;
    shared_ptr<Creature> partyLeader(_party.getLeader());
    if (partyLeader && _cameraType == CameraType::ThirdPerson) {
        lightingRefNode = partyLeader->sceneNode();
    } else {
        lightingRefNode = camera->sceneNode();
    }

    _sceneGraph.setActiveCamera(camera->sceneNode());
    _sceneGraph.setLightingRefNode(lightingRefNode);
    _sceneGraph.setUpdateRoots(!_paused);
    _sceneGraph.update(dt);
}

bool Game::handle(const SDL_Event &event) {
    if (_profileOverlay->handle(event))
        return true;

    if (!_video) {
        GUI *gui = getScreenGUI();
        if (gui && gui->handle(event)) {
            return true;
        }
        switch (_screen) {
        case GameScreen::InGame: {
            if (_console->handle(event)) {
                return true;
            }
            if (_party.handle(event)) {
                return true;
            }
            Camera *camera = getActiveCamera();
            if (camera && camera->handle(event)) {
                return true;
            }
            if (_module->handle(event)) {
                return true;
            }
            break;
        }
        default:
            break;
        }
    }
    switch (event.type) {
    case SDL_MOUSEBUTTONDOWN:
        if (handleMouseButtonDown(event.button))
            return true;
        break;
    case SDL_KEYDOWN:
        if (handleKeyDown(event.key))
            return true;
        break;
    default:
        break;
    }

    return false;
}

bool Game::handleMouseButtonDown(const SDL_MouseButtonEvent &event) {
    if (event.button != SDL_BUTTON_LEFT)
        return false;

    if (_video) {
        _video->finish();
        return true;
    }

    return false;
}

bool Game::handleKeyDown(const SDL_KeyboardEvent &event) {
    if (event.repeat)
        return false;

    switch (event.keysym.sym) {
    case SDLK_MINUS:
        if (_options.developer && _gameSpeed > 1.0f) {
            _gameSpeed = glm::max(1.0f, _gameSpeed - 1.0f);
            return true;
        }
        break;

    case SDLK_EQUALS:
        if (_options.developer && _gameSpeed < 8.0f) {
            _gameSpeed = glm::min(8.0f, _gameSpeed + 1.0f);
            return true;
        }
        break;

    case SDLK_v:
        if (_options.developer && _screen == GameScreen::InGame) {
            toggleInGameCameraType();
            return true;
        }
        break;

    case SDLK_F1:
        if (_options.developer) {
            _features.toggle(Feature::PBR);
            return true;
        }
        break;

    case SDLK_F3:
        if (_options.developer) {
            _features.toggle(Feature::DynamicRoomLighting);
            return true;
        }
        break;

    default:
        break;
    }

    return false;
}

bool Game::getGlobalBoolean(const string &name) const {
    return getFromLookupOrElse(_globalBooleans, name, false);
}

int Game::getGlobalNumber(const string &name) const {
    return getFromLookupOrElse(_globalNumbers, name, 0);
}

string Game::getGlobalString(const string &name) const {
    static string empty;
    return getFromLookupOrElse(_globalStrings, name, empty);
}

shared_ptr<Location> Game::getGlobalLocation(const string &name) const {
    return getFromLookupOrNull(_globalLocations, name);
}

void Game::setGlobalBoolean(const string &name, bool value) {
    _globalBooleans[name] = value;
}

void Game::setGlobalNumber(const string &name, int value) {
    _globalNumbers[name] = value;
}

void Game::setGlobalString(const string &name, const string &value) {
    _globalStrings[name] = value;
}

void Game::setGlobalLocation(const string &name, const shared_ptr<Location> &location) {
    _globalLocations[name] = location;
}

void Game::setPaused(bool paused) {
    _paused = paused;
}

void Game::setRelativeMouseMode(bool relative) {
    _window.setRelativeMouseMode(relative);
}

void Game::saveToFile(const fs::path &path) {
    // Prepare savenfo RES

    vector<shared_ptr<GffStruct>> nfoPartyMembers;
    for (int i = 0; i < 3; ++i) {
        if (_party.getSize() > i) {
            nfoPartyMembers.push_back(getPartyMemberNFOStruct(i));
        }
    }

    vector<shared_ptr<GffStruct>> nfoGlobalBooleans;
    for (auto &global : _globalBooleans) {
        auto gffs = make_shared<GffStruct>(2, vector<GffField> {
                                                  GffField::newCExoString("Name", global.first),
                                                  GffField::newByte("Value", static_cast<uint32_t>(global.second))});
        nfoGlobalBooleans.push_back(move(gffs));
    }

    vector<shared_ptr<GffStruct>> nfoGlobalNumbers;
    for (auto &global : _globalNumbers) {
        auto gffs = make_shared<GffStruct>(2, vector<GffField> {
                                                  GffField::newCExoString("Name", global.first),
                                                  GffField::newInt("Value", global.second)});
        nfoGlobalNumbers.push_back(move(gffs));
    }

    vector<shared_ptr<GffStruct>> nfoGlobalStrings;
    for (auto &global : _globalStrings) {
        auto gffs = make_shared<GffStruct>(2, vector<GffField> {
                                                  GffField::newCExoString("Name", global.first),
                                                  GffField::newCExoString("Value", global.second)});
        nfoGlobalStrings.push_back(move(gffs));
    }

    vector<shared_ptr<GffStruct>> nfoGlobalLocations;
    for (auto &global : _globalLocations) {
        auto gffs = make_shared<GffStruct>(3, vector<GffField> {
                                                  GffField::newCExoString("Name", global.first),
                                                  GffField::newVector("Position", global.second->position()),
                                                  GffField::newFloat("Facing", global.second->facing())});
        nfoGlobalLocations.push_back(move(gffs));
    }

    auto nfoRoot = make_shared<GffStruct>(0xffffffff);
    nfoRoot->add(GffField::newCExoString("LastModule", _module->name()));
    nfoRoot->add(GffField::newList("Party", move(nfoPartyMembers)));
    nfoRoot->add(GffField::newList("GlobalBooleans", move(nfoGlobalBooleans)));
    nfoRoot->add(GffField::newList("GlobalNumbers", move(nfoGlobalNumbers)));
    nfoRoot->add(GffField::newList("GlobalStrings", move(nfoGlobalStrings)));
    nfoRoot->add(GffField::newList("GlobalLocations", move(nfoGlobalLocations)));

    char nfoBuffer[kNfoBufferSize];
    io::array_sink nfoSink(nfoBuffer, kNfoBufferSize);
    auto nfoStream = make_shared<io::stream<io::array_sink>>(nfoSink);

    GffWriter nfo(ResourceType::Res, nfoRoot);
    nfo.save(nfoStream);
    size_t nfoSize = nfoStream->tellp();

    ByteArray nfoResData;
    nfoResData.resize(nfoSize);
    memcpy(&nfoResData[0], nfoBuffer, nfoSize);

    ErfWriter::Resource nfoRes;
    nfoRes.resRef = "savenfo";
    nfoRes.resType = ResourceType::Res;
    nfoRes.data = move(nfoResData);

    // Prepare screen TGA

    ByteArray screenBuffer;
    screenBuffer.resize(kScreenBufferSize);
    io::array_sink screenSink(&screenBuffer[0], kScreenBufferSize);
    auto screenStream = make_shared<io::stream<io::array_sink>>(screenSink);

    shared_ptr<Texture> screenshot(_worldRenderPipeline.screenshot());
    TgaWriter tga(screenshot);
    tga.save(*screenStream);
    screenBuffer.resize(screenStream->tellp());

    ErfWriter::Resource screenRes;
    screenRes.resRef = "screen";
    screenRes.resType = ResourceType::Tga;
    screenRes.data = move(screenBuffer);

    // Save ERF

    ErfWriter erf;
    erf.add(move(nfoRes));
    erf.add(move(screenRes));
    erf.save(ErfWriter::FileType::ERF, path);
}

shared_ptr<GffStruct> Game::getPartyMemberNFOStruct(int index) const {
    auto member = _party.getMember(index);

    return make_shared<GffStruct>(0, vector<GffField> {
                                         GffField::newByte("NPC", _party.getNPCByMemberIndex(index)),
                                         GffField::newCExoString("TemplateResRef", member->blueprintResRef()),
                                         GffField::newVector("Position", member->position()),
                                         GffField::newFloat("Facing", member->getFacing())});
}

void Game::loadFromFile(const fs::path &path) {
    setLoadFromSaveGame(true);

    ErfReader erf;
    erf.load(path);

    shared_ptr<ByteArray> nfoData(erf.find(ResourceId("savenfo", ResourceType::Res)));

    GffReader nfo;
    nfo.load(wrap(nfoData));

    shared_ptr<GffStruct> nfoRoot(nfo.root());

    // Module
    string lastModule(nfoRoot->getString("LastModule"));
    scheduleModuleTransition(lastModule, "");

    // Party
    vector<shared_ptr<GffStruct>> nfoParty(nfoRoot->getList("Party"));
    _party.clear();
    for (size_t i = 0; i < nfoParty.size(); ++i) {
        shared_ptr<GffStruct> member(nfoParty[i]);
        int npc = member->getInt("NPC");
        string blueprintResRef(member->getString("TemplateResRef"));
        glm::vec3 position(member->getVector("Position"));
        float facing = member->getFloat("Facing");

        shared_ptr<Creature> creature(_objectFactory.newCreature());
        if (npc == -1) {
            creature->setTag(kObjectTagPlayer);
        }
        creature->loadFromBlueprint(blueprintResRef);
        creature->setImmortal(true);
        creature->setPosition(move(position));
        creature->setFacing(facing);
        _party.addMember(npc, creature);
        if (i == 0) {
            _party.setPlayer(creature);
        }
    }

    // Globals
    for (auto &global : nfoRoot->getList("GlobalBooleans")) {
        setGlobalBoolean(global->getString("Name"), global->getBool("Value"));
    }
    for (auto &global : nfoRoot->getList("GlobalNumbers")) {
        setGlobalNumber(global->getString("Name"), global->getInt("Value"));
    }
    for (auto &global : nfoRoot->getList("GlobalStrings")) {
        setGlobalString(global->getString("Name"), global->getString("Value"));
    }
    for (auto &global : nfoRoot->getList("GlobalLocations")) {
        setGlobalLocation(global->getString("Name"), make_shared<Location>(global->getVector("Position"), global->getFloat("Facing")));
    }
}

void Game::withLoadingScreen(const string &imageResRef, const function<void()> &block) {
    if (!_loadScreen) {
        loadLoadingScreen();
    }
    if (_loadScreen) {
        _loadScreen->setImage(imageResRef);
        _loadScreen->setProgress(0);
    }
    changeScreen(GameScreen::Loading);
    drawAll();
    block();
}

} // namespace game

} // namespace reone
