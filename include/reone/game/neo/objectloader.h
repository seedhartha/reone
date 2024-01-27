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

#include "object/area.h"
#include "object/module.h"

namespace reone {

namespace resource {

struct ResourceServices;

}

namespace game {

namespace neo {

class IObjectRepository;

class ObjectLoader : public IAreaLoader, public IAreaObjectLoader, boost::noncopyable {
public:
    ObjectLoader(IObjectRepository &objectRepository,
                 resource::ResourceServices &resourceSvc) :
        _objectRepository(objectRepository),
        _resourceSvc(resourceSvc) {
    }

    Module &loadModule(const std::string &name);
    Creature &loadCreature(ObjectTag tag, PortraitId portraitId);

    // IAreaLoader

    Area &loadArea(const std::string &name) override;

    // END IAreaLoader

    // IAreaObjectLoader

    Camera &loadCamera() override;
    Creature &loadCreature(const resource::ResRef &tmplt) override;
    Door &loadDoor(const resource::ResRef &tmplt) override;
    Encounter &loadEncounter(const resource::ResRef &tmplt) override;
    Placeable &loadPlaceable(const resource::ResRef &tmplt) override;
    Sound &loadSound(const resource::ResRef &tmplt) override;
    Store &loadStore(const resource::ResRef &tmplt) override;
    Trigger &loadTrigger(const resource::ResRef &tmplt) override;
    Waypoint &loadWaypoint(const resource::ResRef &tmplt) override;

    // END IAreaObjectLoader

private:
    IObjectRepository &_objectRepository;
    resource::ResourceServices &_resourceSvc;
};

} // namespace neo

} // namespace game

} // namespace reone
