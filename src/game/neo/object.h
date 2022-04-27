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

#include "../types.h"

namespace reone {

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

        virtual std::unique_ptr<TObject> build() = 0;

    protected:
        uint32_t _id;
        ObjectType _type;
        std::string _tag;
        std::shared_ptr<scene::SceneNode> _sceneNode;
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

protected:
    uint32_t _id;
    ObjectType _type;
    std::string _tag;
    std::shared_ptr<scene::SceneNode> _sceneNode;

    Object(
        uint32_t id,
        ObjectType type,
        std::string tag,
        std::shared_ptr<scene::SceneNode> sceneNode) :
        _id(id),
        _type(type),
        _tag(std::move(tag)),
        _sceneNode(std::move(sceneNode)) {
    }
};

} // namespace neo

} // namespace game

} // namespace reone