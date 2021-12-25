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

#include "../common/logutil.h"
#include "../common/streamutil.h"
#include "../resource/resources.h"

#include "format/curreader.h"
#include "format/tgareader.h"
#include "format/tpcreader.h"
#include "format/txireader.h"
#include "texture.h"
#include "textureutil.h"
#include "types.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace graphics {

void Textures::init() {
    _defaultRGB = make_shared<Texture>("default_rgb", getTextureProperties(TextureUsage::Default));
    _defaultRGB->clear(1, 1, PixelFormat::RGB);
    _defaultRGB->init();

    _defaultCubemapRGB = make_shared<Texture>("default_cubemap_rgb", getTextureProperties(TextureUsage::DefaultCubeMap));
    _defaultCubemapRGB->clear(1, 1, PixelFormat::RGB, kNumCubeFaces);
    _defaultCubemapRGB->init();

    _defaultCubemapDepth = make_shared<Texture>("default_cubemap_depth", getTextureProperties(TextureUsage::DefaultCubeMap));
    _defaultCubemapDepth->clear(1, 1, PixelFormat::Depth, kNumCubeFaces);
    _defaultCubemapDepth->init();

    _defaultArrayDepth = make_shared<Texture>("default_array_depth", getTextureProperties(TextureUsage::Default));
    _defaultArrayDepth->clear(1, 1, PixelFormat::Depth, kNumShadowCascades);
    _defaultArrayDepth->init();

    bindDefaults();
}

void Textures::invalidate() {
    _cache.clear();
}

void Textures::bind(Texture &texture, int unit) {
    if (_activeUnit != unit) {
        glActiveTexture(GL_TEXTURE0 + unit);
        _activeUnit = unit;
    }
    texture.bind();
}

void Textures::bindDefaults() {
    bind(*_defaultRGB, TextureUnits::diffuseMap);
    bind(*_defaultRGB, TextureUnits::lightmap);
    bind(*_defaultRGB, TextureUnits::bumpMap);
    bind(*_defaultRGB, TextureUnits::bloom);
    bind(*_defaultCubemapRGB, TextureUnits::environmentMap);
    bind(*_defaultCubemapDepth, TextureUnits::cubeShadowMap);
    bind(*_defaultArrayDepth, TextureUnits::shadowMap);
}

shared_ptr<Texture> Textures::get(const string &resRef, TextureUsage usage) {
    if (resRef.empty()) {
        return nullptr;
    }
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

    shared_ptr<ByteArray> tgaData(_resources.get(resRef, ResourceType::Tga, false));
    if (tgaData) {
        TgaReader tga(resRef, usage);
        tga.load(wrap(tgaData));
        texture = tga.texture();

        if (texture) {
            shared_ptr<ByteArray> txiData(_resources.get(resRef, ResourceType::Txi, false));
            if (txiData) {
                TxiReader txi;
                txi.load(wrap(txiData));
                texture->setFeatures(txi.features());
            }
        }
    }

    if (!texture) {
        shared_ptr<ByteArray> tpcData(_resources.get(resRef, ResourceType::Tpc, false));
        if (tpcData) {
            TpcReader tpc(resRef, usage);
            tpc.load(wrap(tpcData));
            texture = tpc.texture();
        }
    }

    if (texture) {
        if (texture->isCubemap()) {
            prepareCubemap(*texture);
        }
        texture->init();
    } else {
        warn("Texture not found: " + resRef, LogChannels::graphics);
    }

    return move(texture);
}

} // namespace graphics

} // namespace reone
