/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "module.h"

#include "reone/common/exception/validation.h"
#include "reone/common/logutil.h"
#include "reone/graphics/models.h"
#include "reone/graphics/services.h"
#include "reone/resource/gff.h"
#include "reone/resource/gffs.h"
#include "reone/resource/services.h"
#include "reone/scene/graph.h"
#include "reone/scene/node/model.h"

#include "../services.h"

#include "factory.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

void Module::load(const string &name) {
    info("Loading module " + name);

    // From IFO

    auto ifo = _resourceSvc.gffs.get("module", ResourceType::Ifo);
    if (!ifo) {
        throw ValidationException("IFO not found: " + name);
    }

    auto entryArea = ifo->getString("Mod_Entry_Area");
    auto entryX = ifo->getFloat("Mod_Entry_X");
    auto entryY = ifo->getFloat("Mod_Entry_Y");
    auto entryZ = ifo->getFloat("Mod_Entry_Z");
    auto entryDirX = ifo->getFloat("Mod_Entry_Dir_X");
    auto entryDirY = ifo->getFloat("Mod_Entry_Dir_Y");

    // Make player character

    auto &pc = static_cast<Creature &>(*_objectFactory.newCreature());
    pc.setSceneGraph(_sceneGraph);
    pc.setPosition(glm::vec3(entryX, entryY, entryZ));
    pc.setFacing(-glm::atan(entryDirX, entryDirY));
    pc.loadFromUtc("p_bastilla");
    _pc = &pc;

    //

    auto &area = static_cast<Area &>(*_objectFactory.newArea());
    area.setSceneGraph(_sceneGraph);
    area.load(entryArea);
    _area = &area;
}

} // namespace game

} // namespace reone
