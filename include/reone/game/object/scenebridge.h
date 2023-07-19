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

#include "event.h"

namespace reone {

namespace scene {

class ISceneGraph;

}

namespace game {

/**
 * Listens to events emitted by game objects and applies them to the scene graph.
 */
class IObjectSceneBridge {
public:
    virtual ~IObjectSceneBridge() = default;

    virtual void enqueue(ObjectEvent event) = 0;
    virtual void applyAll() = 0;
};

class ObjectSceneBridge : public IObjectSceneBridge, boost::noncopyable {
public:
    ObjectSceneBridge(scene::ISceneGraph &sceneGraph) :
        _sceneGraph(sceneGraph) {
    }

    void clear() {
        std::queue<ObjectEvent>().swap(_events);
    }

    void enqueue(ObjectEvent event) override {
        _events.push(std::move(event));
    }

    void applyAll() override;

private:
    scene::ISceneGraph &_sceneGraph;

    std::queue<ObjectEvent> _events;

    void apply(const ObjectEvent &event);

    void applyObjectCreated(const ObjectEvent &event);
    void applyObjectDestroyed(const ObjectEvent &event);
    void applyObjectTransformChanged(const ObjectEvent &event);
};

} // namespace game

} // namespace reone
