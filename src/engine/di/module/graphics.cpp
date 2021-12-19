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

#include "graphics.h"

#include "resource.h"

using namespace std;

using namespace reone::graphics;

namespace reone {

void GraphicsModule::init() {
    _window = make_unique<Window>(_options);
    _graphicsContext = make_unique<GraphicsContext>(_options);
    _meshes = make_unique<Meshes>();
    _textures = make_unique<Textures>(*_graphicsContext, _resource.resources());
    _models = make_unique<Models>(*_textures, _resource.resources());
    _walkmeshes = make_unique<Walkmeshes>(_resource.resources());
    _lipAnimations = make_unique<LipAnimations>(_resource.resources());
    _shaders = make_unique<Shaders>(*_graphicsContext);
    _fonts = make_unique<Fonts>(*_window, *_graphicsContext, *_meshes, *_textures, *_shaders);
    _worldPipeline = make_unique<WorldPipeline>(_options, *_graphicsContext, *_meshes, *_shaders);
    _controlPipeline = make_unique<ControlPipeline>(_options, *_graphicsContext, *_meshes, *_shaders);

    _window->init();
    _graphicsContext->init();
    _meshes->init();
    _textures->init();
    _shaders->init();
    _worldPipeline->init();
}

void GraphicsModule::deinit() {
    _worldPipeline.reset();
    _controlPipeline.reset();
    _shaders.reset();
    _textures.reset();
    _meshes.reset();
    _graphicsContext.reset();
    _window.reset();
}

} // namespace reone
