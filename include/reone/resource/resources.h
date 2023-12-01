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

#include "reone/system/types.h"

#include "container.h"
#include "id.h"
#include "resource.h"

namespace reone {

namespace resource {

struct ResourceContainerLocalPair {
    std::unique_ptr<IResourceContainer> provider;
    bool local {false};
};

using ResourceContainerList = std::list<ResourceContainerLocalPair>;

class IResources {
public:
    virtual ~IResources() = default;

    virtual void clear() = 0;
    virtual void clearLocal() = 0;

    virtual void addKEY(const std::filesystem::path &path) = 0;
    virtual void addERF(const std::filesystem::path &path, bool local = false) = 0;
    virtual void addRIM(const std::filesystem::path &path, bool local = false) = 0;
    virtual void addEXE(const std::filesystem::path &path) = 0;
    virtual void addFolder(const std::filesystem::path &path) = 0;

    virtual Resource get(const ResourceId &id) = 0;
    virtual std::optional<Resource> find(const ResourceId &id) = 0;
};

class Resources : public IResources, boost::noncopyable {
public:
    void clear() override {
        _containers.clear();
    }

    void clearLocal() override {
        auto toErase = std::remove_if(_containers.begin(), _containers.end(), [](auto &pair) {
            return pair.local;
        });
        _containers.erase(toErase, _containers.end());
    }

    void add(std::unique_ptr<IResourceContainer> provider, bool local = false) {
        _containers.push_front(ResourceContainerLocalPair {std::move(provider), local});
    }

    void addKEY(const std::filesystem::path &path) override;
    void addERF(const std::filesystem::path &path, bool local = false) override;
    void addRIM(const std::filesystem::path &path, bool local = false) override;
    void addEXE(const std::filesystem::path &path) override;
    void addFolder(const std::filesystem::path &path) override;

    Resource get(const ResourceId &id) override;
    std::optional<Resource> find(const ResourceId &id) override;

    const ResourceContainerList &containers() const { return _containers; }

private:
    ResourceContainerList _containers;
};

} // namespace resource

} // namespace reone
