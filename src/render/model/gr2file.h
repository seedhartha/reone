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

#include "../../render/model/model.h"
#include "../../resource/format/binfile.h"

namespace reone {

namespace render {

/**
 * Encapsulates reading GR2 model files, used by Star Wars: The Old Republic.
 * This is highly experimental.
 */
class Gr2File : public resource::BinaryFile {
public:
    Gr2File();

    std::shared_ptr<render::Model> model() const { return _model; }

private:
    struct MeshHeader {
        uint32_t nameOffset { 0 };
        uint16_t numPieces { 0 };
        uint16_t numUsedBones { 0 };
        uint16_t verticesMask { 0 };
        uint16_t numVertexBytes { 0 };
        uint32_t numVertices { 0 };
        uint32_t numFaces { 0 };
        uint32_t verticesOffset { 0 };
        uint32_t piecesOffset { 0 };
        uint32_t facesOffset { 0 };
        uint32_t bonesOffset { 0 };
    };

    struct MeshPiece {
        uint32_t materialFacesIdx { 0 };
        uint32_t numMaterialFaces { 0 };
        uint32_t textureId { 0 };
    };

    uint16_t _numMeshes { 0 };
    std::vector<MeshHeader> _meshHeaders;
    std::vector<std::string> _meshNames;
    std::vector<std::vector<MeshPiece>> _meshPieces;
    std::vector<std::shared_ptr<render::ModelMesh>> _meshes;

    void doLoad() override;

    void loadMeshHeaders();
    void loadMeshNames();
    void loadMeshPieces();
    void loadMeshes();
    void loadModel();

    std::shared_ptr<render::Model> _model;
};

} // namespace render

} // namespace reone
