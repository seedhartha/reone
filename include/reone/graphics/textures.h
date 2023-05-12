/*
 * Copyright (c) 2020-2022 The reone project contributors
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

class GraphicsOptions;
class Texture;

class ITextures {
public:
    virtual ~ITextures() {
    }

    virtual std::shared_ptr<Texture> get(const std::string &resRef, TextureUsage usage = TextureUsage::Default) = 0;
};

class Textures : public ITextures, boost::noncopyable {
public:
    Textures(GraphicsOptions &options, resource::Resources &resources) :
        _options(options),
        _resources(resources) {
    }

    void init();
    void invalidate();

    void bind(Texture &texture, int unit = TextureUnits::mainTex);
    void bindBuiltIn();

    std::shared_ptr<Texture> get(const std::string &resRef, TextureUsage usage = TextureUsage::Default) override;

    // Built-in

    std::shared_ptr<Texture> default2DRGB() const { return _default2DRGB; }
    std::shared_ptr<Texture> defaultArrayDepth() const { return _defaultArrayDepth; }
    std::shared_ptr<Texture> defaultCubemapRGB() const { return _defaultCubemapRGB; }
    std::shared_ptr<Texture> defaultCubemapDepth() const { return _defaultCubemapDepth; }

    std::shared_ptr<Texture> noiseRG() const { return _noiseRG; }
    std::shared_ptr<Texture> ssaoRGB() const { return _ssaoRGB; }
    std::shared_ptr<Texture> ssrRGBA() const { return _ssrRGBA; }

    // END Built-in

private:
    int _activeUnit {0};

    GraphicsOptions &_options;
    resource::Resources &_resources;

    std::unordered_map<std::string, std::shared_ptr<Texture>> _cache;

    // Built-in

    std::shared_ptr<Texture> _default2DRGB;
    std::shared_ptr<Texture> _defaultArrayDepth;
    std::shared_ptr<Texture> _defaultCubemapRGB;
    std::shared_ptr<Texture> _defaultCubemapDepth;

    std::shared_ptr<Texture> _noiseRG;
    std::shared_ptr<Texture> _ssaoRGB;
    std::shared_ptr<Texture> _ssrRGBA;

    // END Built-in

    std::shared_ptr<Texture> doGet(const std::string &resRef, TextureUsage usage);
};

} // namespace graphics

} // namespace reone
