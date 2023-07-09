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

#include "path.h"

namespace reone {

namespace resource {

class Gffs;
class Gff;

} // namespace resource

namespace game {

class IPaths {
public:
    virtual ~IPaths() = default;

    virtual void clear() = 0;

    virtual std::shared_ptr<Path> get(const std::string &key) = 0;
};

class Paths : public IPaths {
public:
    Paths(resource::Gffs &gffs) :
        _gffs(gffs) {
    }

    void clear() override {
        _objects.clear();
    }

    std::shared_ptr<Path> get(const std::string &key) override {
        auto maybeObject = _objects.find(key);
        if (maybeObject != _objects.end()) {
            return maybeObject->second;
        }
        auto object = doGet(key);
        return _objects.insert(make_pair(key, std::move(object))).first->second;
    }

private:
    resource::Gffs &_gffs;

    std::unordered_map<std::string, std::shared_ptr<Path>> _objects;

    std::shared_ptr<Path> doGet(std::string resRef);

    std::unique_ptr<Path> loadPath(const resource::Gff &pth) const;
};

} // namespace game

} // namespace reone
