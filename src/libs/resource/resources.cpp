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

void Resources::indexKEY(const std::filesystem::path &path) {
    auto provider = std::make_unique<KeyBifResourceProvider>(path);
    provider->init();
    _providers.push_front(std::move(provider));
}

void Resources::indexERF(const std::filesystem::path &path) {
    auto provider = std::make_unique<ErfResourceProvider>(path);
    provider->init();
    _providers.push_front(std::move(provider));
}

void Resources::indexRIM(const std::filesystem::path &path) {
    auto provider = std::make_unique<RimResourceProvider>(path);
    provider->init();
    _providers.push_front(std::move(provider));
}

void Resources::indexEXE(const std::filesystem::path &path) {
    auto provider = std::make_unique<ExeResourceProvider>(path);
    provider->init();
    _providers.push_front(std::move(provider));
}

void Resources::indexFolder(const std::filesystem::path &path) {
    auto provider = std::make_unique<Folder>(path);
    provider->init();
    _providers.push_front(std::move(provider));
}

std::shared_ptr<ByteBuffer> Resources::get(const ResourceId &id) {
    auto data = find(id);
    if (!data) {
        throw ResourceNotFoundException(id.string());
    }
    return data;
}

std::shared_ptr<ByteBuffer> Resources::find(const ResourceId &id) {
    for (auto &provider : _providers) {
        auto data = provider->findResourceData(id);
        if (data) {
            return data;
        }
    }
    return nullptr;
}

} // namespace resource

} // namespace reone
