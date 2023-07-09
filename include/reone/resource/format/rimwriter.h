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

#include "../types.h"

namespace reone {

class IOutputStream;

namespace resource {

class RimWriter {
public:
    struct Resource {
        std::string resRef;
        ResourceType resType {ResourceType::Invalid};
        ByteBuffer data;
    };

    void add(Resource &&res);

    void save(const boost::filesystem::path &path);
    void save(IOutputStream &out);

private:
    std::vector<Resource> _resources;
};

} // namespace resource

} // namespace reone
