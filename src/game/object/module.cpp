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

#include "module.h"

#include "../../common/exception/validation.h"
#include "../../common/logutil.h"
#include "../../graphics/models.h"
#include "../../graphics/services.h"
#include "../../resource/gff.h"
#include "../../resource/gffs.h"
#include "../../resource/services.h"
#include "../../scene/graph.h"
#include "../../scene/node/model.h"

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

    _pc = static_pointer_cast<Creature>(_objectFactory.newCreature());
    _pc->setSceneGraph(_sceneGraph);
    _pc->setPosition(glm::vec3(entryX, entryY, entryZ));
    _pc->setFacing(-glm::atan(entryDirX, entryDirY));
    _pc->loadFromUtc("p_bastilla");

    //

    _area = static_pointer_cast<Area>(_objectFactory.newArea());
    _area->setSceneGraph(_sceneGraph);
    _area->load(entryArea);
}

} // namespace game

} // namespace reone
