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

#include "scene.h"

#include "../../di/services/graphics.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::scene;

namespace reone {

namespace di {

SceneServices::SceneServices(GraphicsOptions options, GraphicsServices &graphics) :
    _options(move(options)),
    _graphics(graphics) {
}

void SceneServices::init() {
    _graph = make_unique<SceneGraph>(
        _options,
        _graphics.context(),
        _graphics.features(),
        _graphics.materials(),
        _graphics.meshes(),
        _graphics.pbrIbl(),
        _graphics.shaders(),
        _graphics.textures());

    _worldRenderPipeline = make_unique<WorldRenderPipeline>(_options, *_graph, _graphics.context(), _graphics.meshes(), _graphics.shaders());
    _worldRenderPipeline->init();
}

} // namespace di

} // namespace reone
