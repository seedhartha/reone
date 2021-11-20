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

namespace reone {

namespace scene {

class ModelSceneNode;
class WalkmeshSceneNode;

} // namespace scene

namespace game {

class SpatialObject;

class Room {
public:
    Room(
        std::string name,
        glm::vec3 position,
        std::shared_ptr<scene::ModelSceneNode> model,
        std::shared_ptr<scene::WalkmeshSceneNode> walkmesh) :
        _name(std::move(name)),
        _position(std::move(position)),
        _model(model),
        _walkmesh(walkmesh) {
    }

    void addTenant(SpatialObject *object);
    void removeTenant(SpatialObject *object);

    bool isVisible() const { return _visible; }

    const std::string &name() const { return _name; }
    const glm::vec3 &position() const { return _position; }
    std::shared_ptr<scene::ModelSceneNode> model() const { return _model; }
    std::shared_ptr<scene::WalkmeshSceneNode> walkmesh() const { return _walkmesh; }

    void setVisible(bool visible);

private:
    std::string _name;
    glm::vec3 _position {0.0f};
    std::shared_ptr<scene::ModelSceneNode> _model;
    std::shared_ptr<scene::WalkmeshSceneNode> _walkmesh;

    std::set<SpatialObject *> _tenants;
    bool _visible {true};
};

} // namespace game

} // namespace reone
