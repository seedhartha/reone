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

namespace reone {

namespace graphics {

class GraphicsOptions;
class Texture;

} // namespace graphics

namespace resource {

class Resources;

class ITextures {
public:
    virtual ~ITextures() {
    }

    virtual void clear() = 0;

    virtual std::shared_ptr<graphics::Texture> get(const std::string &resRef, graphics::TextureUsage usage = graphics::TextureUsage::Default) = 0;
};

class Textures : public ITextures, boost::noncopyable {
public:
    Textures(graphics::GraphicsOptions &options, Resources &resources) :
        _options(options),
        _resources(resources) {
    }

    void init();

    void clear() override;

    std::shared_ptr<graphics::Texture> get(const std::string &resRef, graphics::TextureUsage usage = graphics::TextureUsage::Default) override;

private:
    int _activeUnit {0};

    graphics::GraphicsOptions &_options;
    Resources &_resources;

    std::unordered_map<std::string, std::shared_ptr<graphics::Texture>> _cache;
    std::mutex _mutex;

    std::shared_ptr<graphics::Texture> doGet(const std::string &resRef, graphics::TextureUsage usage);
};

} // namespace resource

} // namespace reone
