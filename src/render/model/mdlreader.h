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
#include "../../resource/types.h"

#include "aabbnode.h"
#include "model.h"
#include "modelloader.h"

namespace reone {

namespace render {

class MdlReader : public resource::BinaryReader {
public:
    MdlReader(resource::GameID gameId);

    void load(const std::shared_ptr<std::istream> &mdl, const std::shared_ptr<std::istream> &mdx);

    std::shared_ptr<render::Model> model() const { return _model; }

private:
    struct FileHeader {
        uint32_t magic { 0 };
        uint32_t mdlSize { 0 };
        uint32_t mdxSize { 0 };
    };

    struct GeometryHeader {
        uint32_t fp[2];
        char name[32];
        uint32_t offRootNode { 0 };
        uint32_t numNodes { 0 };
        uint32_t unknown[7];
        uint8_t type { 0 };
        uint8_t padding[3];
    };

    struct ArrayHeader {
        uint32_t offset { 0 };
        uint32_t count { 0 };
        uint32_t count2 { 0 };
    };

    struct ModelHeader {
        GeometryHeader geometry;
        uint8_t classification { 0 };
        uint8_t classification2 { 0 };
        uint8_t padding { 0 };
        uint8_t ignoreFog { 0 };
        uint32_t numChildModels { 0 };
        ArrayHeader animations;
        uint32_t unknown { 0 };
        float bbMin[3];
        float bbMax[3];
        float radius { 0.0f };
        float animationScale { 0.0f };
        char superModel[32];
    };

    struct NamesHeader {
        uint32_t offRootNode { 0 };
        uint32_t unused { 0 };
        uint32_t mdxSize { 0 };
        uint32_t mdxOffset { 0 };
        ArrayHeader names;
    };

    struct NodeHeader {
        uint16_t flags { 0 };
        uint16_t index { 0 };
        uint16_t nodeNumber { 0 };
        uint16_t padding { 0 };
        uint32_t offRootNode { 0 };
        uint32_t offParentNode { 0 };
        float position[3];
        float orientation[4];
        ArrayHeader children;
        ArrayHeader controllers;
        ArrayHeader controllerData;
    };

    struct AnimationHeader {
        GeometryHeader geometry;
        float length { 0.0f };
        float transitionTime { 0.0f };
        char root[32];
        ArrayHeader events;
        uint32_t unknown { 0 };
    };

    struct LightHeader {
        ArrayHeader unknown;
        ArrayHeader lensFlareSizes;
        ArrayHeader flarePositions;
        ArrayHeader flareColorShifts;
        ArrayHeader flareTextures;
        float flareRadius { 0.0f };
        uint32_t priority { 0 };
        uint32_t ambientOnly { 0 };
        uint32_t dynamicType { 0 };
        uint32_t affectDynamic { 0 };
        uint32_t shadow { 0 };
        uint32_t flare { 0 };
        uint32_t fading { 0 };
    };

    struct EmitterHeader {
        float deadSpace { 0.0f };
        float blastRadius { 0.0f };
        float blastLength { 0.0f };
        uint32_t branchCount { 0 };
        float controlPointSmoothing { 0.0f };
        uint32_t xGrid { 0 };
        uint32_t yGrid { 0 };
        uint32_t unknown { 0 };
        char update[32];
        char render[32];
        char blend[32];
        char texture[32];
        char chunkName[16];
        uint32_t twosided { 0 };
        uint32_t loop { 0 };
        uint32_t renderOrder { 0 };
        uint32_t frameBlending { 0 };
        char depthTexture[32];
        uint8_t padding { 0 };
        uint32_t flags { 0 };
    };

    struct ReferenceHeader {
        char modelResRef[32];
        uint32_t reattachable { 0 };
    };

    struct MeshHeader {
        uint32_t fn[2];
        ArrayHeader faces;
        float bbMin[3];
        float bbMax[3];
        float radius { 0.0f };
        float average[3];
        float diffuse[3];
        float ambient[3];
        uint32_t transparencyHint { 0 };
        char texture1[32];
        char texture2[32];
        char texture3[12];
        char texture4[12];
        ArrayHeader indicesCounts;
        ArrayHeader indicesOffsets;
        ArrayHeader invCounters;
        uint32_t unknown1[3];
        uint8_t unknown2[8];
        uint32_t animateUV { 0 };
        float uvDirectionX { 0.0f };
        float uvDirectionY { 0.0f };
        float uvJitter { 0.0f };
        float uvJitterSpeed { 0.0f };
        uint32_t mdxVertexSize { 0 };
        uint32_t mdxDataFlags { 0 };
        int offMdxVertices { 0 };
        int offMdxNormals { 0 };
        int offMdxVertexColors { 0 };
        int offMdxTexCoords1 { 0 };
        int offMdxTexCoords2 { 0 };
        int offMdxTexCoords3 { 0 };
        int offMdxTexCoords4 { 0 };
        int offMdxTanSpace { 0 };
        uint32_t unknown3[3];
        uint16_t numVertices { 0 };
        uint16_t numTextures { 0 };
        uint8_t lightmapped { 0 };
        uint8_t rotateTexture { 0 };
        uint8_t backgroundGeometry { 0 };
        uint8_t shadow { 0 };
        uint8_t beaming { 0 };
        uint8_t render { 0 };
        uint8_t unknown4[2];
        float totalArea { 0.0f };
        uint32_t unknown5 { 0 };

        // Technically, this also contains:
        // - TSL values (8 bytes)
        // - Offset to MDX data (4 bytes)
        // - Offset to vertices (4 bytes)
    };

    struct SkinHeader {
        int uknown[3];
        uint32_t offMdxBoneWeights { 0 };
        uint32_t offMdxBoneIndices { 0 };
        uint32_t offBones { 0 };
        uint32_t numBones { 0 };
        uint8_t unknown[72]; // QBones, TBones, etc.
    };

    struct DanglymeshHeader {
        ArrayHeader constraints;
        float displacement { 0.0f };
        float tightness { 0.0f };
        float period { 0.0f };
        uint32_t unknown1 { 0 };
    };

    struct AABBNodeHeader {
        float bbMin[3];
        float bbMax[3];
        uint32_t offChildLeft { 0 };
        uint32_t offChildRight { 0 };
        int faceIndex { 0 };
        uint32_t mostSignificantPlane { 0 };
    };

    struct SaberHeader {
        uint32_t offVertices { 0 };
        uint32_t offTexCoords { 0 };
        uint32_t offNormals { 0 };
        uint32_t unknown[2];
    };

    resource::GameID _gameId;

    std::unique_ptr<StreamReader> _mdxReader;

    FileHeader _fileHeader;
    ModelHeader _modelHeader;
    NamesHeader _namesHeader;

    int _nodeIndex { 0 };
    std::vector<std::string> _nodeNames;
    std::unordered_map<uint32_t, int> _nodeFlags;
    bool _animations { false }; /**< currently reading animations */

    std::shared_ptr<render::Model> _model;

    void doLoad() override;

    void readNodeNames(const std::vector<uint32_t> &offsets);
    std::unique_ptr<render::ModelNode> readNode(uint32_t offset, render::ModelNode *parent);
    std::vector<std::shared_ptr<render::Animation>> readAnimations(const std::vector<uint32_t> &offsets);
    std::unique_ptr<render::Animation> readAnimation(uint32_t offset);

    void readControllers(int nodeFlags, uint32_t keyOffset, uint32_t keyCount, const std::vector<float> &data, render::ModelNode &node);
    void readLight(render::ModelNode &node);
    void readEmitter(render::ModelNode &node);
    void readReference(render::ModelNode &node);
    void readMesh(render::ModelNode &node);
    void readSkin(render::ModelNode &node);
    void readDanglymesh(render::ModelNode &node);
    void readAABB(render::ModelNode &node);
    void readSaber(render::ModelNode &node);

    void loadMesh(const MeshHeader &header, int numVertices, std::vector<float> &&vertices, std::vector<uint16_t> &&indices, Mesh::VertexOffsets &&offsets, render::ModelNode &node);
    std::shared_ptr<AABBNode> readAABBNode(uint32_t offset);
};

class MdlModelLoader : public IModelLoader {
public:
    std::shared_ptr<Model> loadModel(resource::GameID gameId, const std::string &resRef) override;
};

} // namespace render

} // namespace reone
