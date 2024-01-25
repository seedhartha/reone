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

namespace reone {

namespace input {

struct Event;

}

namespace graphics {

struct GraphicsOptions;

}

namespace scene {

class ISceneGraph;

class CameraSceneNode;

} // namespace scene

namespace game {

namespace neo {

class SpatialObject;

class SelectionController : boost::noncopyable {
public:
    SelectionController(graphics::GraphicsOptions &graphicsOpt,
                        scene::ISceneGraph &scene) :
        _graphicsOpt(graphicsOpt),
        _scene(scene) {
    }

    bool handle(const input::Event &event);

    void setCamera(scene::CameraSceneNode &camera) {
        _camera = camera;
    }

    std::optional<std::reference_wrapper<SpatialObject>> hoveredObject() const {
        return _hoveredObject;
    }

    std::optional<std::reference_wrapper<SpatialObject>> selectedObject() const {
        return _selectedObject;
    }

private:
    graphics::GraphicsOptions &_graphicsOpt;
    scene::ISceneGraph &_scene;

    std::optional<std::reference_wrapper<scene::CameraSceneNode>> _camera;

    std::optional<std::reference_wrapper<SpatialObject>> _hoveredObject;
    std::optional<std::reference_wrapper<SpatialObject>> _selectedObject;

    std::optional<std::reference_wrapper<SpatialObject>> findObjectAt(int x, int y) const;
};

} // namespace neo

} // namespace game

} // namespace reone
