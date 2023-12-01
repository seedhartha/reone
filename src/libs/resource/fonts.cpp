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

#include "reone/resource/fonts.h"

#include "reone/graphics/context.h"
#include "reone/graphics/meshes.h"
#include "reone/graphics/shaders.h"
#include "reone/graphics/window.h"
#include "reone/resource/textures.h"

using namespace reone::graphics;

namespace reone {

namespace resource {

static std::unordered_map<std::string, std::string> g_fontOverride = {
    {"fnt_d16x16", "fnt_d16x16b"}};

std::shared_ptr<Font> Fonts::doGet(std::string resRef) {
    auto maybeOverride = g_fontOverride.find(resRef);
    if (maybeOverride != g_fontOverride.end()) {
        resRef = maybeOverride->second;
    }
    std::shared_ptr<Texture> texture(_textures.get(resRef, TextureUsage::Font));
    if (!texture)
        return nullptr;

    auto font = std::make_shared<Font>(_graphicsContext, _meshes, _shaders, _uniforms);
    font->load(texture);

    return font;
}

} // namespace resource

} // namespace reone
