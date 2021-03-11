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

#include "../../resource/format/binreader.h"

#include "walkmesh.h"

namespace reone {

namespace render {

class BwmReader : public resource::BinaryReader {
public:
    BwmReader();

    std::shared_ptr<Walkmesh> walkmesh() const { return _walkmesh; }

private:
    enum class WalkmeshType {
        PWK_DWK = 0,
        WOK = 1
    };

    WalkmeshType _type { WalkmeshType::WOK };

    uint32_t _numVertices { 0 };
    uint32_t _offsetVertices { 0 };
    uint32_t _numFaces { 0 };
    uint32_t _offsetIndices { 0 };
    uint32_t _offsetMaterials { 0 };
    uint32_t _offsetNormals { 0 };
    uint32_t _offsetPlanarDistances { 0 };
    uint32_t _numAabb { 0 };
    uint32_t _offsetAabb { 0 };
    uint32_t _numAdjacencies { 0 };
    uint32_t _offsetAdjacencies { 0 };
    uint32_t _numEdges { 0 };
    uint32_t _offsetEdges { 0 };
    uint32_t _numPerimeters { 0 };
    uint32_t _offsetPerimeters { 0 };

    std::vector<float> _vertices;
    std::vector<uint32_t> _indices;
    std::vector<WalkmeshMaterial> _materials;
    std::vector<float> _normals;

    std::shared_ptr<Walkmesh> _walkmesh;

    void doLoad() override;

    void loadVertices();
    void loadIndices();
    void loadMaterials();
    void loadNormals();

    void makeWalkmesh();
};

} // namespace render

} // namespace reone
