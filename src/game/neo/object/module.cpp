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
#include "../../../resource/gffs.h"
#include "../../../resource/gffstruct.h"
#include "../../../resource/services.h"

#include "../../services.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace game {

namespace neo {

unique_ptr<Module> Module::Loader::load(const string &name) {
    info("Loading module " + name);

    auto ifo = _services.resource.gffs.get("module", ResourceType::Ifo);
    if (!ifo) {
        throw ValidationException("IFO not found: " + name);
    }

    auto areas = vector<shared_ptr<Area>>();
    auto areaLoader = Area::Loader(_idSeq, _services);
    auto ifoAreas = ifo->getList("Mod_Area_list");

    for (auto &ifoArea : ifoAreas) {
        auto areaName = ifoArea->getString("Area_Name");
        areas.push_back(areaLoader.load(areaName));
    }

    return Module::Builder()
        .id(_idSeq.nextObjectId())
        .tag(name)
        .areas(std::move(areas))
        .build();
}

} // namespace neo

} // namespace game

} // namespace reone