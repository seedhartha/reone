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

#include "types.h"

namespace reone {

namespace resource {

class Resources;

}

namespace graphics {

class Texture;

class Textures : boost::noncopyable {
public:
    Textures(resource::Resources &resources) :
        _resources(resources) {
    }

    void init();
    void invalidate();

    void bind(Texture &texture, int unit = TextureUnits::mainTex);
    void bindDefaults();

    std::shared_ptr<Texture> get(const std::string &resRef, TextureUsage usage = TextureUsage::Default);

    std::shared_ptr<Texture> defaultRGB() const { return _defaultRGB; }
    std::shared_ptr<Texture> defaultArrayDepth() const { return _defaultArrayDepth; }
    std::shared_ptr<Texture> defaultCubemapRGB() const { return _defaultCubemapRGB; }
    std::shared_ptr<Texture> defaultCubemapDepth() const { return _defaultCubemapDepth; }

private:
    int _activeUnit {0};

    resource::Resources &_resources;

    std::unordered_map<std::string, std::shared_ptr<Texture>> _cache;

    // Defaults

    std::shared_ptr<Texture> _defaultRGB;
    std::shared_ptr<Texture> _defaultArrayDepth;
    std::shared_ptr<Texture> _defaultCubemapRGB;
    std::shared_ptr<Texture> _defaultCubemapDepth;

    // END Defaults

    std::shared_ptr<Texture> doGet(const std::string &resRef, TextureUsage usage);
};

} // namespace graphics

} // namespace reone
