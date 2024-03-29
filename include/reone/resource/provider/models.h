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

#include "../types.h"

namespace reone {

namespace graphics {

class IStatistic;
class Model;

} // namespace graphics

namespace resource {

class Resources;
class Textures;

class IModels {
public:
    virtual ~IModels() {
    }

    virtual std::shared_ptr<graphics::Model> get(const std::string &resRef) = 0;
};

class Models : public IModels, boost::noncopyable {
public:
    Models(Textures &textures,
           Resources &resources,
           graphics::IStatistic &statistic) :
        _textures(textures),
        _resources(resources),
        _statistic(statistic) {
    }

    void clear();

    std::shared_ptr<graphics::Model> get(const std::string &resRef) override;

private:
    Textures &_textures;
    Resources &_resources;
    graphics::IStatistic &_statistic;

    std::unordered_map<std::string, std::shared_ptr<graphics::Model>> _cache;

    std::shared_ptr<graphics::Model> doGet(const std::string &resRef);
};

} // namespace resource

} // namespace reone
