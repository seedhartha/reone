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

#include "textures.h"

#include <boost/algorithm/string.hpp>

#include "../common/log.h"
#include "../common/streamutil.h"
#include "../resource/resources.h"

#include "image/curfile.h"
#include "image/tgafile.h"
#include "image/tpcfile.h"
#include "image/txifile.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace render {

Textures &Textures::instance() {
    static Textures instance;
    return instance;
}

void Textures::init(GameID gameId) {
    _gameId = gameId;
}

void Textures::invalidateCache() {
    _cache.clear();
}

shared_ptr<Texture> Textures::get(const string &resRef, TextureType type) {
    auto maybeTexture = _cache.find(resRef);
    if (maybeTexture != _cache.end()) {
        return maybeTexture->second;
    }
    string lcResRef(boost::to_lower_copy(resRef));
    auto inserted = _cache.insert(make_pair(lcResRef, doGet(lcResRef, type)));

    return inserted.first->second;
}

shared_ptr<Texture> Textures::doGet(const string &resRef, TextureType type) {
    shared_ptr<Texture> texture;

    shared_ptr<ByteArray> tgaData(Resources::instance().get(resRef, ResourceType::Tga, false));
    if (tgaData) {
        TgaFile tga(resRef, type);
        tga.load(wrap(tgaData));
        texture = tga.texture();

        shared_ptr<ByteArray> txiData(Resources::instance().get(resRef, ResourceType::Txi, false));
        if (txiData) {
            TxiFile txi;
            txi.load(wrap(txiData));
            texture->setFeatures(txi.features());
        }
    }

    if (!texture) {
        shared_ptr<ByteArray> tpcData(Resources::instance().get(resRef, ResourceType::Tpc, false));
        if (tpcData) {
            TpcFile tpc(resRef, type);
            tpc.load(wrap(tpcData));
            texture = tpc.texture();
        }
    }

    if (!texture) {
        warn("Textures: not found: " + resRef);
    }

    return move(texture);
}

} // namespace render

} // namespace reone
