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

#include "../../../common/exception/validation.h"
#include "../../../common/logutil.h"
#include "../../../graphics/models.h"
#include "../../../graphics/services.h"
#include "../../../resource/gffs.h"
#include "../../../resource/gff.h"
#include "../../../resource/services.h"
#include "../../../scene/graph.h"
#include "../../../scene/node/model.h"

#include "../../services.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

namespace neo {

unique_ptr<Module> Module::Loader::load(const string &name) {
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

    // Make area

    auto areaLoader = Area::Loader(_idSeq, _sceneGraph, _gameSvc, _graphicsOpt, _graphicsSvc, _resourceSvc);
    auto area = areaLoader.load(entryArea);

    // Make player character

    auto model = _graphicsSvc.models.get("PMBTest");

    auto pcSceneNode = _sceneGraph.newModel(move(model), ModelUsage::Creature, nullptr);
    auto pc = Creature::Builder()
                  .id(_idSeq.nextObjectId())
                  .tag(kObjectTagPlayer)
                  .sceneNode(move(pcSceneNode))
                  .sceneGraph(&_sceneGraph)
                  .build();

    pc->setPosition(glm::vec3(entryX, entryY, entryZ));
    pc->setFacing(-glm::atan(entryDirX, entryDirY));

    // Make module

    return Module::Builder()
        .id(_idSeq.nextObjectId())
        .tag(name)
        .sceneGraph(&_sceneGraph)
        .area(move(area))
        .pc(move(pc))
        .build();
}

} // namespace neo

} // namespace game

} // namespace reone