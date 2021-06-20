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

#include "../types.h"

namespace reone {

namespace resource {

class Resources;

}

namespace graphics {

class Context;
class Texture;

class Textures : boost::noncopyable {
public:
    Textures(Context &context, resource::Resources &resources);

    void init();
    void invalidateCache();

    /**
     * Binds default textures to all texture units. Call once per framebuffer.
     */
    void bindDefaults();

    std::shared_ptr<Texture> get(const std::string &resRef, TextureUsage usage = TextureUsage::Default);

private:
    Context &_context;
    resource::Resources &_resources;

    std::shared_ptr<graphics::Texture> _default;
    std::shared_ptr<graphics::Texture> _defaultCubemap;
    std::unordered_map<std::string, std::shared_ptr<Texture>> _cache;

    std::shared_ptr<Texture> doGet(const std::string &resRef, TextureUsage usage);
};

} // namespace graphics

} // namespace reone
