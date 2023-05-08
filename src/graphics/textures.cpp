/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "reone/common/logutil.h"
#include "reone/common/randomutil.h"
#include "reone/common/stream/bytearrayinput.h"
#include "reone/resource/resources.h"

#include "format/curreader.h"
#include "format/tgareader.h"
#include "format/tpcreader.h"
#include "format/txireader.h"
#include "options.h"
#include "texture.h"
#include "textureutil.h"
#include "types.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace graphics {

void Textures::init() {
    _default2DRGB = make_shared<Texture>("default_rgb", getTextureProperties(TextureUsage::Default));
    _default2DRGB->clear(1, 1, PixelFormat::RGB8);
    _default2DRGB->init();

    _defaultCubemapRGB = make_shared<Texture>("default_cubemap_rgb", getTextureProperties(TextureUsage::Default));
    _defaultCubemapRGB->setCubemap(true);
    _defaultCubemapRGB->clear(1, 1, PixelFormat::RGB8, kNumCubeFaces);
    _defaultCubemapRGB->init();

    _defaultCubemapDepth = make_shared<Texture>("default_cubemap_depth", getTextureProperties(TextureUsage::Default));
    _defaultCubemapDepth->setCubemap(true);
    _defaultCubemapDepth->clear(1, 1, PixelFormat::Depth32F, kNumCubeFaces);
    _defaultCubemapDepth->init();

    _defaultArrayDepth = make_shared<Texture>("default_array_depth", getTextureProperties(TextureUsage::Default));
    _defaultArrayDepth->clear(1, 1, PixelFormat::Depth32F, kNumShadowCascades);
    _defaultArrayDepth->init();

    auto noisePixels = make_shared<ByteArray>();
    noisePixels->resize(4 * 4 * 2 * sizeof(float));
    for (int i = 0; i < 4 * 4 * 2; ++i) {
        float *pixel = reinterpret_cast<float *>(&(*noisePixels)[4 * i]);
        *pixel = random(-1.0f, 1.0f);
    }
    auto noiseLayer = Texture::Layer {move(noisePixels)};
    _noiseRG = make_shared<Texture>("noise_rg", getTextureProperties(TextureUsage::Noise));
    _noiseRG->setPixels(4, 4, PixelFormat::RG16F, move(noiseLayer));
    _noiseRG->init();

    auto ssaoPixels = make_shared<ByteArray>();
    ssaoPixels->resize(3);
    (*ssaoPixels)[0] = 0xff;
    (*ssaoPixels)[1] = 0xff;
    (*ssaoPixels)[2] = 0xff;
    auto ssaoLayer = Texture::Layer {move(ssaoPixels)};
    _ssaoRGB = make_shared<Texture>("ssao_rgb", getTextureProperties(TextureUsage::Default));
    _ssaoRGB->setPixels(1, 1, PixelFormat::RGB8, move(ssaoLayer));
    _ssaoRGB->init();

    auto ssrPixels = make_shared<ByteArray>();
    ssrPixels->resize(4);
    (*ssrPixels)[0] = 0;
    (*ssrPixels)[1] = 0;
    (*ssrPixels)[2] = 0;
    (*ssrPixels)[3] = 0;
    auto ssrLayer = Texture::Layer {move(ssrPixels)};
    _ssrRGBA = make_shared<Texture>("ssr_rgba", getTextureProperties(TextureUsage::Default));
    _ssrRGBA->setPixels(1, 1, PixelFormat::RGBA8, move(ssrLayer));
    _ssrRGBA->init();

    bindBuiltIn();
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

void Textures::bindBuiltIn() {
    bind(*_default2DRGB, TextureUnits::mainTex);
    bind(*_default2DRGB, TextureUnits::lightmap);
    bind(*_default2DRGB, TextureUnits::environmentMap);
    bind(*_default2DRGB, TextureUnits::bumpMap);
    bind(*_default2DRGB, TextureUnits::envmapColor);
    bind(*_default2DRGB, TextureUnits::selfIllumColor);
    bind(*_default2DRGB, TextureUnits::features);
    bind(*_default2DRGB, TextureUnits::eyePos);
    bind(*_default2DRGB, TextureUnits::eyeNormal);
    bind(*_default2DRGB, TextureUnits::hilights);
    bind(*_default2DRGB, TextureUnits::oitAccum);
    bind(*_default2DRGB, TextureUnits::oitRevealage);
    bind(*_defaultCubemapRGB, TextureUnits::environmentMapCube);
    bind(*_defaultCubemapDepth, TextureUnits::shadowMapCube);
    bind(*_defaultArrayDepth, TextureUnits::shadowMapArray);

    bind(*_noiseRG, TextureUnits::noise);
    bind(*_ssaoRGB, TextureUnits::ssao);
    bind(*_ssrRGBA, TextureUnits::ssr);
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

    auto tgaData = _resources.get(resRef, ResourceType::Tga, false);
    if (tgaData) {
        auto tga = ByteArrayInputStream(*tgaData);
        auto tgaReader = TgaReader(resRef, usage);
        tgaReader.load(tga);
        texture = tgaReader.texture();

        if (texture) {
            auto txiData = _resources.get(resRef, ResourceType::Txi, false);
            if (txiData) {
                auto txi = ByteArrayInputStream(*txiData);
                auto txiReader = TxiReader();
                txiReader.load(txi);
                texture->setFeatures(txiReader.features());
            }
        }
    }

    if (!texture) {
        auto tpcData = _resources.get(resRef, ResourceType::Tpc, false);
        if (tpcData) {
            auto tpc = ByteArrayInputStream(*tpcData);
            auto tpcReader = TpcReader(resRef, usage);
            tpcReader.load(tpc);
            texture = tpcReader.texture();
        }
    }

    if (texture) {
        if (texture->isCubemap()) {
            prepareCubemap(*texture);
        }
        float anisotropy = max(1.0f, exp2f(_options.anisotropicFiltering));
        texture->setAnisotropy(anisotropy);
        texture->init();
    } else {
        warn("Texture not found: " + resRef, LogChannels::graphics);
    }

    return move(texture);
}

} // namespace graphics

} // namespace reone
