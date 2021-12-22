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
    // Initialize default texture
    _default = make_shared<Texture>("default", getTextureProperties(TextureUsage::Default));
    _default->clear(1, 1, PixelFormat::RGB);
    _default->init();

    // Initialize default cubemap texture
    _defaultCubemap = make_shared<Texture>("default_cubemap", getTextureProperties(TextureUsage::DefaultCubeMap));
    _defaultCubemap->clear(1, 1, PixelFormat::RGB);
    _defaultCubemap->init();

    // Initialize default array texture
    _defaultArray = make_shared<Texture>("default_array", getTextureProperties(TextureUsage::Default));
    _defaultArray->clear(1, 1, PixelFormat::RGB, kNumShadowCascades);
    _defaultArray->init();

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
    bind(*_default, TextureUnits::diffuseMap);
    bind(*_default, TextureUnits::lightmap);
    bind(*_defaultCubemap, TextureUnits::environmentMap);
    bind(*_default, TextureUnits::bumpMap);
    bind(*_default, TextureUnits::bloom);
    bind(*_defaultArray, TextureUnits::shadowMap);
    bind(*_defaultCubemap, TextureUnits::cubeShadowMap);
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
        texture->init();
    } else {
        warn("Texture not found: " + resRef, LogChannels::graphics);
    }
    return move(texture);
}

} // namespace graphics

} // namespace reone
