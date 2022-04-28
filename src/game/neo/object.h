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
#include "../../script/types.h"

#include "../types.h"

namespace reone {

namespace scene {

class SceneGraph;

}

namespace game {

namespace neo {

class IObjectIdSequence {
public:
    virtual uint32_t nextObjectId() = 0;
};

class Object : boost::noncopyable {
public:
    template <class TObject, class TBuilder>
    class Builder : boost::noncopyable {
    public:
        TBuilder &id(uint32_t id) {
            _id = id;
            return static_cast<TBuilder &>(*this);
        }

        TBuilder &tag(std::string tag) {
            _tag = std::move(tag);
            return static_cast<TBuilder &>(*this);
        }

        TBuilder &sceneNode(std::shared_ptr<scene::SceneNode> sceneNode) {
            _sceneNode = std::move(sceneNode);
            return static_cast<TBuilder &>(*this);
        }

        TBuilder &sceneGraph(scene::SceneGraph *sceneGraph) {
            _sceneGraph = sceneGraph;
            return static_cast<TBuilder &>(*this);
        }

        virtual std::unique_ptr<TObject> build() = 0;

    protected:
        uint32_t _id {script::kObjectInvalid};
        ObjectType _type {ObjectType::Invalid};
        std::string _tag;
        std::shared_ptr<scene::SceneNode> _sceneNode;
        scene::SceneGraph *_sceneGraph {nullptr};
    };

    uint32_t id() const {
        return _id;
    }

    ObjectType type() const {
        return _type;
    }

    scene::SceneNode &sceneNode() const {
        return *_sceneNode;
    }

    std::shared_ptr<scene::SceneNode> sceneNodePtr() const {
        return _sceneNode;
    }

    void setPosition(glm::vec3 position) {
        _position = std::move(position);
        flushTransform();
    }

    void setFacing(float facing) {
        _facing = facing;
        flushTransform();
    }

protected:
    uint32_t _id;
    ObjectType _type;
    std::string _tag;
    std::shared_ptr<scene::SceneNode> _sceneNode;
    scene::SceneGraph &_sceneGraph;

    glm::vec3 _position {0.0f};
    float _facing {0.0f};
    float _pitch {0.0f};

    Object(
        uint32_t id,
        ObjectType type,
        std::string tag,
        std::shared_ptr<scene::SceneNode> sceneNode,
        scene::SceneGraph &sceneGraph) :
        _id(id),
        _type(type),
        _tag(std::move(tag)),
        _sceneNode(std::move(sceneNode)),
        _sceneGraph(sceneGraph) {
    }

    virtual void flushTransform() {
        auto transform = glm::translate(_position);
        transform *= glm::rotate(_facing, glm::vec3(0.0f, 0.0f, 1.0f));
        transform *= glm::rotate(_pitch, glm::vec3(1.0f, 0.0f, 0.0f));
        _sceneNode->setLocalTransform(std::move(transform));
    }
};

} // namespace neo

} // namespace game

} // namespace reone