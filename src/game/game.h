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

#pragma once

#include "../graphics/eventhandler.h"
#include "../movie/movie.h"

#include "gui/game.h"
#include "gui/maininterface.h"
#include "gui/mainmenu.h"
#include "object/factory.h"
#include "object/module.h"
#include "services.h"
#include "types.h"

namespace reone {

namespace scene {

class SceneGraph;
class SceneNode;

} // namespace scene

namespace graphics {

struct GraphicsOptions;
struct GraphicsServices;

} // namespace graphics

namespace game {

struct OptionsView;

class Creature;

class Game : public IGuiGame, public IObjectIdSequence, public graphics::IEventHandler, boost::noncopyable {
public:
    Game(GameID id, OptionsView &options, ServicesView &services) :
        _id(id),
        _options(options),
        _services(services) {
    }

    void init();

    void run();

    // IGuiGame

    void startNewGame() override;

    void quit() override;

    // END IGuiGame

    // IObjectIdSequence

    uint32_t nextObjectId() override {
        return _objectIdCounter++;
    }

    // END IObjectIdSequence

    // IEventHandler

    bool handle(const SDL_Event &e) override;

    // END IEventHandler

private:
    enum class Stage {
        MovieLegal,
        MainMenu,
        World
    };

    class PlayerController : boost::noncopyable {
    public:
        bool handle(const SDL_Event &e);
        void update(float delta);

        void setCreature(Creature *creature) {
            _creature = creature;
        }

        void setCamera(Camera *camera) {
            _camera = camera;
        }

    private:
        Creature *_creature {nullptr};
        Camera *_camera {nullptr};

        // Controls
        float _forward {0.0f};
        float _left {0.0f};
        float _backward {0.0f};
        float _right {0.0f};
    };

    class SelectionController : boost::noncopyable {
    public:
        SelectionController(scene::SceneGraph &sceneGraph) :
            _sceneGraph(sceneGraph) {
        }

        bool handle(const SDL_Event &e);

        void setPC(Creature *pc) {
            _pc = pc;
        }

    private:
        scene::SceneGraph &_sceneGraph;

        Creature *_pc {nullptr};

        Object *_hoveredObject {nullptr};
        Object *_clickedObject {nullptr};
    };

    class WorldRenderer : boost::noncopyable {
    public:
        WorldRenderer(
            scene::SceneGraph &sceneGraph,
            graphics::GraphicsOptions &graphicsOptions,
            graphics::GraphicsServices &graphicsSvc) :
            _sceneGraph(sceneGraph),
            _graphicsOptions(graphicsOptions),
            _graphicsSvc(graphicsSvc) {
        }

        void render();

    private:
        scene::SceneGraph &_sceneGraph;
        graphics::GraphicsOptions &_graphicsOptions;
        graphics::GraphicsServices &_graphicsSvc;
    };

    GameID _id;
    OptionsView &_options;
    ServicesView &_services;

    bool _finished {false};
    uint32_t _prevFrameTicks {0};
    uint32_t _objectIdCounter {2}; // 0 is self, 1 is invalid

    Stage _stage {Stage::MovieLegal};

    std::shared_ptr<Module> _module;

    // Services

    std::unique_ptr<ObjectFactory> _objectFactory;
    std::unique_ptr<PlayerController> _playerController;
    std::unique_ptr<SelectionController> _selectionController;
    std::unique_ptr<WorldRenderer> _worldRenderer;

    // END Services

    // Movies

    std::shared_ptr<movie::Movie> _movieLegal;

    // END Movies

    // GUI

    std::unique_ptr<MainMenu> _mainMenu;
    std::unique_ptr<MainInterface> _mainInterface;

    // END GUI

    void handleInput();
    void update();
    void render();

    void loadModule(const std::string &name);
};

} // namespace game

} // namespace reone
