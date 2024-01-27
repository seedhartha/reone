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

namespace graphics {

struct GraphicsServices;

class Texture;

} // namespace graphics

namespace resource {

struct ResourceServices;

}

namespace scene {

class ISceneGraph;

class CameraSceneNode;

} // namespace scene

namespace game {

namespace neo {

class SpatialObject;

class SelectionOverlay : boost::noncopyable {
public:
    SelectionOverlay(scene::ISceneGraph &scene,
                     graphics::GraphicsServices &graphicsSvc,
                     resource::ResourceServices &resourceSvc) :
        _scene(scene),
        _graphicsSvc(graphicsSvc),
        _resourceSvc(resourceSvc) {
    }

    ~SelectionOverlay() {
        deinit();
    }

    void init();
    void deinit();

    void render(const glm::ivec2 &screenSize);

    void setHoveredObject(std::optional<std::reference_wrapper<SpatialObject>> object) {
        _hoveredObject = std::move(object);
    }

    void setSelectedObject(std::optional<std::reference_wrapper<SpatialObject>> object) {
        _selectedObject = std::move(object);
    }

    void setCamera(scene::CameraSceneNode &camera) {
        _camera = camera;
    }

private:
    scene::ISceneGraph &_scene;
    graphics::GraphicsServices &_graphicsSvc;
    resource::ResourceServices &_resourceSvc;

    std::shared_ptr<graphics::Texture> _friendlyReticle;
    std::shared_ptr<graphics::Texture> _friendlyReticle2;

    bool _inited {false};

    std::optional<std::reference_wrapper<SpatialObject>> _hoveredObject;
    std::optional<std::reference_wrapper<SpatialObject>> _selectedObject;
    std::optional<std::reference_wrapper<scene::CameraSceneNode>> _camera;
};

} // namespace neo

} // namespace game

} // namespace reone
