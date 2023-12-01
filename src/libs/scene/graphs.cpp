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

#include "reone/scene/graphs.h"

namespace reone {

namespace scene {

void SceneGraphs::reserve(std::string name) {
    if (_scenes.count(name) > 0) {
        return;
    }
    auto scene = std::make_unique<SceneGraph>(
        name,
        _graphicsOpt,
        _graphicsSvc,
        _audioSvc,
        _resourceSvc);

    _scenes.insert(std::make_pair(name, std::move(scene)));
}

ISceneGraph &SceneGraphs::get(const std::string &name) {
    auto maybeScene = _scenes.find(name);
    if (maybeScene == _scenes.end()) {
        throw std::logic_error(str(boost::format("Scene not found by name '%s'") % name));
    }
    return *maybeScene->second;
}

} // namespace scene

} // namespace reone
