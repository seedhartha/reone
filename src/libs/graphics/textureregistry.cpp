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

#include "reone/graphics/textureregistry.h"

#include "reone/graphics/textureutil.h"
#include "reone/system/randomutil.h"

namespace reone {

namespace graphics {

void TextureRegistry::init() {
    if (_inited) {
        return;
    }

    auto default2DRGB = std::make_shared<Texture>(
        "default_rgb",
        TextureType::TwoDim,
        getTextureProperties(TextureUsage::Default));
    default2DRGB->clear(1, 1, PixelFormat::RGB8);
    default2DRGB->init();
    add(TextureName::default2dRgb, std::move(default2DRGB));

    auto defaultCubemapRGB = std::make_shared<Texture>(
        "default_cubemap_rgb",
        TextureType::CubeMap,
        getTextureProperties(TextureUsage::Default));
    defaultCubemapRGB->clear(1, 1, PixelFormat::RGB8, kNumCubeFaces);
    defaultCubemapRGB->init();
    add(TextureName::defaultCubemapRgb, std::move(defaultCubemapRGB));

    auto defaultCubemapDepth = std::make_shared<Texture>(
        "default_cubemap_depth",
        TextureType::CubeMap,
        getTextureProperties(TextureUsage::Default));
    defaultCubemapDepth->clear(1, 1, PixelFormat::Depth32F, kNumCubeFaces);
    defaultCubemapDepth->init();
    add(TextureName::defaultCubemapDepth, std::move(defaultCubemapDepth));

    auto defaultArrayDepth = std::make_shared<Texture>(
        "default_array_depth",
        TextureType::TwoDimArray,
        getTextureProperties(TextureUsage::Default));
    defaultArrayDepth->clear(1, 1, PixelFormat::Depth32F, kNumShadowCascades);
    defaultArrayDepth->init();
    add(TextureName::defaultArrayDepth, std::move(defaultArrayDepth));

    auto noisePixels = std::make_shared<ByteBuffer>();
    noisePixels->resize(4 * 4 * 2 * sizeof(float));
    for (int i = 0; i < 4 * 4 * 2; ++i) {
        float *pixel = reinterpret_cast<float *>(&(*noisePixels)[4 * i]);
        *pixel = randomFloat(-1.0f, 1.0f);
    }
    auto noiseLayer = Texture::Layer {std::move(noisePixels)};
    auto noiseRG = std::make_shared<Texture>(
        "noise_rg",
        TextureType::TwoDim,
        getTextureProperties(TextureUsage::Noise));
    noiseRG->setPixels(4, 4, PixelFormat::RG16F, std::move(noiseLayer));
    noiseRG->init();
    add(TextureName::noiseRg, std::move(noiseRG));

    auto ssaoPixels = std::make_shared<ByteBuffer>();
    ssaoPixels->resize(3);
    (*ssaoPixels)[0] = 0xff;
    (*ssaoPixels)[1] = 0xff;
    (*ssaoPixels)[2] = 0xff;
    auto ssaoLayer = Texture::Layer {std::move(ssaoPixels)};
    auto ssaoRGB = std::make_shared<Texture>(
        "ssao_rgb",
        TextureType::TwoDim,
        getTextureProperties(TextureUsage::Default));
    ssaoRGB->setPixels(1, 1, PixelFormat::RGB8, std::move(ssaoLayer));
    ssaoRGB->init();
    add(TextureName::ssaoRgb, std::move(ssaoRGB));

    auto ssrPixels = std::make_shared<ByteBuffer>();
    ssrPixels->resize(4);
    (*ssrPixels)[0] = 0;
    (*ssrPixels)[1] = 0;
    (*ssrPixels)[2] = 0;
    (*ssrPixels)[3] = 0;
    auto ssrLayer = Texture::Layer {std::move(ssrPixels)};
    auto ssrRGBA = std::make_shared<Texture>(
        "ssr_rgba",
        TextureType::TwoDim,
        getTextureProperties(TextureUsage::Default));
    ssrRGBA->setPixels(1, 1, PixelFormat::RGBA8, std::move(ssrLayer));
    ssrRGBA->init();
    add(TextureName::ssrRgb, std::move(ssrRGBA));

    _inited = true;
}

void TextureRegistry::deinit() {
    if (!_inited) {
        return;
    }
    _nameToTexture.clear();
    _inited = false;
}

} // namespace graphics

} // namespace reone
