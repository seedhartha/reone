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

#include "../../game/types.h"
#include "../../kotor/types.h"

#include "audio.h"
#include "graphics.h"

using namespace std;

using namespace reone::game;
using namespace reone::graphics;
using namespace reone::kotor;
using namespace reone::scene;

namespace reone {

namespace di {

void SceneModule::init() {
    _sceneGraphs = make_unique<SceneGraphs>(
        _options,
        _audio.audioPlayer(),
        _graphics.context(),
        _graphics.features(),
        _graphics.materials(),
        _graphics.meshes(),
        _graphics.pbrIbl(),
        _graphics.shaders(),
        _graphics.textures());

    _sceneGraphs->add(kSceneMain);
    _sceneGraphs->add(kSceneMainMenu);
    _sceneGraphs->add(kSceneCharGen);
    for (int i = 0; i < kNumClasses; ++i) {
        _sceneGraphs->add(str(boost::format("%s.%d") % kSceneClassSelect % i));
    }
    _sceneGraphs->add(kScenePortraitSelect);
    _sceneGraphs->add(kSceneCharacter);

    auto &mainScene = _sceneGraphs->get(kSceneMain);

    _worldRenderPipeline = make_unique<WorldRenderPipeline>(_options, mainScene, _graphics.context(), _graphics.meshes(), _graphics.shaders());
    _worldRenderPipeline->init();

    _controlRenderPipeline = make_unique<ControlRenderPipeline>(*_sceneGraphs, _graphics.context(), _graphics.meshes(), _graphics.shaders());
    _controlRenderPipeline->init();
}

} // namespace di

} // namespace reone
