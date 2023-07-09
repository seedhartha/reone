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

#include "reone/system/stream/fileinput.h"
#include "reone/system/types.h"

#include "format/pereader.h"
#include "id.h"
#include "provider.h"
#include "types.h"

namespace reone {

namespace resource {

class IResources {
public:
    virtual ~IResources() = default;

    virtual void clearTransientProviders() = 0;

    virtual void indexKeyFile(const boost::filesystem::path &path) = 0;
    virtual void indexErfFile(const boost::filesystem::path &path, bool transient = false) = 0;
    virtual void indexRimFile(const boost::filesystem::path &path, bool transient = false) = 0;
    virtual void indexDirectory(const boost::filesystem::path &path) = 0;
    virtual void indexExeFile(const boost::filesystem::path &path) = 0;

    virtual std::shared_ptr<ByteBuffer> get(const std::string &resRef, ResourceType type, bool logNotFound = true) = 0;
};

class Resources : public IResources, boost::noncopyable {
public:
    typedef std::vector<std::unique_ptr<IResourceProvider>> ProviderList;

    void indexKeyFile(const boost::filesystem::path &path) override;
    void indexErfFile(const boost::filesystem::path &path, bool transient = false) override;
    void indexRimFile(const boost::filesystem::path &path, bool transient = false) override;
    void indexDirectory(const boost::filesystem::path &path) override;
    void indexExeFile(const boost::filesystem::path &path) override;

    void indexProvider(std::unique_ptr<IResourceProvider> &&provider, const boost::filesystem::path &path, bool transient = false);

    void clearAllProviders();
    void clearTransientProviders() override;

    std::shared_ptr<ByteBuffer> get(const std::string &resRef, ResourceType type, bool logNotFound = true) override;
    std::shared_ptr<ByteBuffer> getFromExe(uint32_t name, PEResourceType type);

    const ProviderList &providers() const { return _providers; }
    const ProviderList &transientProviders() const { return _transientProviders; }

private:
    boost::filesystem::path _exePath;
    ProviderList _providers;
    ProviderList _transientProviders; /**< transient providers are replaced when switching between modules */

    std::shared_ptr<ByteBuffer> getFromProviders(const ResourceId &id, const ProviderList &providers);
};

} // namespace resource

} // namespace reone
