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

#include "services.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace graphics {

GraphicsServices::GraphicsServices(GraphicsOptions options, ResourceServices &resource) :
    _options(move(options)),
    _resource(resource) {
}

void GraphicsServices::init() {
    _window = make_unique<Window>(_options);
    _window->init();

    _context = make_unique<Context>();

    _meshes = make_unique<Meshes>();
    _meshes->init();

    _textures = make_unique<Textures>(*this, _resource);
    _textures->init();

    _materials = make_unique<Materials>(&_resource.resources());
    _materials->init();

    _models = make_unique<Models>(_textures.get(), &_resource.resources());
    _walkmeshes = make_unique<Walkmeshes>(&_resource.resources());
    _lips = make_unique<Lips>(&_resource.resources());

    _shaders = make_unique<Shaders>();
    _shaders->init();

    _pbrIbl = make_unique<PBRIBL>(*this);
    _pbrIbl->init();

    _fonts = make_unique<Fonts>(*this);
}

} // namespace graphics

} // namespace reone
