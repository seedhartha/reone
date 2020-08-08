/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "../render/texture.h"

#include "binfile.h"

namespace reone {

namespace resources {

class TpcFile : public BinaryFile {
public:
    TpcFile(const std::string &resRef, render::TextureType type);
    std::shared_ptr<render::Texture> texture() const;

private:
    enum class EncodingType {
        Grayscale = 1,
        RGB = 2,
        RGBA = 4
    };

    std::string _resRef;
    render::TextureType _type;
    uint32_t _dataSize { 0 };
    bool _compressed { false };
    uint16_t _width { 0 };
    uint16_t _height { 0 };
    EncodingType _encoding { EncodingType::Grayscale };
    uint8_t _mipMapCount { 0 };
    bool _cubeMap { false };
    std::shared_ptr<render::Texture> _texture;

    void doLoad() override;
    void getMipMapSize(int index, int &width, int &height) const;
    int getMipMapDataSize(int width, int height) const;
    void loadTexture();
    render::PixelFormat pixelFormat() const;
};

} // namespace resources

} // namespace reone
