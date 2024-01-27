/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "types.h"

namespace reone {

namespace graphics {

class Walkmesh;

}

namespace resource {

struct ResourceServices;

}

namespace game {

namespace neo {

struct Action;

class Creature;
class Module;
class Object;

class IActionExecutor {
public:
    virtual ~IActionExecutor() = default;

    virtual bool executeAction(Object &subject, const Action &action, float dt) = 0;
};

class ActionExecutor : public IActionExecutor, boost::noncopyable {
public:
    ActionExecutor(resource::ResourceServices &resourceSvc) :
        _resourceSvc(resourceSvc) {
    }

    bool executeAction(Object &subject, const Action &action, float dt) override;

    void setModule(Module &module) {
        _module = module;
    }

    void setWalkSurfaceMaterials(std::set<uint32_t> materials) {
        _walkSurfaceMaterials = std::move(materials);
    }

    void setWalkcheckSurfaceMaterials(std::set<uint32_t> materials) {
        _walkcheckSurfaceMaterials = std::move(materials);
    }

private:
    enum class DoorWalkmeshType {
        Closed,
        Open1,
        Open2
    };

    struct ObjectWalkmesh {
        ObjectId objectId;
        graphics::Walkmesh &walkmesh;
        std::optional<DoorWalkmeshType> doorType;

        ObjectWalkmesh(ObjectId objectId,
                       graphics::Walkmesh &walkmesh,
                       std::optional<DoorWalkmeshType> doorType = std::nullopt) :
            objectId(objectId),
            walkmesh(walkmesh),
            doorType(std::move(doorType)) {
        }
    };

    resource::ResourceServices &_resourceSvc;

    std::optional<std::reference_wrapper<Module>> _module;
    std::set<uint32_t> _walkSurfaceMaterials;
    std::set<uint32_t> _walkcheckSurfaceMaterials;

    bool executeMoveToPoint(Creature &subject, const Action &action, float dt);
};

} // namespace neo

} // namespace game

} // namespace reone
