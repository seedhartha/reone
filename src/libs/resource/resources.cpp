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

#include "reone/resource/resources.h"

#include "reone/resource/exception/notfound.h"
#include "reone/resource/provider/erf.h"
#include "reone/resource/provider/exe.h"
#include "reone/resource/provider/folder.h"
#include "reone/resource/provider/keybif.h"
#include "reone/resource/provider/rim.h"

namespace reone {

namespace resource {

void Resources::addKEY(const std::filesystem::path &path) {
    auto provider = std::make_unique<KeyBifResourceProvider>(path);
    provider->init();
    _providers.push_front(ResourceProviderLocalPair {std::move(provider), false});
}

void Resources::addERF(const std::filesystem::path &path, bool local) {
    auto provider = std::make_unique<ErfResourceProvider>(path);
    provider->init();
    _providers.push_front(ResourceProviderLocalPair {std::move(provider), local});
}

void Resources::addRIM(const std::filesystem::path &path, bool local) {
    auto provider = std::make_unique<RimResourceProvider>(path);
    provider->init();
    _providers.push_front(ResourceProviderLocalPair {std::move(provider), local});
}

void Resources::addEXE(const std::filesystem::path &path) {
    auto provider = std::make_unique<ExeResourceProvider>(path);
    provider->init();
    _providers.push_front(ResourceProviderLocalPair {std::move(provider), false});
}

void Resources::addFolder(const std::filesystem::path &path) {
    auto provider = std::make_unique<Folder>(path);
    provider->init();
    _providers.push_front(ResourceProviderLocalPair {std::move(provider), false});
}

Resource Resources::get(const ResourceId &id) {
    auto data = find(id);
    if (!data) {
        throw ResourceNotFoundException(id.string());
    }
    return *data;
}

std::optional<Resource> Resources::find(const ResourceId &id) {
    for (auto &[provider, local] : _providers) {
        auto data = provider->findResourceData(id);
        if (data) {
            return Resource {*data, local};
        }
    }
    return std::nullopt;
}

} // namespace resource

} // namespace reone
