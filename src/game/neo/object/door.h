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

#include "../../../scene/node/walkmesh.h"

#include "../object.h"

namespace reone {

namespace scene {

class SceneGraph;

}

namespace graphics {

struct GraphicsServices;

}

namespace resource {

struct ResourceServices;

class GffStruct;

} // namespace resource

namespace game {

struct GameServices;

namespace neo {

class Door : public Object {
public:
    class Builder : public Object::Builder<Door, Builder> {
    public:
        Builder &walkmeshClosed(std::shared_ptr<scene::WalkmeshSceneNode> walkmesh) {
            _walkmeshClosed = std::move(walkmesh);
            return *this;
        }

        Builder &walkmeshOpen1(std::shared_ptr<scene::WalkmeshSceneNode> walkmesh) {
            _walkmeshOpen1 = std::move(walkmesh);
            return *this;
        }

        Builder &walkmeshOpen2(std::shared_ptr<scene::WalkmeshSceneNode> walkmesh) {
            _walkmeshOpen2 = std::move(walkmesh);
            return *this;
        }

        std::unique_ptr<Door> build() override {
            auto door = std::make_unique<Door>(
                _id,
                _tag,
                _sceneNode,
                *_sceneGraph,
                _walkmeshClosed,
                _walkmeshOpen1,
                _walkmeshOpen2);
            if (_sceneNode) {
                _sceneNode->setUser(*door);
            }
            if (_walkmeshClosed) {
                _walkmeshClosed->setUser(*door);
            }
            if (_walkmeshOpen1) {
                _walkmeshOpen1->setUser(*door);
            }
            if (_walkmeshOpen2) {
                _walkmeshOpen2->setUser(*door);
            }
            return std::move(door);
        }

    private:
        std::shared_ptr<scene::WalkmeshSceneNode> _walkmeshClosed;
        std::shared_ptr<scene::WalkmeshSceneNode> _walkmeshOpen1;
        std::shared_ptr<scene::WalkmeshSceneNode> _walkmeshOpen2;
    };

    class Loader : boost::noncopyable {
    public:
        Loader(
            IObjectIdSequence &idSeq,
            scene::SceneGraph &sceneGraph,
            game::GameServices &gameSvc,
            graphics::GraphicsServices &graphicsSvc,
            resource::ResourceServices &resourceSvc) :
            _idSeq(idSeq),
            _sceneGraph(sceneGraph),
            _gameSvc(gameSvc),
            _graphicsSvc(graphicsSvc),
            _resourceSvc(resourceSvc) {
        }

        std::unique_ptr<Door> load(const resource::GffStruct &gitEntry);

    private:
        IObjectIdSequence &_idSeq;
        scene::SceneGraph &_sceneGraph;
        game::GameServices &_gameSvc;
        graphics::GraphicsServices &_graphicsSvc;
        resource::ResourceServices &_resourceSvc;
    };

    Door(
        uint32_t id,
        std::string tag,
        std::shared_ptr<scene::SceneNode> sceneNode,
        scene::SceneGraph &sceneGraph,
        std::shared_ptr<scene::WalkmeshSceneNode> walkmeshClosed,
        std::shared_ptr<scene::WalkmeshSceneNode> walkmesh1,
        std::shared_ptr<scene::WalkmeshSceneNode> walkmesh2) :
        Object(
            id,
            ObjectType::Door,
            std::move(tag),
            std::move(sceneNode),
            sceneGraph),
        _walkmeshClosed(std::move(walkmeshClosed)),
        _walkmeshOpen1(std::move(walkmesh1)),
        _walkmeshOpen2(std::move(walkmesh2)) {
    }

    std::shared_ptr<scene::WalkmeshSceneNode> walkmeshClosedPtr() {
        return _walkmeshClosed;
    }

    std::shared_ptr<scene::WalkmeshSceneNode> walkmeshOpen1Ptr() {
        return _walkmeshOpen1;
    }

    std::shared_ptr<scene::WalkmeshSceneNode> walkmeshOpen2Ptr() {
        return _walkmeshOpen2;
    }

private:
    std::shared_ptr<scene::WalkmeshSceneNode> _walkmeshClosed;
    std::shared_ptr<scene::WalkmeshSceneNode> _walkmeshOpen1;
    std::shared_ptr<scene::WalkmeshSceneNode> _walkmeshOpen2;

    void flushTransform() override;
};

} // namespace neo

} // namespace game

} // namespace reone