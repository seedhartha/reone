/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "textures.h"

#include "../resource/resources.h"
#include "../system/streamutil.h"

#include "image/curfile.h"
#include "image/tgafile.h"
#include "image/tpcfile.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace render {

static unordered_map<string, uint32_t> g_cursorNameByResRefKotor = {
    { "gui_mp_defaultu", 4 },
    { "gui_mp_defaultd", 5 }
};

static unordered_map<string, uint32_t> g_cursorNameByResRefTsl = {
    { "gui_mp_defaultu", 3 },
    { "gui_mp_defaultd", 4 }
};

Textures &Textures::instance() {
    static Textures instance;
    return instance;
}

void Textures::init(GameVersion version) {
    _version = version;
}

void Textures::invalidateCache() {
    _cache.clear();
}

shared_ptr<Texture> Textures::get(const string &resRef, TextureType type) {
    auto maybeTexture = _cache.find(resRef);
    if (maybeTexture != _cache.end()) {
        return maybeTexture->second;
    }
    auto inserted = _cache.insert(make_pair(resRef, doGet(resRef, type)));

    return inserted.first->second;
}

shared_ptr<Texture> Textures::doGet(const string &resRef, TextureType type) {
    bool tryCur = type == TextureType::Cursor;
    shared_ptr<Texture> texture;
    bool tryTpc = _version == GameVersion::TheSithLords || type != TextureType::Lightmap;

    if (tryCur) {
        uint32_t name;
        switch (_version) {
            case GameVersion::TheSithLords:
                name = g_cursorNameByResRefTsl.find(resRef)->second;
                break;
            default:
                name = g_cursorNameByResRefKotor.find(resRef)->second;
                break;
        }
        shared_ptr<ByteArray> curData(Resources::instance().findPeResource(name, PEResourceType::Cursor));
        if (curData) {
            CurFile cur(resRef);
            cur.load(wrap(curData));
            texture = cur.texture();
        }
    }
    if (!texture && tryTpc) {
        shared_ptr<ByteArray> tpcData(Resources::instance().findRaw(resRef, ResourceType::Texture));
        if (tpcData) {
            TpcFile tpc(resRef, type);
            tpc.load(wrap(tpcData));
            texture = tpc.texture();
        }
    }
    if (!texture) {
        shared_ptr<ByteArray> tgaData(Resources::instance().findRaw(resRef, ResourceType::Tga));
        if (tgaData) {
            TgaFile tga(resRef, type);
            tga.load(wrap(tgaData));
            texture = tga.texture();
        }
    }
    if (texture) {
        texture->initGL();
    }

    return move(texture);
}

} // namespace render

} // namespace reone
