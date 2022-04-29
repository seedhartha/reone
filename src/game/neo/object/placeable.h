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

class Placeable : public Object {
public:
    class Builder : public Object::Builder<Placeable, Builder> {
    public:
        Builder &walkmesh(std::shared_ptr<scene::WalkmeshSceneNode> walkmesh) {
            _walkmesh = std::move(walkmesh);
            return *this;
        }

        std::unique_ptr<Placeable> build() override {
            auto placeable = std::make_unique<Placeable>(
                _id,
                _tag,
                _sceneNode,
                *_sceneGraph,
                _walkmesh);
            if (_sceneNode) {
                _sceneNode->setUser(*placeable);
            }
            if (_walkmesh) {
                _walkmesh->setUser(*placeable);
            }
            return std::move(placeable);
        }

    private:
        std::shared_ptr<scene::WalkmeshSceneNode> _walkmesh;
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

        std::unique_ptr<Placeable> load(const resource::GffStruct &gitEntry);

    private:
        IObjectIdSequence &_idSeq;
        scene::SceneGraph &_sceneGraph;
        game::GameServices &_gameSvc;
        graphics::GraphicsServices &_graphicsSvc;
        resource::ResourceServices &_resourceSvc;
    };

    Placeable(
        uint32_t id,
        std::string tag,
        std::shared_ptr<scene::SceneNode> sceneNode,
        scene::SceneGraph &sceneGraph,
        std::shared_ptr<scene::WalkmeshSceneNode> walkmesh) :
        Object(
            id,
            ObjectType::Placeable,
            std::move(tag),
            std::move(sceneNode),
            sceneGraph),
        _walkmesh(std::move(walkmesh)) {
    }

    std::shared_ptr<scene::WalkmeshSceneNode> walkmeshPtr() {
        return _walkmesh;
    }

private:
    std::shared_ptr<scene::WalkmeshSceneNode> _walkmesh;

    void flushTransform() override;
};

} // namespace neo

} // namespace game

} // namespace reone