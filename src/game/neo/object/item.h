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

#include "../object.h"

namespace reone {

namespace scene {

class SceneGraph;

}

namespace game {

namespace neo {

class Item : public Object {
public:
    class Builder : public Object::Builder<Item, Builder> {
    public:
        std::unique_ptr<Item> build() override {
            return std::make_unique<Item>(_id, _tag, nullptr, *_sceneGraph);
        }
    };

    Item(
        uint32_t id,
        std::string tag,
        std::shared_ptr<scene::SceneNode> sceneNode,
        scene::SceneGraph &sceneGraph) :
        Object(
            id,
            ObjectType::Item,
            std::move(tag),
            std::move(sceneNode),
            sceneGraph) {
    }
};

} // namespace neo

} // namespace game

} // namespace reone
