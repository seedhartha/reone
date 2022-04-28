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

namespace resource {

class GffStruct;

}

namespace game {

struct ServicesView;

namespace neo {

class Creature : public Object {
public:
    class Builder : public Object::Builder<Creature, Builder> {
    public:
        std::unique_ptr<Creature> build() override {
            return std::make_unique<Creature>(_id, _tag, _sceneNode);
        }
    };

    class Loader : boost::noncopyable {
    public:
        Loader(IObjectIdSequence &idSeq, ServicesView &services) :
            _idSeq(idSeq),
            _services(services) {
        }

        std::unique_ptr<Creature> load(const resource::GffStruct &gitEntry);

    private:
        IObjectIdSequence &_idSeq;
        ServicesView &_services;
    };

    Creature(
        uint32_t id,
        std::string tag,
        std::shared_ptr<scene::SceneNode> sceneNode) :
        Object(
            id,
            ObjectType::Creature,
            std::move(tag),
            std::move(sceneNode)) {
    }

    bool moveForward(float delta);
};

} // namespace neo

} // namespace game

} // namespace reone