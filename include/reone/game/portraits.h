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

#include "portrait.h"

namespace reone {

namespace resource {

class TwoDAs;
class Textures;

} // namespace resource

namespace graphics {

class Texture;

}

namespace game {

class IPortraits {
public:
    virtual ~IPortraits() = default;

    virtual std::shared_ptr<graphics::Texture> getTextureByIndex(int index) const = 0;
    ;
    virtual std::shared_ptr<graphics::Texture> getTextureByAppearance(int appearance) const = 0;

    virtual const std::vector<Portrait> &portraits() const = 0;
};

class Portraits : public IPortraits, boost::noncopyable {
public:
    Portraits(resource::Textures &textures, resource::TwoDAs &twoDas) :
        _textures(textures), _twoDas(twoDas) {
    }

    void init();

    std::shared_ptr<graphics::Texture> getTextureByIndex(int index) const override;
    std::shared_ptr<graphics::Texture> getTextureByAppearance(int appearance) const override;

    const std::vector<Portrait> &portraits() const override { return _portraits; }

private:
    resource::Textures &_textures;
    resource::TwoDAs &_twoDas;

    std::vector<Portrait> _portraits;

    std::shared_ptr<graphics::Texture> getPortraitTexture(const Portrait &portrait) const;
};

} // namespace game

} // namespace reone
