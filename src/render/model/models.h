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

#include <string>
#include <memory>
#include <unordered_map>

#include <boost/noncopyable.hpp>

#include "../../resource/types.h"

#include "../types.h"

#include "modelloader.h"

namespace reone {

namespace render {

class Model;

class Models : boost::noncopyable {
public:
    static Models &instance();

    void init(resource::GameID gameId);
    void invalidateCache();

    /**
     * Associates the specified model loader with the specified ResType.
     */
    void registerLoader(resource::ResourceType type, std::shared_ptr<IModelLoader> loader);

    std::shared_ptr<Model> get(const std::string &resRef, resource::ResourceType type = resource::ResourceType::Mdl);

private:
    resource::GameID _gameId { resource::GameID::KotOR };
    std::unordered_map<resource::ResourceType, std::shared_ptr<IModelLoader>> _loaders;
    std::unordered_map<std::string, std::shared_ptr<Model>> _cache;

    std::shared_ptr<Model> doGet(const std::string &resRef, resource::ResourceType type);
};

} // namespace render

} // namespace reone
