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

namespace scene {

struct SceneServices;

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
    ActionExecutor(std::unique_lock<std::mutex> &sceneLock,
                   resource::ResourceServices &resourceSvc,
                   scene::SceneServices &sceneSvc) :
        _sceneLock(sceneLock),
        _resourceSvc(resourceSvc),
        _sceneSvc(sceneSvc) {
    }

    bool executeAction(Object &subject, const Action &action, float dt) override;

    void setModule(Module &module) {
        _module = module;
    }

private:
    std::unique_lock<std::mutex> &_sceneLock;
    resource::ResourceServices &_resourceSvc;
    scene::SceneServices &_sceneSvc;

    std::optional<std::reference_wrapper<Module>> _module;

    bool executeMoveToPoint(Creature &subject, const Action &action, float dt);
};

} // namespace neo

} // namespace game

} // namespace reone
