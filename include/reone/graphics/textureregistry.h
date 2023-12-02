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

#include "texture.h"

namespace reone {

namespace graphics {

struct TextureName {
    static constexpr char default2dRgb[] = "default_2d_rgb";
    static constexpr char defaultArrayDepth[] = "default_array_depth";
    static constexpr char defaultCubemapRgb[] = "default_cubemap_rgb";
    static constexpr char defaultCubemapDepth[] = "default_cubemap_depth";
    static constexpr char noiseRg[] = "noise_rg";
    static constexpr char ssaoRgb[] = "ssao_rgb";
    static constexpr char ssrRgb[] = "ssr_rgb";
};

class ITextureRegistry {
public:
    virtual ~ITextureRegistry() = default;

    virtual Texture &get(const std::string &name) = 0;
};

class TextureRegistry : public ITextureRegistry, boost::noncopyable {
public:
    ~TextureRegistry() {
        deinit();
    }

    void init();
    void deinit();

    void add(std::string name, std::shared_ptr<Texture> texture) {
        _nameToTexture[name] = std::move(texture);
    }

    Texture &get(const std::string &name) override {
        auto texture = _nameToTexture.find(name);
        if (texture == _nameToTexture.end()) {
            throw std::runtime_error("Texture not found by name: " + name);
        }
        return *texture->second;
    }

private:
    bool _inited {false};
    std::map<std::string, std::shared_ptr<Texture>> _nameToTexture;
};

} // namespace graphics

} // namespace reone
