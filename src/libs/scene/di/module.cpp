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

#include "reone/scene/di/module.h"

#include "reone/game/types.h"

using namespace reone::game;

namespace reone {

namespace scene {

void SceneModule::init() {
    _renderPipelineFactory = std::make_unique<RenderPipelineFactory>(
        _graphicsOpt,
        _graphics.context(),
        _graphics.meshRegistry(),
        _graphics.pbrTextures(),
        _graphics.shaderRegistry(),
        _graphics.statistic(),
        _graphics.textureRegistry(),
        _graphics.uniforms());
    _graphs = std::make_unique<SceneGraphs>(
        *_renderPipelineFactory,
        _graphicsOpt,
        _graphics.services(),
        _audio.services(),
        _resource.services());

    _services = std::make_unique<SceneServices>(*_graphs, *_renderPipelineFactory);

    // Init scenes
    _graphs->reserve(kSceneMain);
    _graphs->reserve(kSceneMainMenu);
    _graphs->reserve(kSceneCharGen);
    for (int i = 0; i < kNumClasses; ++i) {
        _graphs->reserve(str(boost::format("%s.%d") % kSceneClassSelect % i));
    }
    _graphs->reserve(kScenePortraitSelect);
    _graphs->reserve(kSceneCharacter);
}

void SceneModule::deinit() {
    _services.reset();

    _graphs.reset();
    _renderPipelineFactory.reset();
}

} // namespace scene

} // namespace reone
