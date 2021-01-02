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

#include "../../resource/gfffile.h"

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

    bool isLockable() const;
    bool isLocked() const;
    bool isStatic() const;

    const std::string &getOnOpen() const;
    const std::string &getOnFailToOpen() const;

    int genericType() const;
    const std::string &linkedToModule() const;
    const std::string &linkedTo() const;
    const std::string &transitionDestin() const;

    void setLocked(bool locked);

private:
    bool _lockable { false };
    bool _locked { false };
    int _genericType { 0 };
    bool _static { false };
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
