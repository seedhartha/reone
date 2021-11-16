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

class Model;
class Textures;

class Models : boost::noncopyable {
public:
    Models(Textures &textures, resource::Resources &resources);

    void invalidate();

    std::shared_ptr<Model> get(const std::string &resRef);

private:
    Textures &_textures;
    resource::Resources &_resources;

    std::unordered_map<std::string, std::shared_ptr<Model>> _cache;

    std::shared_ptr<Model> doGet(const std::string &resRef);
};

} // namespace graphics

} // namespace reone
