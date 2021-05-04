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

#include "image/curreader.h"
#include "image/tgareader.h"
#include "image/tpcreader.h"
#include "image/txireader.h"
#include "stateutil.h"
#include "textureutil.h"
#include "types.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace graphics {

Textures &Textures::instance() {
    static Textures instance;
    return instance;
}

void Textures::init() {
    // Initialize default texture
    _default = make_shared<Texture>("default", getTextureProperties(TextureUsage::Default));
    _default->init();
    _default->bind();
    _default->clearPixels(1, 1, PixelFormat::RGB);

    // Initialize default cubemap texture
    _defaultCubemap = make_shared<Texture>("default_cubemap", getTextureProperties(TextureUsage::CubeMapDefault));
    _defaultCubemap->init();
    _defaultCubemap->bind();
    _defaultCubemap->clearPixels(1, 1, PixelFormat::RGB);
}

void Textures::invalidateCache() {
    _cache.clear();
}

void Textures::bindDefaults() {
    setActiveTextureUnit(TextureUnits::diffuse);
    _default->bind();

    setActiveTextureUnit(TextureUnits::lightmap);
    _default->bind();

    setActiveTextureUnit(TextureUnits::envmap);
    _defaultCubemap->bind();

    setActiveTextureUnit(TextureUnits::bumpmap);
    _default->bind();

    setActiveTextureUnit(TextureUnits::bloom);
    _default->bind();

    setActiveTextureUnit(TextureUnits::irradianceMap);
    _defaultCubemap->bind();

    setActiveTextureUnit(TextureUnits::prefilterMap);
    _defaultCubemap->bind();

    setActiveTextureUnit(TextureUnits::brdfLookup);
    _default->bind();

    setActiveTextureUnit(TextureUnits::shadowMap);
    _default->bind();

    setActiveTextureUnit(TextureUnits::cubeShadowMap);
    _defaultCubemap->bind();
}

shared_ptr<Texture> Textures::get(const string &resRef, TextureUsage usage) {
    if (resRef.empty()) return nullptr;

    auto maybeTexture = _cache.find(resRef);
    if (maybeTexture != _cache.end()) {
        return maybeTexture->second;
    }
    string lcResRef(boost::to_lower_copy(resRef));
    auto inserted = _cache.insert(make_pair(lcResRef, doGet(lcResRef, usage)));

    return inserted.first->second;
}

shared_ptr<Texture> Textures::doGet(const string &resRef, TextureUsage usage) {
    shared_ptr<Texture> texture;

    shared_ptr<ByteArray> tgaData(Resources::instance().getRaw(resRef, ResourceType::Tga, false));
    if (tgaData) {
        TgaReader tga(resRef, usage);
        tga.load(wrap(tgaData));
        texture = tga.texture();

        if (texture) {
            shared_ptr<ByteArray> txiData(Resources::instance().getRaw(resRef, ResourceType::Txi, false));
            if (txiData) {
                TxiReader txi;
                txi.load(wrap(txiData));
                texture->setFeatures(txi.features());
            }
        }
    }

    if (!texture) {
        shared_ptr<ByteArray> tpcData(Resources::instance().getRaw(resRef, ResourceType::Tpc, false));
        if (tpcData) {
            TpcReader tpc(resRef, usage);
            tpc.load(wrap(tpcData));
            texture = tpc.texture();
        }
    }

    if (!texture) {
        warn("Texture not found: " + resRef);
    }

    return move(texture);
}

} // namespace graphics

} // namespace reone
