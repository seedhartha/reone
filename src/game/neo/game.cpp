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

#include "../options.h"
#include "../resourcelayout.h"

#include "object/module.h"

using namespace std;

using namespace reone::scene;
using namespace reone::resource;

namespace reone {

namespace game {

namespace neo {

static const string kCameraHookNodeName = "camerahook";

void Game::init() {
    auto &scene = _services.scene.graphs.get(kSceneMain);

    _playerController = make_unique<PlayerController>();
    _worldRenderer = make_unique<WorldRenderer>(scene, _options.graphics, _services.graphics);

    _services.graphics.window.setEventHandler(this);
    _services.graphics.window.setRelativeMouseMode(true);
}

void Game::run() {
    auto moduleName = _id == GameID::KotOR ? "end_m01aa" : "001ebo";
    loadModule("end_m01aa");

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

    if (_module) {
        _module->area().mainCamera().update(delta);
    }
    _playerController->update(delta);

    auto &scene = _services.scene.graphs.get(kSceneMain);
    scene.update(delta);
}

void Game::render() {
    auto &scene = _services.scene.graphs.get(kSceneMain);
    _worldRenderer->render();
}

bool Game::handle(const SDL_Event &e) {
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

    auto newModule = Module::Loader(*this, _services).load(name);

    _module = move(newModule);

    auto &scene = _services.scene.graphs.get(kSceneMain);
    scene.clear();

    // Rooms

    for (auto &room : _module->area().rooms()) {
        auto model = static_pointer_cast<ModelSceneNode>(room->sceneNodePtr());
        if (model) {
            scene.addRoot(move(model));
        }
    }

    // Objects

    for (auto &object : _module->area().objects()) {
        auto model = static_pointer_cast<ModelSceneNode>(object->sceneNodePtr());
        if (model) {
            scene.addRoot(move(model));
        }
    }

    // Main camera

    auto &mainCamera = _module->area().mainCamera();
    mainCamera.setMode(Camera::Mode::ThirdPerson);
    scene.setActiveCamera(static_pointer_cast<CameraSceneNode>(mainCamera.sceneNodePtr()));

    // Player character

    auto &pc = _module->pc();
    _playerController->setCreature(pc);

    auto pcModel = static_pointer_cast<ModelSceneNode>(pc.sceneNodePtr());
    pcModel->attach(kCameraHookNodeName, mainCamera.sceneNodePtr());
    scene.addRoot(move(pcModel));
}

static constexpr float kPlayerMoveSpeed = 1.0f;

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
    auto &sceneNode = _creature->sceneNode();

    auto transform = glm::translate(sceneNode.getOrigin());
    transform *= glm::rotate(_bearing, glm::vec3(0.0f, 0.0f, 1.0f));
    transform *= glm::translate(delta * kPlayerMoveSpeed * glm::vec3(_right - _left, _forward - _backward, 0.0f));

    sceneNode.setLocalTransform(move(transform));
}

void Game::WorldRenderer::render() {
    auto output = _graphicsSvc.pipeline.draw(_sceneGraph, glm::ivec2(_graphicsOptions.width, _graphicsOptions.height));
    if (!output) {
        return;
    }
    _graphicsSvc.context.clearColorDepth();
    _graphicsSvc.uniforms.setGeneral([](auto &general) {
        general.resetGlobals();
        general.resetLocals();
    });
    _graphicsSvc.shaders.use(_graphicsSvc.shaders.simpleTexture());
    _graphicsSvc.textures.bind(*output);
    _graphicsSvc.meshes.quadNDC().draw();
    _graphicsSvc.window.swapBuffers();
}

} // namespace neo

} // namespace game

} // namespace reone
