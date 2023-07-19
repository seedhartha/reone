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

class SceneGraph;

}

namespace game {

/**
 * Listens to events emitted by game objects and applies them to the scene graph.
 */
class IObjectSceneBridge {
public:
    virtual ~IObjectSceneBridge() = 0;

    virtual void enqueue(ObjectEvent event) = 0;
    virtual void flush() = 0;
};

class ObjectSceneBridge : public IObjectSceneBridge, boost::noncopyable {
public:
    ObjectSceneBridge(scene::SceneGraph &sceneGraph) :
        _sceneGraph(sceneGraph) {
    }

    void enqueue(ObjectEvent event) override {
        _events.push(std::move(event));
    }

    void flush() override;

private:
    scene::SceneGraph &_sceneGraph;

    std::queue<ObjectEvent> _events;

    void applyEvent(const ObjectEvent &event);
};

} // namespace game

} // namespace reone
