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

#include "../blueprint/door.h"

namespace reone {

namespace game {

class Door : public SpatialObject {
public:
    Door(uint32_t id, scene::SceneGraph *sceneGraph);

    std::string conversation() const override;

    void load(const resource::GffStruct &gffs);
    void open(Object *triggerrer);
    void close(Object *triggerrer);

    bool isOpen() const;
    bool isLocked() const;

    const DoorBlueprint &blueprint() const;
    const std::string &linkedToModule() const;
    const std::string &linkedTo() const;
    const std::string &transitionDestin() const;

    void setLocked(bool locked);

private:
    std::shared_ptr<DoorBlueprint> _blueprint;
    bool _open { false };
    bool _locked { false };
    std::string _linkedToModule;
    std::string _linkedTo;
    std::string _transitionDestin;

    void loadBlueprint(const std::string &resRef);
};

} // namespace game

} // namespace reone
