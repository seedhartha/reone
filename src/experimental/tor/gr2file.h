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
#include "../../render/model/modelloader.h"
#include "../../resource/format/binfile.h"

namespace reone {

namespace tor {

/**
 * Encapsulates loading GR2 model files, used by Star Wars: The Old Republic.
 */
class Gr2File : public resource::BinaryFile {
public:
    enum class FileType {
        Geometry = 0,
        GeometryWithCollision = 1,
        Skeleton = 2
    };

    /**
     * @param resRef ResRef of the model
     * @param animations list of animations to pass to the loaded model
     * @param skeleton the skeleton model to append to the loaded model
     */
    Gr2File(
        std::string resRef,
        std::vector<std::shared_ptr<render::Animation>> animations,
        std::shared_ptr<render::Model> skeleton = nullptr);

    std::shared_ptr<render::Model> model() const { return _model; }

private:
    struct MeshHeader {
        std::string name;
        uint16_t numPieces { 0 };
        uint16_t numUsedBones { 0 };
        uint16_t vertexMask { 0 };
        uint16_t vertexSize { 0 };
        uint32_t numVertices { 0 };
        uint32_t numIndices { 0 };
        uint32_t offsetVertices { 0 };
        uint32_t offsetPieces { 0 };
        uint32_t offsetIndices { 0 };
        uint32_t offsetBones { 0 };
    };

    struct MeshPiece {
        uint32_t startFaceIdx { 0 };
        uint32_t numFaces { 0 };
        uint32_t materialIndex { 0 };
        uint32_t pieceIndex { 0 };
    };

    struct MeshBone {
        std::string name;
        std::vector<float> bounds;
    };

    struct Gr2Mesh {
        MeshHeader header;
        std::vector<std::shared_ptr<MeshPiece>> pieces;
        std::shared_ptr<render::ModelMesh> mesh;
        std::vector<std::shared_ptr<MeshBone>> bones;
    };

    struct SkeletonBone {
        std::string name;
        uint32_t parentIndex { 0 };
        glm::mat4 rootToBone { 1.0f };
    };

    struct Attachment {
        std::string name;
        std::string boneName;
        glm::mat4 transform { 1.0f };
    };

    std::string _resRef;
    std::vector<std::shared_ptr<render::Animation>> _animations;
    std::shared_ptr<render::Model> _skeleton;

    FileType _fileType { FileType::Geometry };
    uint16_t _numMeshes { 0 };
    uint16_t _numMaterials { 0 };
    uint16_t _numBones { 0 };
    uint16_t _numAttachments { 0 };
    uint32_t _offsetMeshHeader { 0 };
    uint32_t _offsetMaterialHeader { 0 };
    uint32_t _offsetBoneStructure { 0 };
    uint32_t _offsetAttachments { 0 };

    std::vector<std::shared_ptr<Gr2Mesh>> _meshes;
    std::vector<std::shared_ptr<SkeletonBone>> _bones;
    std::vector<std::string> _materials;
    std::vector<std::shared_ptr<Attachment>> _attachments;
    std::shared_ptr<render::Model> _model;

    void doLoad() override;

    void loadMeshes();
    void loadMaterials();
    void loadSkeletonBones();
    void loadAttachments();
    void loadModel();

    std::unique_ptr<Gr2Mesh> readMesh();
    std::unique_ptr<MeshPiece> readMeshPiece();
    std::unique_ptr<render::ModelMesh> readModelMesh(const Gr2Mesh &mesh);
    std::unique_ptr<MeshBone> readMeshBone();
    std::unique_ptr<SkeletonBone> readSkeletonBone();
    std::unique_ptr<Attachment> readAttachment();
};

class Gr2ModelLoader : public render::IModelLoader {
public:
    std::shared_ptr<render::Model> loadModel(resource::GameID gameId, const std::string &resRef) override;
};

} // namespace tor

} // namespace reone
