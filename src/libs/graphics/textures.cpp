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

#include "reone/graphics/textures.h"

#include "reone/graphics/format/curreader.h"
#include "reone/graphics/format/tgareader.h"
#include "reone/graphics/format/tpcreader.h"
#include "reone/graphics/format/txireader.h"
#include "reone/graphics/options.h"
#include "reone/graphics/texture.h"
#include "reone/graphics/textureutil.h"
#include "reone/graphics/types.h"
#include "reone/resource/resources.h"
#include "reone/system/logutil.h"
#include "reone/system/randomutil.h"
#include "reone/system/stream/memoryinput.h"
#include "reone/system/threadutil.h"

using namespace reone::resource;

namespace reone {

namespace graphics {

void Textures::init() {
    checkMainThread();

    _default2DRGB = std::make_shared<Texture>("default_rgb", getTextureProperties(TextureUsage::Default));
    _default2DRGB->clear(1, 1, PixelFormat::RGB8);
    _default2DRGB->init();

    _defaultCubemapRGB = std::make_shared<Texture>("default_cubemap_rgb", getTextureProperties(TextureUsage::Default));
    _defaultCubemapRGB->setCubemap(true);
    _defaultCubemapRGB->clear(1, 1, PixelFormat::RGB8, kNumCubeFaces);
    _defaultCubemapRGB->init();

    _defaultCubemapDepth = std::make_shared<Texture>("default_cubemap_depth", getTextureProperties(TextureUsage::Default));
    _defaultCubemapDepth->setCubemap(true);
    _defaultCubemapDepth->clear(1, 1, PixelFormat::Depth32F, kNumCubeFaces);
    _defaultCubemapDepth->init();

    _defaultArrayDepth = std::make_shared<Texture>("default_array_depth", getTextureProperties(TextureUsage::Default));
    _defaultArrayDepth->clear(1, 1, PixelFormat::Depth32F, kNumShadowCascades);
    _defaultArrayDepth->init();

    auto noisePixels = std::make_shared<ByteBuffer>();
    noisePixels->resize(4 * 4 * 2 * sizeof(float));
    for (int i = 0; i < 4 * 4 * 2; ++i) {
        float *pixel = reinterpret_cast<float *>(&(*noisePixels)[4 * i]);
        *pixel = randomFloat(-1.0f, 1.0f);
    }
    auto noiseLayer = Texture::Layer {std::move(noisePixels)};
    _noiseRG = std::make_shared<Texture>("noise_rg", getTextureProperties(TextureUsage::Noise));
    _noiseRG->setPixels(4, 4, PixelFormat::RG16F, std::move(noiseLayer));
    _noiseRG->init();

    auto ssaoPixels = std::make_shared<ByteBuffer>();
    ssaoPixels->resize(3);
    (*ssaoPixels)[0] = 0xff;
    (*ssaoPixels)[1] = 0xff;
    (*ssaoPixels)[2] = 0xff;
    auto ssaoLayer = Texture::Layer {std::move(ssaoPixels)};
    _ssaoRGB = std::make_shared<Texture>("ssao_rgb", getTextureProperties(TextureUsage::Default));
    _ssaoRGB->setPixels(1, 1, PixelFormat::RGB8, std::move(ssaoLayer));
    _ssaoRGB->init();

    auto ssrPixels = std::make_shared<ByteBuffer>();
    ssrPixels->resize(4);
    (*ssrPixels)[0] = 0;
    (*ssrPixels)[1] = 0;
    (*ssrPixels)[2] = 0;
    (*ssrPixels)[3] = 0;
    auto ssrLayer = Texture::Layer {std::move(ssrPixels)};
    _ssrRGBA = std::make_shared<Texture>("ssr_rgba", getTextureProperties(TextureUsage::Default));
    _ssrRGBA->setPixels(1, 1, PixelFormat::RGBA8, std::move(ssrLayer));
    _ssrRGBA->init();

    bindBuiltIn();
}

void Textures::clear() {
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

std::shared_ptr<Texture> Textures::get(const std::string &resRef, TextureUsage usage) {
    if (resRef.empty()) {
        return nullptr;
    }
    auto maybeTexture = _cache.find(resRef);
    if (maybeTexture != _cache.end()) {
        return maybeTexture->second;
    }
    std::string lcResRef(boost::to_lower_copy(resRef));
    auto inserted = _cache.insert(std::make_pair(lcResRef, doGet(lcResRef, usage)));

    return inserted.first->second;
}

std::shared_ptr<Texture> Textures::doGet(const std::string &resRef, TextureUsage usage) {
    std::shared_ptr<Texture> texture;

    auto tgaRes = _resources.find(ResourceId(resRef, ResType::Tga));
    if (tgaRes) {
        auto tga = MemoryInputStream(tgaRes->data);
        auto tgaReader = TgaReader(tga, resRef, usage);
        tgaReader.load();
        texture = tgaReader.texture();

        if (texture) {
            auto txiRes = _resources.find(ResourceId(resRef, ResType::Txi));
            if (txiRes) {
                auto txi = MemoryInputStream(txiRes->data);
                auto txiReader = TxiReader();
                txiReader.load(txi);
                texture->setFeatures(txiReader.features());
            }
        }
    }

    if (!texture) {
        auto tpcRes = _resources.find(ResourceId(resRef, ResType::Tpc));
        if (tpcRes) {
            auto tpc = MemoryInputStream(tpcRes->data);
            auto tpcReader = TpcReader(tpc, resRef, usage);
            tpcReader.load();
            texture = tpcReader.texture();
        }
    }

    if (texture) {
        if (texture->isCubemap()) {
            prepareCubemap(*texture);
        }
        float anisotropy = std::max(1.0f, exp2f(_options.anisotropicFiltering));
        texture->setAnisotropy(anisotropy);
    } else {
        warn("Texture not found: " + resRef, LogChannel::Graphics);
    }

    return texture;
}

} // namespace graphics

} // namespace reone
