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

#include <unordered_map>

#include "format/bifreader.h"
#include "format/keyreader.h"
#include "resourceprovider.h"

namespace reone {

namespace resource {

class KeyBifResourceProvider : public IResourceProvider {
public:
    void init(const boost::filesystem::path &keyPath);

    std::shared_ptr<ByteArray> find(const std::string &resRef, ResourceType type) override;

    bool supports(ResourceType type) const override;

private:
    boost::filesystem::path _gamePath;
    KeyReader _keyFile;
    std::unordered_map<int, std::unique_ptr<BifReader>> _bifCache;
};

} // namespace resource

} // namespace reone
