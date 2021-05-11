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

#pragma once

#include "../../resource/format/binreader.h"

#include "texture.h"

namespace reone {

namespace graphics {

class TpcReader : public resource::BinaryReader {
public:
    /**
     * @param headless true if texture will not be used for rendering
     */
    TpcReader(const std::string &resRef, TextureUsage usage, bool headless = false);

    std::shared_ptr<Texture> texture() const { return _texture; }
    const ByteArray &txiData() const { return _txiData; }

private:
    enum class EncodingType {
        Grayscale = 1,
        RGB = 2,
        RGBA = 4
    };

    std::string _resRef;
    TextureUsage _usage;
    bool _headless;

    uint32_t _dataSize { 0 };
    bool _compressed { false };
    uint16_t _width { 0 };
    uint16_t _height { 0 };
    EncodingType _encoding { EncodingType::Grayscale };
    uint8_t _mipMapCount { 0 };
    bool _cubeMap { false };
    std::vector<Texture::Layer> _pixels;
    ByteArray _txiData;
    Texture::Features _features;

    std::shared_ptr<Texture> _texture;

    void doLoad() override;

    void loadPixels();
    void loadFeatures();

    void makeTexture();

    void getMipMapSize(int index, int &width, int &height) const;
    int getMipMapDataSize(int width, int height) const;
    PixelFormat getPixelFormat() const;
};

} // namespace graphics

} // namespace reone
