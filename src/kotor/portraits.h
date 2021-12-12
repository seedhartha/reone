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

#include "../game/portrait.h"
#include "../game/portraits.h"

namespace reone {

namespace resource {

class TwoDas;

}

namespace graphics {

class Textures;

}

namespace kotor {

class Portraits : public game::IPortraits {
public:
    Portraits(graphics::Textures &textures, resource::TwoDas &twoDas) :
        _textures(textures), _twoDas(twoDas) {
    }

    void init();

    std::shared_ptr<graphics::Texture> getTextureByIndex(int index) const override;
    std::shared_ptr<graphics::Texture> getTextureByAppearance(int appearance) const override;

    const std::vector<game::Portrait> &portraits() const override { return _portraits; }

private:
    graphics::Textures &_textures;
    resource::TwoDas &_twoDas;

    std::vector<game::Portrait> _portraits;

    std::shared_ptr<graphics::Texture> getPortraitTexture(const game::Portrait &portrait) const;
};

} // namespace kotor

} // namespace reone
