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
    _features = make_unique<Features>(_options);
    _window = make_unique<Window>(_options);
    _context = make_unique<Context>();
    _meshes = make_unique<Meshes>();
    _textures = make_unique<Textures>(*_context, _resource.resources());
    _materials = make_unique<Materials>(_resource.twoDas());
    _models = make_unique<Models>(*_textures, _resource.resources());
    _walkmeshes = make_unique<Walkmeshes>(_resource.resources());
    _lips = make_unique<Lips>(_resource.resources());
    _shaders = make_unique<Shaders>();
    _pbrIbl = make_unique<PBRIBL>(*_context, *_meshes, *_shaders);
    _fonts = make_unique<Fonts>(*_window, *_context, *_meshes, *_textures, *_shaders);

    _features->init();
    _window->init();
    _context->init();
    _meshes->init();
    _textures->init();
    _materials->init();
    _shaders->init();
    _pbrIbl->init();
}

void GraphicsModule::deinit() {
    _pbrIbl.reset();
    _shaders.reset();
    _materials.reset();
    _textures.reset();
    _meshes.reset();
    _context.reset();
    _window.reset();
    _features.reset();
}

} // namespace reone
