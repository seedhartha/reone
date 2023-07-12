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

#include "reone/resource/format/gffreader.h"

#include "../object.h"
#include "../schema/git.h"
#include "../schema/utt.h"

namespace reone {

namespace game {

class Trigger : public Object {
public:
    Trigger(
        uint32_t id,
        std::string sceneName,
        Game &game,
        ServicesView &services) :
        Object(
            id,
            ObjectType::Trigger,
            std::move(sceneName),
            game,
            services) {
    }

    void loadFromGIT(const schema::GIT_TriggerList &git);
    void loadFromBlueprint(const std::string &resRef);

    void update(float dt) override;

    void addTenant(const std::shared_ptr<Object> &object);

    bool isIn(const glm::vec2 &point) const;
    bool isTenant(const std::shared_ptr<Object> &object) const;

    const std::string &getOnEnter() const { return _onEnter; }
    const std::string &getOnExit() const { return _onExit; }

    const std::string &linkedToModule() const { return _linkedToModule; }
    const std::string &linkedTo() const { return _linkedTo; }

private:
    std::string _transitionDestin;
    std::string _linkedToModule;
    std::string _linkedTo;
    int _linkedToFlags {0};
    Faction _faction {Faction::Invalid};
    float _hilightHeight {0.0f};
    int _triggerType {0};
    bool _trapDetectable {false};
    int _trapDetectDC {0};
    bool _trapDisarmable {false};
    int _disarmDC {0};
    bool _trapFlag {false};
    int _trapType {0};
    std::vector<glm::vec3> _geometry;
    std::set<std::shared_ptr<Object>> _tenants;
    std::string _keyName;

    // Scripts

    std::string _onEnter;
    std::string _onExit;
    std::string _onDisarm;
    std::string _onTrapTriggered;

    // END Scripts

    void loadTransformFromGIT(const schema::GIT_TriggerList &git);
    void loadGeometryFromGIT(const schema::GIT_TriggerList &git);

    void loadUTT(const schema::UTT &utt);
};

} // namespace game

} // namespace reone
