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

#include <set>

#include "../../resource/format/gfffile.h"

#include "spatial.h"

namespace reone {

namespace game {

class TriggerBlueprint;

class Trigger : public SpatialObject {
public:
    Trigger(
        uint32_t id,
        ObjectFactory *objectFactory,
        scene::SceneGraph *sceneGraph,
        ScriptRunner *scriptRunner);

    void update(float dt) override;

    void load(const resource::GffStruct &gffs);
    void addTenant(const std::shared_ptr<SpatialObject> &object);

    bool isIn(const glm::vec2 &point) const;
    bool isTenant(const std::shared_ptr<SpatialObject> &object) const;

    const std::string &getOnEnter() const { return _onEnter; }
    const std::string &getOnExit() const { return _onExit; }

    const std::string &linkedToModule() const { return _linkedToModule; }
    const std::string &linkedTo() const { return _linkedTo; }

private:
    std::string _transitionDestin;
    std::string _linkedToModule;
    std::string _linkedTo;
    std::vector<glm::vec3> _geometry;
    std::set<std::shared_ptr<SpatialObject>> _tenants;

    // Scripts

    std::string _onEnter;
    std::string _onExit;

    // END Scripts

    void loadBlueprint(const resource::GffStruct &gffs);
    void loadAppearance();

    friend class TriggerBlueprint;
};

} // namespace game

} // namespace reone
