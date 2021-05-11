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

#include <cstdint>

namespace reone {

namespace graphics {

struct VertexAttributes {
    uint32_t stride { 0 };
    int offCoords { 0 };
    int offNormals { -1 };
    int offTexCoords1 { -1 };
    int offTexCoords2 { -1 };
    int offTangents { -1 };
    int offBitangents { -1 };
    int offBoneWeights { -1 };
    int offBoneIndices { -1 };
};

} // namespace graphics

} // namespace reone
