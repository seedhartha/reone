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

#include "../../scene/node.h"
#include "../../scene/user.h"
#include "../../script/types.h"

#include "../types.h"

namespace reone {

namespace scene {

class SceneGraph;

}

namespace graphics {

struct GraphicsOptions;
struct GraphicsServices;

} // namespace graphics

namespace resource {

struct ResourceServices;

class Gff;

} // namespace resource

namespace game {

struct GameServices;

namespace neo {

class ObjectFactory;

class Object : public scene::IUser, boost::noncopyable {
public:
    virtual void update(float delta) {
    }

    virtual void handleClick(Object &clicker) {
    }

    uint32_t id() const {
        return _id;
    }

    ObjectType type() const {
        return _type;
    }

    const std::string &tag() const {
        return _tag;
    }

    const glm::vec3 &position() const {
        return _position;
    }

    float facing() const {
        return _facing;
    }

    float pitch() const {
        return _pitch;
    }

    scene::SceneNode &sceneNode() const {
        return *_sceneNode;
    }

    std::shared_ptr<scene::SceneNode> sceneNodePtr() const {
        return _sceneNode;
    }

    void setTag(std::string tag) {
        _tag = std::move(tag);
    }

    void setPosition(glm::vec3 position) {
        _position = std::move(position);
        flushTransform();
    }

    void setFacing(float facing) {
        _facing = facing;
        flushTransform();
    }

    void setPitch(float pitch) {
        _pitch = pitch;
        flushTransform();
    }

    void setSceneGraph(scene::SceneGraph *sceneGraph) {
        _sceneGraph = sceneGraph;
    }

    void setSceneNode(std::shared_ptr<scene::SceneNode> sceneNode) {
        _sceneNode = std::move(sceneNode);
        flushTransform();
    }

protected:
    uint32_t _id;
    ObjectType _type;
    ObjectFactory &_objectFactory;
    GameServices &_gameSvc;
    graphics::GraphicsOptions &_graphicsOpt;
    graphics::GraphicsServices &_graphicsSvc;
    resource::ResourceServices &_resourceSvc;

    std::string _tag;

    glm::vec3 _position {0.0f};
    float _facing {0.0f};
    float _pitch {0.0f};

    scene::SceneGraph *_sceneGraph {nullptr};
    std::shared_ptr<scene::SceneNode> _sceneNode;

    Object(
        uint32_t id,
        ObjectType type,
        ObjectFactory &objectFactory,
        GameServices &gameSvc,
        graphics::GraphicsOptions &graphicsOpt,
        graphics::GraphicsServices &graphicsSvc,
        resource::ResourceServices &resourceSvc) :
        _id(id),
        _type(type),
        _objectFactory(objectFactory),
        _gameSvc(gameSvc),
        _graphicsOpt(graphicsOpt),
        _graphicsSvc(graphicsSvc),
        _resourceSvc(resourceSvc) {
    }

    virtual void flushTransform() {
        if (!_sceneNode) {
            return;
        }
        auto transform = glm::translate(_position);
        transform *= glm::rotate(_facing, glm::vec3(0.0f, 0.0f, 1.0f));
        transform *= glm::rotate(_pitch, glm::vec3(1.0f, 0.0f, 0.0f));
        _sceneNode->setLocalTransform(std::move(transform));
    }
};

} // namespace neo

} // namespace game

} // namespace reone