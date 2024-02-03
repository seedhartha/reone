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

#include "reone/resource/provider/textures.h"

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
#include "reone/system/stream/memoryinput.h"
#include "reone/system/threadutil.h"

using namespace reone::graphics;

namespace reone {

namespace resource {

void Textures::init() {
}

void Textures::clear() {
    _cache.clear();
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
    std::optional<Texture::Features> features;

    auto txiRes = _resources.find(ResourceId(resRef, ResType::Txi));
    if (txiRes) {
        auto txi = MemoryInputStream(txiRes->data);
        auto txiReader = TxiReader();
        txiReader.load(txi);
        features = txiReader.features();
    }

    auto tgaRes = _resources.find(ResourceId(resRef, ResType::Tga));
    if (tgaRes) {
        auto tga = MemoryInputStream(tgaRes->data);
        auto tgaReader = TgaReader(tga, resRef, usage);
        tgaReader.load();
        texture = tgaReader.texture();
        if (texture && features) {
            texture->setFeatures(*features);
        }
    }

    if (!texture) {
        auto tpcRes = _resources.find(ResourceId(resRef, ResType::Tpc));
        if (tpcRes) {
            auto tpc = MemoryInputStream(tpcRes->data);
            auto tpcReader = TpcReader(tpc, resRef, usage);
            tpcReader.load();
            texture = tpcReader.texture();
            if (texture) {
                if (features) {
                    texture->setFeatures(*features);
                } else {
                    features = texture->features();
                }
            }
        }
    }

    if (texture) {
        if (features &&
            features->procedureType != Texture::ProcedureType::Invalid &&
            (features->numX > 1 || features->numY > 1)) {
            convertGridTextureToArray(*texture, features->numX, features->numY);
        }
        float anisotropy = std::max(1.0f, exp2f(_options.anisotropicFiltering));
        texture->setAnisotropy(anisotropy);
        texture->init();
    } else {
        warn("Texture not found: " + resRef, LogChannel::Graphics);
    }

    return texture;
}

} // namespace resource

} // namespace reone
