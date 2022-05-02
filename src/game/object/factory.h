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

namespace game {

class IObjectIdSequence {
public:
    virtual uint32_t nextObjectId() = 0;
};

class ObjectFactory : boost::noncopyable {
public:
    ObjectFactory(
        IObjectIdSequence &idSeq,
        GameServices &gameSvc,
        graphics::GraphicsOptions &graphicsOpt,
        graphics::GraphicsServices &graphicsSvc,
        resource::ResourceServices &resourceSvc) :
        _idSeq(idSeq),
        _gameSvc(gameSvc),
        _graphicsOpt(graphicsOpt),
        _graphicsSvc(graphicsSvc),
        _resourceSvc(resourceSvc) {
    }

    std::unique_ptr<Object> newArea();
    std::unique_ptr<Object> newCamera();
    std::unique_ptr<Object> newCreature();
    std::unique_ptr<Object> newDoor();
    std::unique_ptr<Object> newEncounter();
    std::unique_ptr<Object> newItem();
    std::unique_ptr<Object> newModule();
    std::unique_ptr<Object> newPlaceable();
    std::unique_ptr<Object> newRoom();
    std::unique_ptr<Object> newSound();
    std::unique_ptr<Object> newStore();
    std::unique_ptr<Object> newTrigger();
    std::unique_ptr<Object> newWaypoint();

private:
    IObjectIdSequence &_idSeq;
    GameServices &_gameSvc;
    graphics::GraphicsOptions &_graphicsOpt;
    graphics::GraphicsServices &_graphicsSvc;
    resource::ResourceServices &_resourceSvc;
};

} // namespace game

} // namespace reone