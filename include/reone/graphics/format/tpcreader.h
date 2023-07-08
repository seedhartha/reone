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

#pragma once

#include "reone/system/binaryreader.h"
#include "reone/system/stream/input.h"

#include "../texture.h"

namespace reone {

namespace graphics {

class TpcReader : boost::noncopyable {
public:
    TpcReader(IInputStream &tpc, std::string resRef, TextureUsage usage) :
        _tpc(BinaryReader(tpc)),
        _resRef(std::move(resRef)),
        _usage(usage) {
    }

    void load();

    std::shared_ptr<Texture> texture() const { return _texture; }
    const ByteArray &txiData() const { return _txiData; }

private:
    enum class EncodingType {
        Grayscale = 1,
        RGB = 2,
        RGBA = 4
    };

    BinaryReader _tpc;
    std::string _resRef;
    TextureUsage _usage;

    uint32_t _dataSize {0};
    uint16_t _width {0};
    uint16_t _height {0};
    EncodingType _encoding {EncodingType::Grayscale};
    bool _compressed {false};
    int _numLayers {0};
    uint8_t _numMipMaps {0};

    std::vector<Texture::Layer> _layers;
    Texture::Features _features;

    std::shared_ptr<Texture> _texture;
    ByteArray _txiData;

    void loadLayers();
    void loadFeatures();

    void loadTexture();

    void getMipMapSize(int index, int &width, int &height) const;
    int getMipMapDataSize(int width, int height) const;

    PixelFormat getPixelFormat() const;
};

} // namespace graphics

} // namespace reone
