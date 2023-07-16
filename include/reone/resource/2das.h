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

#include "reone/system/cache.h"

#include "2da.h"

namespace reone {

namespace resource {

class Resources;

class ITwoDas {
public:
    virtual ~ITwoDas() = default;

    virtual void clear() = 0;

    virtual std::shared_ptr<TwoDa> get(const std::string &key) = 0;
};

class TwoDas : public ITwoDas, boost::noncopyable {
public:
    TwoDas(Resources &resources) :
        _resources(resources) {
    }

    void clear() override {
        _cache.clear();
    }

    std::shared_ptr<TwoDa> get(const std::string &key) override;

private:
    Resources &_resources;

    Cache<std::string, TwoDa> _cache;
};

} // namespace resource

} // namespace reone
