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

#pragma once

#include "reone/scene/node.h"
#include "reone/scene/user.h"
#include "reone/script/types.h"

#include "action.h"
#include "types.h"

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

class IGame;
class IItem;
class IObjectFactory;

class Room;

class Object : public scene::IUser, boost::noncopyable {
public:
    typedef std::queue<std::shared_ptr<Action>> ActionQueue;
    typedef std::vector<IItem *> ItemList;

    virtual void handleClick(Object &clicker) {
    }

    virtual void update(float delta);

    void face(Object &other);
    void face(const glm::vec2 &point);

    virtual void show();
    virtual void hide();

    uint32_t id() const {
        return _id;
    }

    ObjectType type() const {
        return _type;
    }

    const std::string &tag() const {
        return _tag;
    }

    const std::string &name() const {
        return _name;
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

    scene::SceneNode *sceneNode() {
        return _sceneNode;
    }

    const Room *room() const {
        return _room;
    }

    float square2dDistanceTo(Object &other) const {
        return glm::distance2(glm::vec2(_position), glm::vec2(other._position));
    }

    float square2dDistanceTo(const glm::vec2 &point) const {
        return glm::distance2(glm::vec2(_position), point);
    }

    virtual glm::vec3 targetWorldCoords() const;

    glm::ivec3 targetScreenCoords() const;

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

    void setSceneNode(scene::SceneNode *sceneNode) {
        _sceneNode = std::move(sceneNode);
        flushTransform();
    }

    void setRoom(Room *room) {
        _room = room;
    }

    // Actions

    void clearAllActions() {
        auto empty = ActionQueue();
        _actions.swap(empty);
    }

    void enqueue(std::shared_ptr<Action> action) {
        _actions.push(std::move(action));
    }

    const ActionQueue &actions() const {
        return _actions;
    }

    // END Actions

    // Items

    void add(IItem &item) {
        _items.push_back(&item);
    }

    void remove(IItem &item) {
        auto it = std::find(_items.begin(), _items.end(), &item);
        if (it != _items.end()) {
            _items.erase(it);
        }
    }

    const ItemList &items() const {
        return _items;
    }

    // END Items

    // Local variables

    const std::map<std::string, bool> &localBooleans() const {
        return _localBooleans;
    }

    const std::map<std::string, int> &localNumbers() const {
        return _localNumbers;
    }

    void setLocalBoolean(const std::string &key, bool value) {
        _localBooleans[key] = value;
    }

    void setLocalNumber(const std::string &key, int value) {
        _localNumbers[key] = value;
    }

    // END Local variables

protected:
    uint32_t _id;
    ObjectType _type;
    IGame &_game;
    IObjectFactory &_objectFactory;
    GameServices &_gameSvc;
    graphics::GraphicsOptions &_graphicsOpt;
    graphics::GraphicsServices &_graphicsSvc;
    resource::ResourceServices &_resourceSvc;

    std::string _tag;
    std::string _name;

    ActionQueue _actions;
    ItemList _items;

    glm::vec3 _position {0.0f};
    float _facing {0.0f};
    float _pitch {0.0f};

    scene::SceneGraph *_sceneGraph {nullptr};
    scene::SceneNode *_sceneNode {nullptr};

    Room *_room {nullptr};

    // Local variables

    std::map<std::string, bool> _localBooleans;
    std::map<std::string, int> _localNumbers;

    // END Local variables

    Object(
        uint32_t id,
        ObjectType type,
        IGame &game,
        IObjectFactory &objectFactory,
        GameServices &gameSvc,
        graphics::GraphicsOptions &graphicsOpt,
        graphics::GraphicsServices &graphicsSvc,
        resource::ResourceServices &resourceSvc) :
        _id(id),
        _type(type),
        _game(game),
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

} // namespace game

} // namespace reone