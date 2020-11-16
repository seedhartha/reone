/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include <set>

#include "../../resource/gfffile.h"

#include "spatial.h"

namespace reone {

namespace game {

class TriggerBlueprint;

class Trigger : public SpatialObject {
public:
    Trigger(uint32_t id, scene::SceneGraph *sceneGraph);

    void update(float dt) override;

    void load(const resource::GffStruct &gffs);

    void addTenant(const std::shared_ptr<SpatialObject> &object);

    bool isIn(const glm::vec2 &point) const;
    bool isTenant(const std::shared_ptr<SpatialObject> &object) const;

    const TriggerBlueprint &blueprint() const;
    const std::string &linkedToModule() const;
    const std::string &linkedTo() const;

private:
    std::shared_ptr<TriggerBlueprint> _blueprint;
    std::string _transitionDestin;
    std::string _linkedToModule;
    std::string _linkedTo;
    std::vector<glm::vec3> _geometry;
    std::set<std::shared_ptr<SpatialObject>> _tenants;
};

} // namespace game

} // namespace reone
