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

#include "id.h"
#include "provider.h"

namespace reone {

namespace resource {

typedef std::list<std::unique_ptr<IResourceProvider>> ResourceProviderList;

class IResources {
public:
    virtual ~IResources() = default;

    virtual void indexKEY(const std::filesystem::path &path) = 0;
    virtual void indexERF(const std::filesystem::path &path) = 0;
    virtual void indexRIM(const std::filesystem::path &path) = 0;
    virtual void indexEXE(const std::filesystem::path &path) = 0;
    virtual void indexFolder(const std::filesystem::path &path) = 0;

    virtual std::shared_ptr<ByteBuffer> get(const ResourceId &id) = 0;
    virtual std::shared_ptr<ByteBuffer> find(const ResourceId &id) = 0;
};

class Resources : public IResources, boost::noncopyable {
public:
    void clearProviders() {
        _providers.clear();
    }

    void addProvider(std::unique_ptr<IResourceProvider> provider) {
        _providers.push_front(std::move(provider));
    }

    void indexKEY(const std::filesystem::path &path) override;
    void indexERF(const std::filesystem::path &path) override;
    void indexRIM(const std::filesystem::path &path) override;
    void indexEXE(const std::filesystem::path &path) override;
    void indexFolder(const std::filesystem::path &path) override;

    std::shared_ptr<ByteBuffer> get(const ResourceId &id) override;
    std::shared_ptr<ByteBuffer> find(const ResourceId &id) override;

    const ResourceProviderList &providers() const { return _providers; }

private:
    ResourceProviderList _providers;
};

} // namespace resource

} // namespace reone
