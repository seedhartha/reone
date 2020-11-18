/*
 * Copyright (c) 2020 The reone project contributors
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

#include "../resource/resources.h"
#include "../common/streamutil.h"

#include "textures.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace render {

static unordered_map<string, string> g_fontOverride = {
    { "fnt_d16x16", "fnt_d16x16b" }
};

Fonts &Fonts::instance() {
    static Fonts instance;
    return instance;
}

void Fonts::invalidateCache() {
    _cache.clear();
}

shared_ptr<Font> Fonts::get(const string &resRef) {
    auto maybeTexture = _cache.find(resRef);
    if (maybeTexture != _cache.end()) {
        return maybeTexture->second;
    }
    auto inserted = _cache.insert(make_pair(resRef, doGet(resRef)));

    return inserted.first->second;
}

shared_ptr<Font> Fonts::doGet(const string &resRef) {
    auto maybeOverride = g_fontOverride.find(resRef);
    string finalResRef(maybeOverride != g_fontOverride.end() ? maybeOverride->second : resRef);
    shared_ptr<Texture> texture(Textures::instance().get(finalResRef, TextureType::GUI));
    shared_ptr<Font> font;

    if (texture) {
        font.reset(new Font());
        font->load(texture);
        if (font) {
            font->initGL();
        }
    }

    return move(font);
}

} // namespace render

} // namespace reone
