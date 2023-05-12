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

#include "scene.h"

#include "reone/game/types.h"

#include "audio.h"
#include "graphics.h"

using namespace std;

using namespace reone::game;
using namespace reone::scene;

namespace reone {

namespace engine {

void SceneModule::init() {
    _sceneGraphs = make_unique<SceneGraphs>(_graphicsOpt, _graphics.services(), _audio.services());
    _services = make_unique<SceneServices>(*_sceneGraphs);

    // Init scenes
    _sceneGraphs->reserve(kSceneMain);
    _sceneGraphs->reserve(kSceneMainMenu);
    _sceneGraphs->reserve(kSceneCharGen);
    for (int i = 0; i < kNumClasses; ++i) {
        _sceneGraphs->reserve(str(boost::format("%s.%d") % kSceneClassSelect % i));
    }
    _sceneGraphs->reserve(kScenePortraitSelect);
    _sceneGraphs->reserve(kSceneCharacter);
}

void SceneModule::deinit() {
    _services.reset();
    _sceneGraphs.reset();
}

} // namespace engine

} // namespace reone
