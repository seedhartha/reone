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

#include "spatial.h"

#include "../../resource/format/gffreader.h"

#include "../blueprint/door.h"

namespace reone {

namespace game {

class Door : public SpatialObject {
public:
    Door(
        uint32_t id,
        ObjectFactory *objectFactory,
        scene::SceneGraph *sceneGraph,
        ScriptRunner *scriptRunner);

    bool isSelectable() const override;

    void load(const resource::GffStruct &gffs);
    void open(const std::shared_ptr<Object> &triggerrer);
    void close(const std::shared_ptr<Object> &triggerrer);

    bool isLockable() const { return _lockable; }
    bool isLocked() const { return _locked; }
    bool isStatic() const { return _static; }
    bool isKeyRequired() const { return _keyRequired; }

    const std::string &getOnOpen() const { return _onOpen; }
    const std::string &getOnFailToOpen() const { return _onFailToOpen; }

    int genericType() const { return _genericType; }
    const std::string &linkedToModule() const { return _linkedToModule; }
    const std::string &linkedTo() const { return _linkedTo; }
    const std::string &transitionDestin() const { return _transitionDestin; }

    void setLocked(bool locked);

private:
    bool _lockable { false };
    bool _locked { false };
    int _genericType { 0 };
    bool _static { false };
    bool _keyRequired { false };
    std::string _linkedToModule;
    std::string _linkedTo;
    std::string _transitionDestin;

    // Scripts

    std::string _onOpen;
    std::string _onFailToOpen;

    // END Scripts

    void loadBlueprint(const resource::GffStruct &gffs);

    friend class DoorBlueprint;
};

} // namespace game

} // namespace reone
