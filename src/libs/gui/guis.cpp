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

#include "reone/gui/guis.h"

#include "reone/resource/provider/gffs.h"

using namespace reone::resource;

namespace reone {

namespace gui {

std::shared_ptr<IGUI> GUIs::doGet(std::string resRef, std::function<void(IGUI &)> preload) {
    auto gff = _resourceSvc.gffs.get(resRef, ResType::Gui);
    if (!gff) {
        return nullptr;
    }
    auto gui = std::make_shared<GUI>(_graphicsOpt, _sceneGraphs, _graphicsSvc, _resourceSvc);
    if (preload) {
        preload(*gui);
    }
    gui->load(*gff);
    return gui;
}

} // namespace gui

} // namespace reone
