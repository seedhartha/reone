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

#include "reone/system/memorycache.h"

#include "2da.h"

namespace reone {

namespace resource {

class Resources;

class ITwoDas {
public:
    virtual ~ITwoDas() = default;
};

class TwoDas : public ITwoDas, public MemoryCache<std::string, TwoDa>, boost::noncopyable {
public:
    TwoDas(Resources &resources) :
        MemoryCache(bind(&TwoDas::doGet, this, std::placeholders::_1)),
        _resources(resources) {
    }

    void add(std::string resRef, std::shared_ptr<TwoDa> twoDa) {
        _objects[resRef] = std::move(twoDa);
    }

private:
    Resources &_resources;

    std::shared_ptr<TwoDa> doGet(const std::string &resRef);
};

} // namespace resource

} // namespace reone
