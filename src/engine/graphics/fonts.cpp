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

#include "fonts.h"

#include <stdexcept>

#include "../common/guardutil.h"
#include "../common/streamutil.h"
#include "../resource/resources.h"

#include "font.h"
#include "texture/textures.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::resource;

namespace reone {

namespace graphics {

static unordered_map<string, string> g_fontOverride = {
    { "fnt_d16x16", "fnt_d16x16b" }
};

Fonts::Fonts(Window *window, Shaders *shaders, Meshes *meshes, Textures *textures) :
    MemoryCache(bind(&Fonts::doGet, this, _1)),
    _window(window),
    _shaders(shaders),
    _meshes(meshes),
    _textures(textures) {

    ensureNotNull(window, "window");
    ensureNotNull(shaders, "shaders");
    ensureNotNull(meshes, "meshes");
    ensureNotNull(textures, "textures");
}

shared_ptr<Font> Fonts::doGet(string resRef) {
    auto maybeOverride = g_fontOverride.find(resRef);
    if (maybeOverride != g_fontOverride.end()) {
        resRef = maybeOverride->second;
    }
    shared_ptr<Texture> texture(_textures->get(resRef, TextureUsage::GUI));
    if (!texture) return nullptr;

    auto font = make_shared<Font>(_window, _shaders, _meshes);
    font->load(texture);

    return move(font);
}

} // namespace graphics

} // namespace reone
