/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "../common/pathutil.h"
#include "../graphics/context.h"
#include "../graphics/meshes.h"
#include "../graphics/pipeline.h"
#include "../graphics/services.h"
#include "../graphics/shaders.h"
#include "../graphics/textures.h"
#include "../graphics/uniforms.h"
#include "../graphics/window.h"
#include "../movie/format/bikreader.h"
#include "../resource/gffs.h"
#include "../resource/services.h"
#include "../scene/graphs.h"
#include "../scene/node/camera.h"
#include "../scene/node/model.h"
#include "../scene/services.h"

#include "astar.h"
#include "cursors.h"
#include "debug.h"
#include "dialog.h"
#include "object/area.h"
#include "object/camera.h"
#include "object/creature.h"
#include "object/door.h"
#include "object/encounter.h"
#include "object/item.h"
#include "object/placeable.h"
#include "object/room.h"
#include "object/sound.h"
#include "object/store.h"
#include "object/trigger.h"
#include "object/waypoint.h"
#include "options.h"
#include "resourcelayout.h"
#include "surfaces.h"

using namespace std;

using namespace reone::movie;
using namespace reone::resource;
using namespace reone::scene;

namespace fs = boost::filesystem;

namespace reone {

namespace game {

static const string kCameraHookNodeName = "camerahook";

void Game::init() {
    loadModuleNames();

    auto &scene = _services.scene.graphs.get(kSceneMain);

    // Movies

    auto legalBikPath = getPathIgnoreCase(_options.game.path, "movies/legal.bik");
    auto bikReader = BikReader(legalBikPath, _services.graphics, _services.audio);
    bikReader.load();
    _movieLegal = bikReader.movie();

    // GUI

    _mainMenu = make_unique<MainMenu>(*this, _options.game, _services.scene, _options.graphics, _services.graphics, _services.resource);
    _mainMenu->init();

    _mainInterface = make_unique<MainInterface>(_options.graphics, _services.graphics, _services.resource);
    _mainInterface->init();

    _dialogGui = make_unique<DialogGui>(_options.graphics, _services.graphics, _services.resource);
    _dialogGui->init();

    _console = make_unique<Console>(*this, _options.graphics, _services.graphics, _services.resource);
    _console->init();

    // Services

    _playerController = make_unique<PlayerController>();
    _selectionController = make_unique<SelectionController>(*this, *_mainInterface, scene);
    _worldRenderer = make_unique<WorldRenderer>(scene, _options.graphics, _services.graphics);

    _routines = make_unique<Routines>(_id, *this, _services);
    _routines->init();

    // Surfaces

    auto walkableSurfaces = _services.game.surfaces.getWalkableSurfaces();
    auto walkcheckSurfaces = _services.game.surfaces.getWalkcheckSurfaces();
    auto lineOfSightSurfaces = _services.game.surfaces.getLineOfSightSurfaces();
    for (auto &scene : _services.scene.graphs.scenes()) {
        scene.second->setWalkableSurfaces(walkableSurfaces);
        scene.second->setWalkcheckSurfaces(walkcheckSurfaces);
        scene.second->setLineOfSightSurfaces(lineOfSightSurfaces);
    }

    // Debugging

    scene.setDrawAABB(isShowAABBEnabled());
    scene.setDrawWalkmeshes(isShowWalkmeshEnabled());
    scene.setDrawTriggers(isShowTriggersEnabled());

    //

    _services.graphics.window.setEventHandler(this);

    changeCursor(CursorType::Default);
}

Object *Game::getObjectByTag(const string &tag, int nth) {
    int match = 0;
    for (auto &object : _objects) {
        if (object.second->tag() != tag) {
            continue;
        }
        if (match++ == nth) {
            return object.second.get();
        }
    }
    return nullptr;
}

void Game::loadModuleNames() {
    auto modulesPath = getPathIgnoreCase(_options.game.path, "modules");
    for (auto &entry : fs::directory_iterator(modulesPath)) {
        auto filename = boost::to_lower_copy(entry.path().filename().string());
        if ((!boost::iends_with(filename, ".rim") && !boost::iends_with(filename, ".mod")) ||
            boost::iends_with(filename, "_s.rim")) {
            continue;
        }
        _moduleNames.insert(filename.substr(0, filename.find_first_of('.')));
    }
}

void Game::run() {
    while (!_finished) {
        handleInput();
        update();
        render();
    }
}

bool Game::handle(const SDL_Event &e) {
    if (e.type == SDL_KEYDOWN) {
        if (e.key.keysym.scancode == SDL_SCANCODE_MINUS) {
            _deltaMultiplier = glm::max(1.0f, _deltaMultiplier - 1.0f);
            return true;
        } else if (e.key.keysym.scancode == SDL_SCANCODE_EQUALS) {
            _deltaMultiplier = glm::min(8.0f, _deltaMultiplier + 1.0f);
            return true;
        }
    }
    if (_stage == Stage::MovieLegal) {
        if (e.type == SDL_MOUSEBUTTONDOWN) {
            _stage = Stage::MainMenu;
            return true;
        }
    } else if (_stage == Stage::MainMenu) {
        if (_mainMenu->handle(e)) {
            return true;
        }
    } else if (_stage == Stage::World) {
        if (_mainInterface->handle(e)) {
            return true;
        }
        if (_module && _module->area().mainCamera().handle(e)) {
            return true;
        }
        if (_selectionController->handle(e)) {
            return true;
        }
        if (_playerController->handle(e)) {
            return true;
        }
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_BACKQUOTE) {
            _stage = Stage::Console;
            return true;
        }
    } else if (_stage == Stage::Conversation) {
        if (_dialogGui->handle(e)) {
            return true;
        }
    } else if (_stage == Stage::Console) {
        if (_console->handle(e)) {
            return true;
        }
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_BACKQUOTE) {
            _stage = Stage::World;
            return true;
        }
    }
    return false;
}

void Game::update() {
    // Calculate delta time

    auto then = _prevFrameTicks;
    if (then == 0) {
        then = _prevFrameTicks = SDL_GetTicks();
    }
    auto now = SDL_GetTicks();
    float delta = _deltaMultiplier * (now - then) / 1000.0f;
    _prevFrameTicks = now;

    if (_stage == Stage::MovieLegal) {
        _movieLegal->update(delta);
        if (_movieLegal->isFinished()) {
            _stage = Stage::MainMenu;
        }

    } else if (_stage == Stage::MainMenu) {
        _mainMenu->update(delta);

    } else if (_stage == Stage::World ||
               _stage == Stage::Conversation ||
               _stage == Stage::Console) {
        // Update game objects

        if (_module) {
            for (auto &object : _module->area().objects()) {
                object->update(delta);
            }
            _module->area().mainCamera().update(delta);
        }
        _playerController->update(delta);

        // Update scene

        auto &scene = _services.scene.graphs.get(kSceneMain);
        scene.update(delta);

        // Update GUI

        if (_stage == Stage::World) {
            _mainInterface->update(delta);
        } else if (_stage == Stage::Conversation) {
            _dialogGui->update(delta);
        } else if (_stage == Stage::Console) {
            _console->update(delta);
        }
    }

    // Update cursor

    if (_stage != Stage::MovieLegal && _cursor) {
        int x, y;
        uint32_t state = SDL_GetMouseState(&x, &y);
        bool pressed = state & SDL_BUTTON(1);

        _cursor->setPosition(glm::ivec2(x, y));
        _cursor->setPressed(pressed);
    }
}

void Game::render() {
    _services.graphics.context.clearColorDepth();

    if (_stage == Stage::MovieLegal) {
        _movieLegal->render();

    } else if (_stage == Stage::MainMenu) {
        _mainMenu->render();

    } else if (_stage == Stage::World ||
               _stage == Stage::Conversation ||
               _stage == Stage::Console) {
        // Render world
        auto &scene = _services.scene.graphs.get(kSceneMain);
        _worldRenderer->render();

        // Render GUI
        if (_stage == Stage::World) {
            _mainInterface->render();
        } else if (_stage == Stage::Conversation) {
            _dialogGui->render();
        } else if (_stage == Stage::Console) {
            _console->render();
        }
    }

    // Render cursor
    if (_stage != Stage::MovieLegal && _cursor) {
        _cursor->draw();
    }

    _services.graphics.window.swapBuffers();
}

void Game::loadModule(const string &name) {
    _services.game.resourceLayout.loadModuleResources(name);

    auto &scene = _services.scene.graphs.get(kSceneMain);
    scene.clear();

    auto &module = static_cast<Module &>(*newModule());
    module.setSceneGraph(&scene);
    module.load(name);
    _module = &module;

    scene.setFog(module.area().fog());

    // Main camera

    auto &camera = module.area().mainCamera();
    scene.setActiveCamera(static_cast<CameraSceneNode *>(camera.sceneNode()));

    // Rooms

    for (auto &room : module.area().rooms()) {
        auto model = static_cast<ModelSceneNode *>(room->sceneNode());
        if (model) {
            scene.addRoot(*model);
        }
        auto grass = room->grass();
        if (grass) {
            scene.addRoot(*grass);
        }
        auto walkmesh = room->walkmesh();
        if (walkmesh) {
            scene.addRoot(*walkmesh);
        }
    }

    // Objects

    for (auto &object : module.area().objects()) {
        auto sceneNode = object->sceneNode();
        if (sceneNode->type() == SceneNodeType::Model) {
            auto model = static_cast<ModelSceneNode *>(sceneNode);
            scene.addRoot(*model);
        } else if (sceneNode->type() == SceneNodeType::Trigger) {
            auto trigger = static_cast<TriggerSceneNode *>(sceneNode);
            scene.addRoot(*trigger);
        }
        if (object->type() == ObjectType::Placeable) {
            auto &placeable = static_cast<Placeable &>(*object);
            auto walkmesh = placeable.walkmesh();
            if (walkmesh) {
                scene.addRoot(*walkmesh);
            }
        } else if (object->type() == ObjectType::Door) {
            auto &door = static_cast<Door &>(*object);
            auto walkmeshClosed = door.walkmeshClosed();
            if (walkmeshClosed) {
                scene.addRoot(*walkmeshClosed);
            }
            auto walkmeshOpen1 = door.walkmeshOpen1();
            if (walkmeshOpen1) {
                scene.addRoot(*walkmeshOpen1);
            }
            auto walkmeshOpen2 = door.walkmeshOpen2();
            if (walkmeshOpen2) {
                scene.addRoot(*walkmeshOpen2);
            }
        }
    }

    // Player character

    auto &pc = module.pc();

    auto pcModel = static_cast<ModelSceneNode *>(pc.sceneNode());
    scene.addRoot(*pcModel);

    auto pcCameraHook = pcModel->getNodeByName(kCameraHookNodeName);
    camera.setMode(Camera::Mode::ThirdPerson);
    camera.setThirdPersonHook(pcCameraHook);

    _playerController->setCreature(&pc);
    _playerController->setCamera(&camera);

    _selectionController->setPC(&pc);

    // Path

    auto path = module.area().path();
    if (path) {
        _services.game.aStar.setPath(*path);
    } else {
        _services.game.aStar.setPath(Path());
    }
}

// IGame

void Game::startNewGame() {
    auto moduleName = _id == GameID::KotOR ? "end_m01aa" : "001ebo";
    warpToModule(moduleName);
}

void Game::warpToModule(const string &name) {
    loadModule(name);
    _stage = Stage::World;
}

void Game::quit() {
    _finished = true;
}

void Game::startConversation(const std::string &name) {
    auto dialog = Dialog(_services.resource);
    dialog.load(name);

    _stage = Stage::Conversation;
}

void Game::changeCursor(CursorType type) {
    if (_cursorType == type) {
        return;
    }
    auto cursor = _services.game.cursors.get(type);
    if (cursor) {
        _cursor = cursor.get();
        SDL_ShowCursor(SDL_DISABLE);
    } else {
        _cursor = nullptr;
        SDL_ShowCursor(SDL_ENABLE);
    }
    _cursorType = type;
}

// END IGame

// IObjectFactory

shared_ptr<Object> Game::newArea() {
    return newObject<Area>();
}

shared_ptr<Object> Game::newCamera() {
    return newObject<Camera>();
}

shared_ptr<Object> Game::newCreature() {
    return newObject<Creature>();
}

shared_ptr<Object> Game::newDoor() {
    return newObject<Door>();
}

shared_ptr<Object> Game::newEncounter() {
    return newObject<Encounter>();
}

shared_ptr<Object> Game::newItem() {
    return newObject<Item>();
}

shared_ptr<Object> Game::newModule() {
    return newObject<Module>();
}

shared_ptr<Object> Game::newPlaceable() {
    return newObject<Placeable>();
}

shared_ptr<Object> Game::newRoom() {
    return newObject<Room>();
}

shared_ptr<Object> Game::newSound() {
    return newObject<Sound>();
}

shared_ptr<Object> Game::newStore() {
    return newObject<Store>();
}

shared_ptr<Object> Game::newTrigger() {
    return newObject<Trigger>();
}

shared_ptr<Object> Game::newWaypoint() {
    return newObject<Waypoint>();
}

// END IObjectFactory

// IEventHandler

void Game::handleInput() {
    _services.graphics.window.processEvents(_finished);
}

// END IEventHandler

bool Game::PlayerController::handle(const SDL_Event &e) {
    if (e.type == SDL_KEYDOWN) {
        if (e.key.keysym.sym == SDLK_w) {
            _forward = 1.0f;
            return true;
        } else if (e.key.keysym.sym == SDLK_z) {
            _left = 1.0f;
            return true;
        } else if (e.key.keysym.sym == SDLK_s) {
            _backward = 1.0f;
            return true;
        } else if (e.key.keysym.sym == SDLK_c) {
            _right = 1.0f;
            return true;
        }
    } else if (e.type == SDL_KEYUP) {
        if (e.key.keysym.sym == SDLK_w) {
            _forward = 0.0f;
            return true;
        } else if (e.key.keysym.sym == SDLK_z) {
            _left = 0.0f;
            return true;
        } else if (e.key.keysym.sym == SDLK_s) {
            _backward = 0.0f;
            return true;
        } else if (e.key.keysym.sym == SDLK_c) {
            _right = 0.0f;
            return true;
        }
    }
    return false;
}

void Game::PlayerController::update(float delta) {
    _creature->update(delta);

    if (!_camera) {
        return;
    }
    float facing;
    if (_forward != 0.0f && _backward == 0.0f) {
        facing = _camera->facing();
    } else if (_forward == 0.0f && _backward != 0.0f) {
        facing = glm::mod(_camera->facing() + glm::pi<float>(), glm::two_pi<float>());
    } else if (_left != 0.0f && _right == 0.0f) {
        facing = glm::mod(_camera->facing() + glm::half_pi<float>(), glm::two_pi<float>());
    } else if (_left == 0.0f && _right != 0.0f) {
        facing = glm::mod(_camera->facing() - glm::half_pi<float>(), glm::two_pi<float>());
    } else {
        _creature->setState(Creature::State::Pause);
        return;
    }
    _creature->setFacing(facing);
    _creature->setState(Creature::State::Run);
    _creature->moveForward(delta);
}

void Game::WorldRenderer::render() {
    auto output = _graphicsSvc.pipeline.draw(_sceneGraph, glm::ivec2(_graphicsOptions.width, _graphicsOptions.height));
    if (!output) {
        return;
    }
    _graphicsSvc.uniforms.setGeneral([](auto &general) {
        general.resetGlobals();
        general.resetLocals();
    });
    _graphicsSvc.shaders.use(_graphicsSvc.shaders.simpleTexture());
    _graphicsSvc.textures.bind(*output);
    _graphicsSvc.meshes.quadNDC().draw();
}

} // namespace game

} // namespace reone
