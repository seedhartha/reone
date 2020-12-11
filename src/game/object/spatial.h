/*
 * Copyright (c) 2020 The reone project contributors
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

#include <memory>
#include <queue>
#include <vector>

#include "object.h"

#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

#include "../../render/walkmesh.h"
#include "../../scene/node/modelscenenode.h"

#include "../enginetype/effect.h"

namespace reone {

namespace game {

static const float kDefaultDrawDistance = 1024.0f;

class Item;
class ObjectFactory;
class Room;

class SpatialObject : public Object {
public:
    void update(float dt) override;

    void face(const SpatialObject &other);

    void addItem(const std::string &resRef);
    void addItem(const std::shared_ptr<Item> &item);
    void removeItem(const std::shared_ptr<Item> &item);
    void moveItemsTo(SpatialObject &other);

    void applyEffect(const std::shared_ptr<Effect> &eff);

    float distanceTo(const glm::vec2 &point) const;
    float distanceTo(const glm::vec3 &point) const;
    float distanceTo(const SpatialObject &other) const;

    bool contains(const glm::vec3 &point) const;

    virtual bool isSelectable() const;

    virtual glm::vec3 getSelectablePosition() const;

    ObjectFactory &objectFactory();
    scene::SceneGraph &sceneGraph();
    Room *room() const;
    const glm::vec3 &position() const;
    float heading() const;
    const glm::mat4 &transform() const;
    bool visible() const;
    std::shared_ptr<scene::ModelSceneNode> model() const;
    std::shared_ptr<render::Walkmesh> walkmesh() const;
    const std::vector<std::shared_ptr<Item>> &items() const;
    float drawDistance() const;

    void setRoom(Room *room);
    void setPosition(const glm::vec3 &position);
    void setHeading(float heading);
    void setVisible(bool visible);

protected:
    ObjectFactory *_objectFactory { nullptr };
    scene::SceneGraph *_sceneGraph { nullptr };
    glm::vec3 _position { 0.0f };
    glm::quat _orientation { 1.0f, 0.0f, 0.0f, 0.0f };
    float _heading { 0.0f };
    glm::mat4 _transform { 1.0f };
    bool _visible { true };
    std::shared_ptr<scene::ModelSceneNode> _model;
    std::shared_ptr<render::Walkmesh> _walkmesh;
    float _drawDistance { kDefaultDrawDistance };
    Room *_room { nullptr };
    std::vector<std::shared_ptr<Item>> _items;
    std::deque<std::shared_ptr<Effect>> _effects;

    SpatialObject(uint32_t id, ObjectType type, ObjectFactory *objectFactory, scene::SceneGraph *sceneGraph);

    virtual void updateTransform();
};

} // namespace game

} // namespace reone
