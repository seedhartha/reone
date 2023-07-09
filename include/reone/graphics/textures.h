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

    virtual void clear() = 0;

    virtual void bind(Texture &texture, int unit = TextureUnits::mainTex) = 0;
    virtual void bindBuiltIn() = 0;

    virtual std::shared_ptr<Texture> get(const std::string &resRef, TextureUsage usage = TextureUsage::Default) = 0;

    // Built-in

    virtual std::shared_ptr<Texture> default2DRGB() const = 0;
    virtual std::shared_ptr<Texture> defaultArrayDepth() const = 0;
    virtual std::shared_ptr<Texture> defaultCubemapRGB() const = 0;
    virtual std::shared_ptr<Texture> defaultCubemapDepth() const = 0;

    virtual std::shared_ptr<Texture> noiseRG() const = 0;
    virtual std::shared_ptr<Texture> ssaoRGB() const = 0;
    virtual std::shared_ptr<Texture> ssrRGBA() const = 0;

    // END Built-in
};

class Textures : public ITextures, boost::noncopyable {
public:
    Textures(GraphicsOptions &options, resource::Resources &resources) :
        _options(options),
        _resources(resources) {
    }

    void init();

    void clear() override;

    void bind(Texture &texture, int unit = TextureUnits::mainTex) override;
    void bindBuiltIn() override;

    std::shared_ptr<Texture> get(const std::string &resRef, TextureUsage usage = TextureUsage::Default) override;

    // Built-in

    std::shared_ptr<Texture> default2DRGB() const override { return _default2DRGB; }
    std::shared_ptr<Texture> defaultArrayDepth() const override { return _defaultArrayDepth; }
    std::shared_ptr<Texture> defaultCubemapRGB() const override { return _defaultCubemapRGB; }
    std::shared_ptr<Texture> defaultCubemapDepth() const override { return _defaultCubemapDepth; }

    std::shared_ptr<Texture> noiseRG() const override { return _noiseRG; }
    std::shared_ptr<Texture> ssaoRGB() const override { return _ssaoRGB; }
    std::shared_ptr<Texture> ssrRGBA() const override { return _ssrRGBA; }

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
