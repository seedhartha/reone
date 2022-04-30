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

namespace game {

namespace neo {

class Room : public Object {
public:
    class Builder : public Object::Builder<Room, Builder> {
    public:
        Builder &walkmesh(std::shared_ptr<scene::WalkmeshSceneNode> walkmesh) {
            _walkmesh = std::move(walkmesh);
            return *this;
        }

        std::unique_ptr<Room> build() override {
            return std::make_unique<Room>(_id, _tag, _sceneNode, *_sceneGraph, _walkmesh);
        }

    private:
        std::shared_ptr<scene::WalkmeshSceneNode> _walkmesh;
    };

    class Loader : public Object::Loader {
    public:
        Loader(
            IObjectIdSequence &idSeq,
            scene::SceneGraph &sceneGraph,
            game::GameServices &gameSvc,
            graphics::GraphicsOptions &graphicsOpt,
            graphics::GraphicsServices &graphicsSvc,
            resource::ResourceServices &resourceSvc) :
            Object::Loader(
                idSeq,
                sceneGraph,
                gameSvc,
                graphicsOpt,
                graphicsSvc,
                resourceSvc) {
        }

        std::unique_ptr<Room> load(const std::string &name, const glm::vec3 &position);
    };

    Room(
        uint32_t id,
        std::string tag,
        std::shared_ptr<scene::SceneNode> sceneNode,
        scene::SceneGraph &sceneGraph,
        std::shared_ptr<scene::WalkmeshSceneNode> walkmesh) :
        Object(
            id,
            ObjectType::Room,
            std::move(tag),
            std::move(sceneNode),
            sceneGraph),
        _walkmesh(std::move(walkmesh)) {
    }

    scene::WalkmeshSceneNode &walkmesh() {
        return *_walkmesh;
    }

    std::shared_ptr<scene::WalkmeshSceneNode> walkmeshPtr() const {
        return _walkmesh;
    }

private:
    std::shared_ptr<scene::WalkmeshSceneNode> _walkmesh;
};

} // namespace neo

} // namespace game

} // namespace reone