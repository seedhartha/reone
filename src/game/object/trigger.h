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

#include "spatial.h"

#include "../../resource/gfffile.h"

namespace reone {

namespace game {

class TriggerBlueprint;

class Trigger : public SpatialObject {
public:
    Trigger(uint32_t id, scene::SceneGraph *sceneGraph);

    void load(const resource::GffStruct &gffs);

    const TriggerBlueprint &blueprint() const;
    const std::string &linkedToModule() const;
    const std::string &linkedTo() const;
    const std::vector<glm::vec3> &geometry() const;

private:
    std::shared_ptr<TriggerBlueprint> _blueprint;
    std::string _tag;
    std::string _transitionDestin;
    std::string _linkedToModule;
    std::string _linkedTo;
    std::vector<glm::vec3> _geometry;
};

} // namespace game

} // namespace reone
