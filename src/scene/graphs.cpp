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

#include "graphs.h"

using namespace std;

namespace reone {

namespace scene {

void SceneGraphs::add(string name) {
    if (_scenes.count(name) > 0) {
        return;
    }
    auto scene = make_unique<SceneGraph>(
        name,
        _options,
        _audioPlayer,
        _context,
        _features,
        _materials,
        _meshes,
        _pbrIbl,
        _shaders,
        _textures);

    _scenes.insert(make_pair(name, move(scene)));
}

SceneGraph &SceneGraphs::get(const string &name) {
    auto maybeScene = _scenes.find(name);
    if (maybeScene == _scenes.end()) {
        throw logic_error(str(boost::format("Scene not found by name '%s'") % name));
    }
    return *maybeScene->second;
}

} // namespace scene

} // namespace reone
