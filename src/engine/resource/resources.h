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

#include "../common/types.h"

#include "format/pereader.h"
#include "id.h"
#include "resourceprovider.h"
#include "types.h"

namespace reone {

namespace resource {

/**
 * Encapsulates game resource management. Contains a prioritized list of
 * resource providers, that it queries for resources by ResRef and ResType.
 * Caches found resources.
 */
class Resources : boost::noncopyable {
public:
    Resources() = default;

    void indexKeyFile(const boost::filesystem::path &path);
    void indexErfFile(const boost::filesystem::path &path, bool transient = false);
    void indexRimFile(const boost::filesystem::path &path, bool transient = false);
    void indexDirectory(const boost::filesystem::path &path);
    void indexExeFile(const boost::filesystem::path &path);

    void invalidateCache();
    void clearTransientProviders();

    std::shared_ptr<ByteArray> getRaw(const std::string &resRef, ResourceType type, bool logNotFound = true);
    std::shared_ptr<ByteArray> getFromExe(uint32_t name, PEResourceType type);

private:
    std::unordered_map<ResourceId, std::shared_ptr<ByteArray>, ResourceIdHasher> _cache;

    // Providers

    PEReader _exeFile;
    std::vector<std::unique_ptr<IResourceProvider>> _providers;
    std::vector<std::unique_ptr<IResourceProvider>> _transientProviders; /**< transient providers are replaced when switching between modules */

    // END Providers

    void indexProvider(std::unique_ptr<IResourceProvider> &&provider, const boost::filesystem::path &path, bool transient = false);

    std::shared_ptr<ByteArray> doGetRaw(const ResourceId &id, const std::vector<std::unique_ptr<IResourceProvider>> &providers);
};

} // namespace resource

} // namespace reone
