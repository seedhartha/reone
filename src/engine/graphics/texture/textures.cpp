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

#include "../../common/logutil.h"
#include "../../common/streamutil.h"
#include "../../resource/resources.h"

#include "../context.h"
#include "../services.h"
#include "../types.h"

#include "curreader.h"
#include "texture.h"
#include "textureutil.h"
#include "tgareader.h"
#include "tpcreader.h"
#include "txireader.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace graphics {

Textures::Textures(Context &context, Resources &resources) : _context(context), _resources(resources) {
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
    _context.setActiveTextureUnit(TextureUnits::diffuseMap);
    _default->bind();

    _context.setActiveTextureUnit(TextureUnits::lightmap);
    _default->bind();

    _context.setActiveTextureUnit(TextureUnits::environmentMap);
    _defaultCubemap->bind();

    _context.setActiveTextureUnit(TextureUnits::bumpMap);
    _default->bind();

    _context.setActiveTextureUnit(TextureUnits::bloom);
    _default->bind();

    _context.setActiveTextureUnit(TextureUnits::irradianceMap);
    _defaultCubemap->bind();

    _context.setActiveTextureUnit(TextureUnits::prefilterMap);
    _defaultCubemap->bind();

    _context.setActiveTextureUnit(TextureUnits::brdfLookup);
    _default->bind();

    _context.setActiveTextureUnit(TextureUnits::shadowMap);
    _default->bind();

    _context.setActiveTextureUnit(TextureUnits::shadowMapCube);
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

    shared_ptr<ByteArray> tgaData(_resources.getRaw(resRef, ResourceType::Tga, false));
    if (tgaData) {
        TgaReader tga(resRef, usage);
        tga.load(wrap(tgaData));
        texture = tga.texture();

        if (texture) {
            shared_ptr<ByteArray> txiData(_resources.getRaw(resRef, ResourceType::Txi, false));
            if (txiData) {
                TxiReader txi;
                txi.load(wrap(txiData));
                texture->setFeatures(txi.features());
            }
        }
    }

    if (!texture) {
        shared_ptr<ByteArray> tpcData(_resources.getRaw(resRef, ResourceType::Tpc, false));
        if (tpcData) {
            TpcReader tpc(resRef, usage);
            tpc.load(wrap(tpcData));
            texture = tpc.texture();
        }
    }

    if (!texture) {
        warn("Texture not found: " + resRef, LogChannels::graphics);
    }

    return move(texture);
}

} // namespace graphics

} // namespace reone
