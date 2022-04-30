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

#include "../../graphics/context.h"
#include "../../graphics/meshes.h"
#include "../../graphics/pipeline.h"
#include "../../graphics/services.h"
#include "../../graphics/shaders.h"
#include "../../graphics/textures.h"
#include "../../graphics/window.h"
#include "../../resource/gffs.h"
#include "../../resource/services.h"
#include "../../scene/graphs.h"
#include "../../scene/node/camera.h"
#include "../../scene/node/model.h"
#include "../../scene/services.h"

#include "../debug.h"
#include "../options.h"
#include "../resourcelayout.h"
#include "../surfaces.h"

#include "object/door.h"
#include "object/module.h"
#include "object/placeable.h"

using namespace std;

using namespace reone::scene;
using namespace reone::resource;

namespace reone {

namespace game {

namespace neo {

static const string kCameraHookNodeName = "camerahook";

void Game::init() {
    auto &scene = _services.scene.graphs.get(kSceneMain);

    // Helpers

    _playerController = make_unique<PlayerController>();
    _selectionController = make_unique<SelectionController>(scene);
    _worldRenderer = make_unique<WorldRenderer>(scene, _options.graphics, _services.graphics);

    // GUI

    _mainInterface = make_unique<MainInterfaceGui>(_options.graphics, _services.graphics, _services.resource);
    _mainInterface->init();

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
    _services.graphics.window.setRelativeMouseMode(true);
}

void Game::run() {
    auto moduleName = _id == GameID::KotOR ? "end_m01aa" : "001ebo";
    loadModule(moduleName);

    while (!_finished) {
        handleInput();
        update();
        render();
    }
}

void Game::handleInput() {
    _services.graphics.window.processEvents(_finished);
}

void Game::update() {
    // Calculate delta time

    auto then = _prevFrameTicks;
    if (then == 0) {
        then = _prevFrameTicks = SDL_GetTicks();
    }
    auto now = SDL_GetTicks();
    float delta = (now - then) / 1000.0f;
    _prevFrameTicks = now;

    // Update game objects

    if (_module) {
        _module->area().mainCamera().update(delta);
    }
    _playerController->update(delta);

    // Update scene

    auto &scene = _services.scene.graphs.get(kSceneMain);
    scene.update(delta);

    // Update GUI

    _mainInterface->gui().update(delta);
}

void Game::render() {
    _services.graphics.context.clearColorDepth();

    // Render world
    auto &scene = _services.scene.graphs.get(kSceneMain);
    _worldRenderer->render();

    // Render GUI
    _mainInterface->gui().render();

    _services.graphics.window.swapBuffers();
}

bool Game::handle(const SDL_Event &e) {
    if (_mainInterface->gui().handle(e)) {
        return true;
    }
    if (_selectionController->handle(e)) {
        return true;
    }
    if (_playerController->handle(e)) {
        return true;
    }
    if (_module && _module->area().mainCamera().handle(e)) {
        return true;
    }
    return false;
}

void Game::loadModule(const string &name) {
    _services.game.resourceLayout.loadModuleResources(name);

    auto &scene = _services.scene.graphs.get(kSceneMain);
    auto newModule = Module::Loader(*this, scene, _services.game, _options.graphics, _services.graphics, _services.resource).load(name);

    _module = move(newModule);

    scene.clear();

    // Rooms

    for (auto &room : _module->area().rooms()) {
        auto model = static_pointer_cast<ModelSceneNode>(room->sceneNodePtr());
        if (model) {
            scene.addRoot(move(model));
        }
        auto walkmesh = room->walkmeshPtr();
        if (walkmesh) {
            scene.addRoot(move(walkmesh));
        }
    }

    // Objects

    for (auto &object : _module->area().objects()) {
        auto model = static_pointer_cast<ModelSceneNode>(object->sceneNodePtr());
        if (model) {
            scene.addRoot(move(model));
        }
        if (object->type() == ObjectType::Placeable) {
            auto &placeable = static_cast<Placeable &>(*object);
            auto walkmesh = placeable.walkmeshPtr();
            if (walkmesh) {
                scene.addRoot(move(walkmesh));
            }
        } else if (object->type() == ObjectType::Door) {
            auto &door = static_cast<Door &>(*object);
            auto walkmeshClosed = door.walkmeshClosedPtr();
            if (walkmeshClosed) {
                scene.addRoot(move(walkmeshClosed));
            }
            auto walkmeshOpen1 = door.walkmeshOpen1Ptr();
            if (walkmeshOpen1) {
                scene.addRoot(move(walkmeshOpen1));
            }
            auto walkmeshOpen2 = door.walkmeshOpen2Ptr();
            if (walkmeshOpen2) {
                scene.addRoot(move(walkmeshOpen2));
            }
        }
    }

    // Player character / camera

    auto &pc = _module->pc();
    auto &camera = _module->area().mainCamera();

    auto pcModel = static_pointer_cast<ModelSceneNode>(pc.sceneNodePtr());
    auto pcCameraHook = pcModel->getNodeByName(kCameraHookNodeName);
    scene.addRoot(pcModel);

    camera.setMode(Camera::Mode::ThirdPerson);
    camera.setThirdPersonHook(pcCameraHook.get());
    scene.setActiveCamera(static_pointer_cast<CameraSceneNode>(camera.sceneNodePtr()));

    _playerController->setCreature(&pc);
    _playerController->setCamera(&camera);

    _selectionController->setPC(&pc);

    //

    _services.graphics.window.setRelativeMouseMode(false);
}

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
        return;
    }
    _creature->setFacing(facing);
    _creature->moveForward(delta);
}

bool Game::SelectionController::handle(const SDL_Event &e) {
    if (e.type == SDL_MOUSEMOTION) {
        auto hoveredSceneNode = _sceneGraph.pickModelAt(e.motion.x, e.motion.y, _pc);
        if (hoveredSceneNode) {
            _hoveredObject = static_cast<Object *>(hoveredSceneNode->user());
            // debug("Object hovered on: " + to_string(_hoveredObject->id()) + "[" + _hoveredObject->tag() + "]");
        } else {
            _hoveredObject = nullptr;
        }
        return true;
    }
    if (e.type == SDL_MOUSEBUTTONDOWN && _hoveredObject) {
        _clickedObject = _hoveredObject;
        debug("Object clicked on: " + to_string(_clickedObject->id()) + "[" + _clickedObject->tag() + "]");
        _clickedObject->handleClick(*_pc);
        return true;
    }
    return false;
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

} // namespace neo

} // namespace game

} // namespace reone
