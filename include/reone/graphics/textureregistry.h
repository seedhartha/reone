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

enum class TextureName {
    Default2dRgb,
    DefaultArrayDepth,
    DefaultCubemapRgb,
    DefaultCubemapDepth,
    NoiseRg,
    SsaoRgb,
    SsrRgba
};

class ITextureRegistry {
public:
    virtual ~ITextureRegistry() = default;

    virtual Texture &get(TextureName name) = 0;
};

class TextureRegistry : public ITextureRegistry, boost::noncopyable {
public:
    ~TextureRegistry() {
        deinit();
    }

    void init();
    void deinit();

    Texture &get(TextureName name) override {
        auto texture = _nameToTexture.find(name);
        if (texture == _nameToTexture.end()) {
            throw std::runtime_error("Texture not found by name: " + std::to_string(static_cast<int>(name)));
        }
        return *texture->second;
    }

private:
    bool _inited {false};
    std::map<TextureName, std::shared_ptr<Texture>> _nameToTexture;

    void add(TextureName name, std::shared_ptr<Texture> texture) {
        _nameToTexture[name] = std::move(texture);
    }
};

} // namespace graphics

} // namespace reone
