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

namespace reone {

namespace graphics {

class Texture;

class Material {
public:
    using TextureUnit = int;

    bool transparent() const {
        return _transparent;
    }

    void setTransparent(bool transparent) {
        _transparent = transparent;
    }

private:
    bool _transparent {false};
    std::unordered_map<TextureUnit, std::reference_wrapper<Texture>> _textures;
};

} // namespace graphics

} // namespace reone
