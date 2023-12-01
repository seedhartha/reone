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

#include "reone/graphics/types.h"
#include "reone/system/binaryreader.h"
#include "reone/system/stream/input.h"

namespace reone {

namespace graphics {

class Texture;

}

namespace resource {

class TgaReader : boost::noncopyable {
public:
    TgaReader(IInputStream &tga, std::string resRef, graphics::TextureUsage usage) :
        _tga(BinaryReader(tga)),
        _resRef(std::move(resRef)),
        _usage(usage) {
    }

    void load();

    std::shared_ptr<graphics::Texture> texture() const { return _texture; }

private:
    BinaryReader _tga;
    std::string _resRef;
    graphics::TextureUsage _usage;

    graphics::TGADataType _dataType {graphics::TGADataType::RGBA};
    int _width {0};
    int _height {0};
    int _numLayers {0};
    bool _alpha {false};

    std::shared_ptr<graphics::Texture> _texture;

    void loadTexture();

    ByteBuffer readPixels(int w, int h);
    ByteBuffer readPixelsRLE(int w, int h);

    bool isRGBA() const;
    bool isGrayscale() const;
    bool isRLE() const;
};

} // namespace resource

} // namespace reone
