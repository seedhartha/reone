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

#include "../../common/types.h"

#include "../types.h"

namespace reone {

namespace resource {

class ErfWriter {
public:
    enum class FileType {
        ERF,
        MOD
    };

    struct Resource {
        std::string resRef;
        ResourceType resType {ResourceType::Invalid};
        ByteArray data;
    };

    void add(Resource &&res);

    void save(FileType type, const boost::filesystem::path &path);
    void save(FileType type, std::shared_ptr<std::ostream> out);

private:
    std::vector<Resource> _resources;
};

} // namespace resource

} // namespace reone
