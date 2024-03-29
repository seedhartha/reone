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

#include "reone/graphics/format/mdlmdxreader.h"

#include "reone/graphics/animation.h"
#include "reone/graphics/mesh.h"
#include "reone/graphics/model.h"
#include "reone/graphics/statistic.h"
#include "reone/system/exception/validation.h"
#include "reone/system/logutil.h"

namespace reone {

namespace graphics {

static constexpr int kFlagBezier = 16;
static constexpr int kNumSaberPieceFaces = 6;
static constexpr int kNumSaberPieceVertices = 8;

struct EmitterFlags {
    static constexpr int p2p = 1;
    static constexpr int p2pBezier = 2;
    static constexpr int affectedByWind = 4;
    static constexpr int tinted = 8;
    static constexpr int bounce = 0x10;
    static constexpr int random = 0x20;
    static constexpr int inherit = 0x40;
    static constexpr int inheritVelocity = 0x80;
    static constexpr int inheritLocal = 0x100;
    static constexpr int splat = 0x200;
    static constexpr int inheritParticle = 0x400;
    static constexpr int depthTexture = 0x800;
    static constexpr int flag13 = 0x1000;
};

static bool isTSLFunctionPointer(uint32_t ptr) {
    return ptr == kMdlModelFuncPtr1TslPC || ptr == kMdlModelFuncPtr1TslXbox;
}

void MdlMdxReader::load() {
    // File Header
    _mdl.skipBytes(4); // unknown
    uint32_t mdlSize = _mdl.readUint32();
    uint32_t mdxSize = _mdl.readUint32();

    // Geometry Header
    uint32_t funcPtr1 = _mdl.readUint32();
    uint32_t funcPtr2 = _mdl.readUint32();
    std::string name(boost::to_lower_copy(_mdl.readString(32)));
    uint32_t offRootNode = _mdl.readUint32();
    uint32_t numNodes = _mdl.readUint32();
    _mdl.skipBytes(6 * 4); // unknown
    uint32_t refCount = _mdl.readUint32();
    uint8_t modelType = _mdl.readByte();
    _mdl.skipBytes(3); // padding

    // Model Header
    uint8_t classification = _mdl.readByte();
    uint8_t subclassification = _mdl.readByte();
    _mdl.skipBytes(1); // unknown
    uint8_t affectedByFog = _mdl.readByte();
    uint32_t numChildModels = _mdl.readUint32();
    ArrayDefinition animationArrayDef(readArrayDefinition());
    uint32_t superModelRef = _mdl.readUint32();
    std::vector<float> boundingBox(_mdl.readFloatArray(6));
    float radius = _mdl.readFloat();
    float animationScale = _mdl.readFloat();
    std::string superModelName(boost::to_lower_copy(_mdl.readString(32)));
    uint32_t offAnimRoot = _mdl.readUint32();
    _mdl.skipBytes(4); // unknown
    uint32_t mdxSize2 = _mdl.readUint32();
    uint32_t mdxOffset = _mdl.readUint32();
    ArrayDefinition nameArrayDef(readArrayDefinition());

    _tsl = isTSLFunctionPointer(funcPtr1);
    if (superModelName == "null") {
        superModelName.clear();
    }
    _modelName = name;
    _offAnimRoot = offAnimRoot;

    // Read node names
    std::vector<uint32_t> nameOffsets(_mdl.readUint32ArrayAt(kMdlDataOffset + nameArrayDef.offset, nameArrayDef.count));
    readNodeNames(nameOffsets);

    // Read nodes
    std::shared_ptr<ModelNode> rootNode(readNodes(offRootNode, nullptr, false));
    prepareSkinMeshes();

    // Read animations
    std::vector<uint32_t> animOffsets(_mdl.readUint32ArrayAt(kMdlDataOffset + animationArrayDef.offset, animationArrayDef.count));
    std::vector<std::shared_ptr<Animation>> animations(readAnimations(animOffsets));

    _model = std::make_unique<Model>(
        name,
        classification,
        std::move(rootNode),
        std::move(animations),
        std::move(superModelName),
        animationScale);

    _model->setAffectedByFog(affectedByFog != 0);
}

MdlMdxReader::ArrayDefinition MdlMdxReader::readArrayDefinition() {
    ArrayDefinition result;
    result.offset = _mdl.readUint32();
    result.count = _mdl.readUint32();
    result.count2 = _mdl.readUint32();
    return result;
}

void MdlMdxReader::readNodeNames(const std::vector<uint32_t> &offsets) {
    for (uint32_t offset : offsets) {
        std::string name(boost::to_lower_copy(_mdl.readCStringAt(kMdlDataOffset + offset, 32)));
        _nodeNames.push_back(std::move(name));
    }
}

std::shared_ptr<ModelNode> MdlMdxReader::readNodes(uint32_t offset, ModelNode *parent, bool animated, bool animNode) {
    _mdl.seek(kMdlDataOffset + offset);
    if (!animated && offset == _offAnimRoot) {
        animated = true;
    }

    uint16_t flags = _mdl.readUint16();
    uint16_t nodeNumber = _mdl.readUint16();
    uint16_t nameIndex = _mdl.readUint16();
    _mdl.skipBytes(2); // padding
    uint32_t offRootNode = _mdl.readUint32();
    uint32_t offParentNode = _mdl.readUint32();
    std::vector<float> positionValues(_mdl.readFloatArray(3));
    std::vector<float> orientationValues(_mdl.readFloatArray(4));
    ArrayDefinition childArrayDef(readArrayDefinition());
    ArrayDefinition controllerArrayDef(readArrayDefinition());
    ArrayDefinition controllerDataArrayDef(readArrayDefinition());

    if (flags & 0xf408) {
        throw ValidationException("Unsupported MDL node flags: " + std::to_string(flags));
    }
    std::string name(_nodeNames[nameIndex]);
    glm::vec3 restPosition(glm::make_vec3(&positionValues[0]));
    glm::quat restOrientation(orientationValues[0], orientationValues[1], orientationValues[2], orientationValues[3]);

    auto node = std::make_shared<ModelNode>(
        nodeNumber,
        name,
        std::move(restPosition),
        std::move(restOrientation),
        animated,
        parent);

    node->setFlags(flags);

    if (flags & MdlNodeFlags::mesh) {
        node->setMesh(readMesh(flags));
    }
    if (flags & MdlNodeFlags::light) {
        node->setLight(readLight());
    }
    if (flags & MdlNodeFlags::emitter) {
        node->setEmitter(readEmitter());
    }
    if (flags & MdlNodeFlags::reference) {
        node->setReference(readReference());
    }
    if (!animNode) {
        _nodes.push_back(node);
    }

    std::vector<float> controllerData(_mdl.readFloatArrayAt(kMdlDataOffset + controllerDataArrayDef.offset, controllerDataArrayDef.count));
    readControllers(controllerArrayDef.offset, controllerArrayDef.count, controllerData, *node);

    std::vector<uint32_t> childOffsets(_mdl.readUint32ArrayAt(kMdlDataOffset + childArrayDef.offset, childArrayDef.count));
    for (uint32_t offset : childOffsets) {
        node->addChild(readNodes(offset, node.get(), animated, animNode));
    }

    return node;
}

std::shared_ptr<ModelNode::TriangleMesh> MdlMdxReader::readMesh(int flags) {
    // Common Mesh Header
    uint32_t funcPtr1 = _mdl.readUint32();
    uint32_t funcPtr2 = _mdl.readUint32();
    ArrayDefinition faceArrayDef(readArrayDefinition());
    std::vector<float> boundingBox(_mdl.readFloatArray(6));
    float radius = _mdl.readFloat();
    std::vector<float> average(_mdl.readFloatArray(3));
    std::vector<float> diffuse(_mdl.readFloatArray(3));
    std::vector<float> ambient(_mdl.readFloatArray(3));
    uint32_t transprencyHint = _mdl.readUint32();
    std::string texture1(boost::to_lower_copy(_mdl.readString(32)));
    std::string texture2(boost::to_lower_copy(_mdl.readString(32)));
    std::string texture3(boost::to_lower_copy(_mdl.readString(12)));
    std::string texture4(boost::to_lower_copy(_mdl.readString(12)));
    ArrayDefinition indicesCountArrayDef(readArrayDefinition());
    ArrayDefinition indicesOffsetArrayDef(readArrayDefinition());
    ArrayDefinition invCounterArrayDef(readArrayDefinition());
    _mdl.skipBytes(3 * 4 + 8); // unknown
    uint32_t animateUV = _mdl.readUint32();
    float uvDirectionX = _mdl.readFloat();
    float uvDirectionY = _mdl.readFloat();
    float uvJitter = _mdl.readFloat();
    float uvJitterSpeed = _mdl.readFloat();
    uint32_t mdxVertexSize = _mdl.readUint32();
    uint32_t mdxDataFlags = _mdl.readUint32();
    int offMdxVertices = _mdl.readInt32();
    int offMdxNormals = _mdl.readInt32();
    int offMdxVertexColors = _mdl.readInt32();
    int offMdxTexCoords1 = _mdl.readInt32();
    int offMdxTexCoords2 = _mdl.readInt32();
    int offMdxTexCoords3 = _mdl.readInt32();
    int offMdxTexCoords4 = _mdl.readInt32();
    int offMdxTanSpace = _mdl.readInt32();
    _mdl.skipBytes(3 * 4); // unknown
    uint16_t numVertices = _mdl.readUint16();
    uint16_t numTextures = _mdl.readUint16();
    uint8_t lightmapped = _mdl.readByte();
    uint8_t rotateTexture = _mdl.readByte();
    uint8_t backgroundGeometry = _mdl.readByte();
    uint8_t shadow = _mdl.readByte();
    uint8_t beaming = _mdl.readByte();
    uint8_t render = _mdl.readByte();
    _mdl.skipBytes(2); // unknown
    float totalArea = _mdl.readFloat();
    _mdl.skipBytes(4); // unknown
    if (_tsl) {
        _mdl.skipBytes(8);
    }
    uint32_t offMdxData = _mdl.readUint32();
    uint32_t offVertices = _mdl.readUint32();

    std::vector<float> vertices;
    std::vector<uint16_t> indices;
    std::shared_ptr<ModelNode::Skin> skin;
    std::shared_ptr<ModelNode::Danglymesh> danglymesh;
    std::shared_ptr<ModelNode::AABBTree> aabbTree;

    Mesh::VertexLayout vertexLayout;
    vertexLayout.stride = mdxVertexSize;
    vertexLayout.offPosition = offMdxVertices;
    vertexLayout.offNormals = offMdxNormals;
    vertexLayout.offUV1 = offMdxTexCoords1;
    vertexLayout.offUV2 = offMdxTexCoords2;
    vertexLayout.offTanSpace = offMdxTanSpace;

    if (flags & MdlNodeFlags::skin) {
        // Skin Mesh Header
        _mdl.skipBytes(3 * 4); // unknown
        uint32_t offMdxBoneWeights = _mdl.readUint32();
        uint32_t offMdxBoneIndices = _mdl.readUint32();
        uint32_t offBones = _mdl.readUint32();
        uint32_t numBones = _mdl.readUint32();
        ArrayDefinition qBoneArrayDef(readArrayDefinition());
        ArrayDefinition tBoneArrayDef(readArrayDefinition());
        _mdl.skipBytes(3 * 4); // unknown
        std::vector<uint16_t> boneNodeSerial(_mdl.readUint16Array(16));
        _mdl.skipBytes(4); // padding

        std::vector<float> boneMap(_mdl.readFloatArrayAt(kMdlDataOffset + offBones, numBones));
        std::vector<float> qBoneValues(_mdl.readFloatArrayAt(kMdlDataOffset + qBoneArrayDef.offset, 4 * numBones));
        std::vector<float> tBoneValues(_mdl.readFloatArrayAt(kMdlDataOffset + tBoneArrayDef.offset, 3 * numBones));

        std::vector<glm::mat4> boneMatrices;
        boneMatrices.resize(numBones);
        for (int i = 0; i < numBones; ++i) {
            const float *qBone = &qBoneValues[4 * i];
            glm::mat4 boneMatrix(1.0f);
            boneMatrix *= glm::translate(glm::make_vec3(&tBoneValues[3 * i]));
            boneMatrix *= glm::mat4_cast(glm::quat(qBone[0], qBone[1], qBone[2], qBone[3]));
            boneMatrices[i] = std::move(boneMatrix);
        }

        skin = std::make_shared<ModelNode::Skin>();
        skin->boneMap = std::move(boneMap);
        skin->boneMatrices = std::move(boneMatrices);

        vertexLayout.offBoneIndices = static_cast<int>(offMdxBoneIndices);
        vertexLayout.offBoneWeights = static_cast<int>(offMdxBoneWeights);

    } else if (flags & MdlNodeFlags::dangly) {
        // Dangly Mesh Header
        ArrayDefinition constraintArrayDef(readArrayDefinition());
        float displacement = _mdl.readFloat();
        float tightness = _mdl.readFloat();
        float period = _mdl.readFloat();
        uint32_t offDanglyVertices = _mdl.readUint32();

        danglymesh = std::make_shared<ModelNode::Danglymesh>();
        danglymesh->displacement = displacement; // 1/2 meters per unit
        danglymesh->tightness = tightness;
        danglymesh->period = period;

        danglymesh->constraints.resize(constraintArrayDef.count);
        danglymesh->positions.resize(constraintArrayDef.count);
        _mdl.seek(kMdlDataOffset + constraintArrayDef.offset);
        for (uint32_t i = 0; i < constraintArrayDef.count; ++i) {
            danglymesh->constraints[i] = _mdl.readFloat();
        }
        _mdl.seek(kMdlDataOffset + offDanglyVertices);
        for (uint32_t i = 0; i < constraintArrayDef.count; ++i) {
            std::vector<float> position {_mdl.readFloatArray(3)};
            danglymesh->positions[i] = glm::make_vec3(&position[0]);
        }

    } else if (flags & MdlNodeFlags::aabb) {
        // AABB Mesh Header
        uint32_t offTree = _mdl.readUint32();
        aabbTree = readAABBTree(offTree);

    } else if (flags & MdlNodeFlags::saber) {
        // Saber Mesh Header
        uint32_t offSaberVertices = _mdl.readUint32();
        uint32_t offTexCoords = _mdl.readUint32();
        uint32_t offNormals = _mdl.readUint32();
        _mdl.skipBytes(2 * 4); // unknown

        _mdl.seek(static_cast<size_t>(kMdlDataOffset) + offSaberVertices);
        std::vector<float> saberVertices(_mdl.readFloatArray(3 * numVertices));

        _mdl.seek(static_cast<size_t>(kMdlDataOffset) + offTexCoords);
        std::vector<float> texCoords(_mdl.readFloatArray(2 * numVertices));

        _mdl.seek(static_cast<size_t>(kMdlDataOffset) + offNormals);
        std::vector<float> normals(_mdl.readFloatArray(3 * numVertices));

        vertices.resize((3 + 2 + 3) * numVertices);
        float *verticesPtr = &vertices[0];
        for (int i = 0; i < numVertices; ++i) {
            int vertexIdx;
            if (i < 20 * 4) {
                vertexIdx = i + 8;
            } else if (i >= 20 * 4 && i < 20 * 4 + 8) {
                vertexIdx = i - 20 * 4;
            } else if (i >= 20 * 4 + 8 && i < 20 * 4 + 8 + 20 * 4) {
                vertexIdx = i + 8;
            } else {
                vertexIdx = i - 20 * 4;
            }

            // Vertex coordinates
            float *vertexCoordsPtr = &saberVertices[3 * vertexIdx];
            *(verticesPtr++) = vertexCoordsPtr[0];
            *(verticesPtr++) = vertexCoordsPtr[1];
            *(verticesPtr++) = vertexCoordsPtr[2];

            // Normals
            float *normalsPtr = &normals[3 * vertexIdx];
            *(verticesPtr++) = normalsPtr[0];
            *(verticesPtr++) = normalsPtr[1];
            *(verticesPtr++) = normalsPtr[2];

            // Texture coordinates
            float *texCoordsPtr = &texCoords[2 * vertexIdx];
            *(verticesPtr++) = texCoordsPtr[0];
            *(verticesPtr++) = texCoordsPtr[1];
        }
        vertexLayout.stride = 8 * sizeof(float);
        vertexLayout.offPosition = 0;
        vertexLayout.offNormals = 3 * sizeof(float);
        vertexLayout.offUV1 = 6 * sizeof(float);
    }

    // Read vertices
    if (!(flags & MdlNodeFlags::saber) && mdxVertexSize > 0) {
        _mdx.seek(offMdxData);
        vertices = _mdx.readFloatArray(numVertices * mdxVertexSize / sizeof(float));
    }

    std::vector<Mesh::Face> faces;
    if (!(flags & MdlNodeFlags::saber) && faceArrayDef.count > 0) {
        faces.resize(faceArrayDef.count);

        // Faces
        _mdl.seek(kMdlDataOffset + faceArrayDef.offset);
        for (uint32_t i = 0; i < faceArrayDef.count; ++i) {
            std::vector<float> normalValues(_mdl.readFloatArray(3));
            float distance = _mdl.readFloat();
            uint32_t material = _mdl.readUint32();
            std::vector<uint16_t> adjacentFaces(_mdl.readUint16Array(3));
            std::vector<uint16_t> faceIndices(_mdl.readUint16Array(3));

            Mesh::Face face;
            face.vertices[0] = faceIndices[0];
            face.vertices[1] = faceIndices[1];
            face.vertices[2] = faceIndices[2];
            face.adjacentFaces[0] = adjacentFaces[0];
            face.adjacentFaces[1] = adjacentFaces[1];
            face.adjacentFaces[2] = adjacentFaces[2];
            face.normal = glm::make_vec3(&normalValues[0]);
            face.material = material;
            faces[i] = std::move(face);
        }

        // Indices
        _mdl.seek(kMdlDataOffset + indicesOffsetArrayDef.offset);
        uint32_t offIndices = _mdl.readUint32();
        _mdl.seek(kMdlDataOffset + offIndices);
        indices = _mdl.readUint16Array(3 * faceArrayDef.count);

    } else if (flags & MdlNodeFlags::saber) {
        static const int kSaberPieceFaceIndices[] {
            4, 5, 0,
            5, 1, 0,
            5, 6, 1,
            6, 2, 1,
            6, 7, 2,
            7, 3, 2 //
        };
        // rightside pieces
        int indexOffset = 0;
        for (int i = 0; i < kNumSaberSegments + 1; ++i) {
            for (int j = 0; j < kNumSaberPieceFaces; ++j) {
                Mesh::Face face;
                face.vertices[0] = indexOffset + kSaberPieceFaceIndices[3 * j + 0];
                face.vertices[1] = indexOffset + kSaberPieceFaceIndices[3 * j + 1];
                face.vertices[2] = indexOffset + kSaberPieceFaceIndices[3 * j + 2];
                faces.push_back(std::move(face));
            }
            indexOffset += kNumSaberSegmentVertices;
        }
        // leftside pieces
        indexOffset += kNumSaberSegmentVertices;
        for (int i = 0; i < kNumSaberSegments + 1; ++i) {
            for (int j = 0; j < kNumSaberPieceFaces; ++j) {
                Mesh::Face face;
                face.vertices[0] = indexOffset + kSaberPieceFaceIndices[3 * j + 2];
                face.vertices[1] = indexOffset + kSaberPieceFaceIndices[3 * j + 1];
                face.vertices[2] = indexOffset + kSaberPieceFaceIndices[3 * j + 0];
                faces.push_back(std::move(face));
            }
            indexOffset += kNumSaberSegmentVertices;
        }
    }

    auto mesh = std::make_unique<Mesh>(
        std::move(vertices),
        std::move(vertexLayout),
        std::move(faces));

    ModelNode::UVAnimation uvAnimation;
    if (animateUV) {
        uvAnimation.dir = glm::vec2(uvDirectionX, uvDirectionY);
    }
    std::string diffuseMap;
    if (!texture1.empty() && texture1 != "null") {
        diffuseMap = texture1;
    }
    std::string lightmap;
    if (!texture2.empty()) {
        lightmap = texture2;
    }

    auto nodeMesh = std::make_unique<ModelNode::TriangleMesh>();
    nodeMesh->mesh = std::move(mesh);
    nodeMesh->uvAnimation = std::move(uvAnimation);
    nodeMesh->diffuse = glm::make_vec3(&diffuse[0]);
    nodeMesh->ambient = glm::make_vec3(&ambient[0]);
    nodeMesh->transparency = static_cast<int>(transprencyHint);
    nodeMesh->render = static_cast<bool>(render);
    nodeMesh->shadow = static_cast<bool>(shadow);
    nodeMesh->backgroundGeometry = static_cast<bool>(backgroundGeometry);
    nodeMesh->diffuseMap = std::move(diffuseMap);
    nodeMesh->lightmap = std::move(lightmap);
    nodeMesh->skin = std::move(skin);
    nodeMesh->danglymesh = std::move(danglymesh);
    nodeMesh->aabbTree = std::move(aabbTree);
    nodeMesh->saber = flags & MdlNodeFlags::saber;

    return nodeMesh;
}

std::shared_ptr<ModelNode::AABBTree> MdlMdxReader::readAABBTree(uint32_t offset) {
    _mdl.seek(kMdlDataOffset + offset);

    std::vector<float> boundingBox(_mdl.readFloatArray(6));
    uint32_t offChildLeft = _mdl.readUint32();
    uint32_t offChildRight = _mdl.readUint32();
    int faceIndex = _mdl.readInt32();
    uint32_t mostSignificantPlane = _mdl.readUint32();

    auto node = std::make_shared<ModelNode::AABBTree>();
    node->faceIndex = faceIndex;
    node->mostSignificantPlane = static_cast<ModelNode::AABBTree::Plane>(mostSignificantPlane);
    node->aabb.expand(glm::make_vec3(&boundingBox[0]));
    node->aabb.expand(glm::make_vec3(&boundingBox[3]));

    if (faceIndex == -1) {
        node->left = readAABBTree(offChildLeft);
        node->right = readAABBTree(offChildRight);
    }

    return node;
}

std::shared_ptr<ModelNode::Light> MdlMdxReader::readLight() {
    float flareRadius = _mdl.readFloat();
    _mdl.skipBytes(3 * 4); // unknown
    ArrayDefinition flareSizesArrayDef(readArrayDefinition());
    ArrayDefinition flarePositionsArrayDef(readArrayDefinition());
    ArrayDefinition flareColorShiftsArrayDef(readArrayDefinition());
    ArrayDefinition flareTexturesArrayDef(readArrayDefinition());
    uint32_t priority = _mdl.readUint32();
    uint32_t ambientOnly = _mdl.readUint32();
    uint32_t dynamicType = _mdl.readUint32();
    uint32_t affectDynamic = _mdl.readUint32();
    uint32_t shadow = _mdl.readUint32();
    uint32_t flare = _mdl.readUint32();
    uint32_t fading = _mdl.readUint32();

    auto light = std::make_shared<ModelNode::Light>();
    light->priority = priority;
    light->ambientOnly = static_cast<bool>(ambientOnly);
    light->dynamicType = dynamicType;
    light->affectDynamic = static_cast<bool>(affectDynamic);
    light->shadow = static_cast<bool>(shadow);
    light->flareRadius = flareRadius;
    light->fading = static_cast<bool>(fading);

    int numFlares = static_cast<int>(flareTexturesArrayDef.count);
    if (numFlares > 0) {
        std::vector<float> flareSizes(_mdl.readFloatArrayAt(kMdlDataOffset + flareSizesArrayDef.offset, flareSizesArrayDef.count));
        std::vector<float> flarePositions(_mdl.readFloatArrayAt(kMdlDataOffset + flarePositionsArrayDef.offset, flarePositionsArrayDef.count));
        std::vector<uint32_t> texNameOffsets(_mdl.readUint32ArrayAt(kMdlDataOffset + flareTexturesArrayDef.offset, flareTexturesArrayDef.count));

        std::vector<glm::vec3> colorShifts;
        for (int i = 0; i < numFlares; ++i) {
            _mdl.seek(kMdlDataOffset + flareColorShiftsArrayDef.offset + 12 * i);
            glm::vec3 colorShift(_mdl.readFloat(), _mdl.readFloat(), _mdl.readFloat());
            colorShifts.push_back(std::move(colorShift));
        }

        std::vector<std::string> flareTextures;
        for (int i = 0; i < numFlares; ++i) {
            _mdl.seek(kMdlDataOffset + texNameOffsets[i]);
            std::string textureName(boost::to_lower_copy(_mdl.readString(12)));
            flareTextures.push_back(std::move(textureName));
        }

        for (int i = 0; i < numFlares; ++i) {
            ModelNode::LensFlare lensFlare;
            lensFlare.textureName = flareTextures[i];
            lensFlare.colorShift = colorShifts[i];
            lensFlare.position = flarePositions[i];
            lensFlare.size = flareSizes[i];
            light->flares.push_back(std::move(lensFlare));
        }
    }

    return light;
}

static ModelNode::Emitter::UpdateMode parseEmitterUpdate(const std::string &str) {
    auto result = ModelNode::Emitter::UpdateMode::Invalid;
    if (str == "fountain") {
        result = ModelNode::Emitter::UpdateMode::Fountain;
    } else if (str == "single") {
        result = ModelNode::Emitter::UpdateMode::Single;
    } else if (str == "explosion") {
        result = ModelNode::Emitter::UpdateMode::Explosion;
    } else if (str == "lightning") {
        result = ModelNode::Emitter::UpdateMode::Lightning;
    }
    return result;
}

static ModelNode::Emitter::RenderMode parseEmitterRender(const std::string &str) {
    auto result = ModelNode::Emitter::RenderMode::Invalid;
    if (str == "normal") {
        result = ModelNode::Emitter::RenderMode::Normal;
    } else if (str == "linked") {
        result = ModelNode::Emitter::RenderMode::Linked;
    } else if (str == "billboard_to_local_z") {
        result = ModelNode::Emitter::RenderMode::BillboardToLocalZ;
    } else if (str == "billboard_to_world_z") {
        result = ModelNode::Emitter::RenderMode::BillboardToWorldZ;
    } else if (str == "aligned_to_world_z") {
        result = ModelNode::Emitter::RenderMode::AlignedToWorldZ;
    } else if (str == "aligned_to_particle_dir") {
        result = ModelNode::Emitter::RenderMode::AlignedToParticleDir;
    } else if (str == "motion_blur") {
        result = ModelNode::Emitter::RenderMode::MotionBlur;
    }
    return result;
}

static ModelNode::Emitter::BlendMode parseEmitterBlend(const std::string &str) {
    auto result = ModelNode::Emitter::BlendMode::Invalid;
    if (str == "normal") {
        result = ModelNode::Emitter::BlendMode::Normal;
    } else if (str == "punch-through") {
        result = ModelNode::Emitter::BlendMode::PunchThrough;
    } else if (str == "lighten") {
        result = ModelNode::Emitter::BlendMode::Lighten;
    }
    return result;
}

std::shared_ptr<ModelNode::Emitter> MdlMdxReader::readEmitter() {
    float deadSpace = _mdl.readFloat();
    float blastRadius = _mdl.readFloat();
    float blastLength = _mdl.readFloat();
    uint32_t branchCount = _mdl.readUint32();
    float controlPointSmoothing = _mdl.readFloat();
    uint32_t xGrid = _mdl.readUint32();
    uint32_t yGrid = _mdl.readUint32();
    _mdl.skipBytes(4); // unknown
    std::string update(boost::to_lower_copy(_mdl.readString(32)));
    std::string render(boost::to_lower_copy(_mdl.readString(32)));
    std::string blend(boost::to_lower_copy(_mdl.readString(32)));
    std::string texture(boost::to_lower_copy(_mdl.readString(32)));
    std::string chunkName(boost::to_lower_copy(_mdl.readString(16)));
    uint32_t twosided = _mdl.readUint32();
    uint32_t loop = _mdl.readUint32();
    uint32_t renderOrder = _mdl.readUint32();
    uint32_t frameBlending = _mdl.readUint32();
    std::string depthTexture(boost::to_lower_copy(_mdl.readString(32)));
    _mdl.skipBytes(1); // padding
    uint32_t flags = _mdl.readUint32();

    auto emitter = std::make_shared<ModelNode::Emitter>();
    emitter->updateMode = parseEmitterUpdate(update);
    emitter->renderMode = parseEmitterRender(render);
    emitter->blendMode = parseEmitterBlend(blend);
    emitter->textureName = std::move(texture);
    emitter->gridSize = glm::ivec2(glm::max(xGrid, 1u), glm::max(yGrid, 1u));
    emitter->renderOrder = renderOrder;
    emitter->twosided = static_cast<bool>(twosided);
    emitter->loop = static_cast<bool>(loop);
    emitter->p2p = flags & EmitterFlags::p2p;
    emitter->p2pBezier = flags & EmitterFlags::p2pBezier;

    return emitter;
}

std::shared_ptr<ModelNode::Reference> MdlMdxReader::readReference() {
    std::string modelResRef(boost::to_lower_copy(_mdl.readString(32)));
    uint32_t reattachable = _mdl.readUint32();

    auto reference = std::make_shared<ModelNode::Reference>();
    reference->modelName = std::move(modelResRef);
    reference->reattachable = static_cast<bool>(reattachable);

    return reference;
}

void MdlMdxReader::readControllers(uint32_t keyOffset,
                                   uint32_t keyCount,
                                   const std::vector<float> &data,
                                   ModelNode &node) {
    _mdl.seek(kMdlDataOffset + keyOffset);
    for (uint32_t i = 0; i < keyCount; ++i) {
        uint32_t type = _mdl.readUint32();
        uint16_t unk = _mdl.readUint16();
        uint16_t numRows = _mdl.readUint16();
        uint16_t timeIndex = _mdl.readUint16();
        uint16_t dataIndex = _mdl.readUint16();
        uint8_t numColumns = _mdl.readByte();
        _mdl.skipBytes(3); // padding

        ControllerKey key;
        key.type = type;
        key.numRows = numRows;
        key.timeIndex = timeIndex;
        key.dataIndex = dataIndex;
        key.numColumns = numColumns;

        int numColumnsBase = numColumns & ~kFlagBezier;
        if (type == ControllerTypes::orientation) {
            KeyframeTrack<glm::quat> track;
            readQuaternionController(key, data, node, track);
            node.quaternionTracks().insert({type, std::move(track)});
        } else if (numColumnsBase == 3) {
            KeyframeTrack<glm::vec3> track;
            readVectorController(key, data, node, track);
            node.vectorTracks().insert({type, std::move(track)});
        } else if (numColumnsBase == 1) {
            KeyframeTrack<float> track;
            readFloatController(key, data, node, track);
            node.floatTracks().insert({type, std::move(track)});
        } else {
            throw ValidationException(str(boost::format("Unsupported controller: type=%d numColumnsBase=%d") % type % numColumnsBase));
        }
    }
}

void MdlMdxReader::prepareSkinMeshes() {
    for (auto &node : _nodes) {
        if (!node->isSkinMesh()) {
            continue;
        }
        auto skin = node->mesh()->skin;
        for (size_t i = 0; i < skin->boneMap.size(); ++i) {
            auto boneIdx = static_cast<uint16_t>(skin->boneMap[i]);
            if (boneIdx >= skin->boneNodeNumber.size()) {
                skin->boneSerial.resize(boneIdx + 1);
                skin->boneNodeNumber.resize(boneIdx + 1);
            }
            if (boneIdx == 0xffff) {
                continue;
            }
            auto boneNode = _nodes[i];
            skin->boneSerial[boneIdx] = i;
            skin->boneNodeNumber[boneIdx] = boneNode->number();
        }
    }
}

std::vector<std::shared_ptr<Animation>> MdlMdxReader::readAnimations(const std::vector<uint32_t> &offsets) {
    std::vector<std::shared_ptr<Animation>> anims;
    anims.reserve(offsets.size());

    for (uint32_t offset : offsets) {
        anims.push_back(readAnimation(offset));
    }

    return anims;
}

std::unique_ptr<Animation> MdlMdxReader::readAnimation(uint32_t offset) {
    _mdl.seek(kMdlDataOffset + offset);

    // Geometry Header
    uint32_t funcPtr1 = _mdl.readUint32();
    uint32_t funcPtr2 = _mdl.readUint32();
    std::string name(boost::to_lower_copy(_mdl.readString(32)));
    uint32_t offRootNode = _mdl.readUint32();
    uint32_t numNodes = _mdl.readUint32();
    _mdl.skipBytes(6 * 4); // unknown
    uint32_t refCount = _mdl.readUint32();
    uint8_t modelType = _mdl.readByte();
    _mdl.skipBytes(3); // padding

    // Animation Header
    float length = _mdl.readFloat();
    float transitionTime = _mdl.readFloat();
    std::string root(boost::to_lower_copy(_mdl.readString(32)));
    ArrayDefinition eventArrayDef(readArrayDefinition());
    _mdl.skipBytes(4); // unknown

    std::shared_ptr<ModelNode> rootNode(readNodes(offRootNode, nullptr, false, true));

    // Events
    std::vector<Animation::Event> events;
    if (eventArrayDef.count > 0) {
        _mdl.seek(kMdlDataOffset + eventArrayDef.offset);
        for (uint32_t i = 0; i < eventArrayDef.count; ++i) {
            Animation::Event event;
            event.time = _mdl.readFloat();
            event.name = boost::to_lower_copy(_mdl.readString(32));
            events.push_back(std::move(event));
        }
        sort(events.begin(), events.end(), [](auto &left, auto &right) { return left.time < right.time; });
    }

    return std::make_unique<Animation>(
        std::move(name),
        length,
        transitionTime,
        root != _modelName ? root : "",
        std::move(rootNode),
        std::move(events));
}

static inline void ensureNumColumnsEquals(int type, int expected, int actual) {
    if (actual != expected) {
        throw ValidationException(str(boost::format("Controller %d: number of columns is %d, expected %d") % type % actual % expected));
    }
}

void MdlMdxReader::readFloatController(const ControllerKey &key,
                                       const std::vector<float> &data,
                                       ModelNode &node,
                                       KeyframeTrack<float> &track) {
    bool bezier = key.numColumns & kFlagBezier;
    int numColumns = key.numColumns & ~kFlagBezier;
    ensureNumColumnsEquals(key.type, 1, numColumns);
    for (uint16_t i = 0; i < key.numRows; ++i) {
        float time = data[key.timeIndex + i];
        float value = data[key.dataIndex + (bezier ? 3 : 1) * i];
        track.add(time, value);
    }
    track.update();
}

void MdlMdxReader::readVectorController(const ControllerKey &key,
                                        const std::vector<float> &data,
                                        ModelNode &node,
                                        KeyframeTrack<glm::vec3> &track) {
    bool bezier = key.numColumns & kFlagBezier;
    int numColumns = key.numColumns & ~kFlagBezier;
    if (numColumns == 9) {
        // HACK: workaround for s_male02 from TSLRCM
        numColumns = 3;
    }
    ensureNumColumnsEquals(key.type, 3, numColumns);
    for (uint16_t i = 0; i < key.numRows; ++i) {
        float time = data[key.timeIndex + i];
        glm::vec3 value(glm::make_vec3(&data[key.dataIndex + (bezier ? 9 : 3) * i]));
        track.add(time, value);
    }
    track.update();
}

void MdlMdxReader::readQuaternionController(const ControllerKey &key,
                                            const std::vector<float> &data,
                                            ModelNode &node,
                                            KeyframeTrack<glm::quat> &track) {
    switch (key.numColumns) {
    case 2:
        for (uint16_t i = 0; i < key.numRows; ++i) {
            int rowTimeIdx = key.timeIndex + i;
            int rowDataIdx = key.dataIndex + i;

            uint32_t temp = *reinterpret_cast<const uint32_t *>(&data[rowDataIdx]);
            float x = 1.0f - static_cast<float>(temp & 0x7ff) / 1023.0f;
            float y = 1.0f - static_cast<float>((temp >> 11) & 0x7ff) / 1023.0f;
            float z = 1.0f - static_cast<float>(temp >> 22) / 511.0f;
            float dot = x * x + y * y + z * z;
            float w;

            if (dot >= 1.0f) {
                float len = glm::sqrt(dot);
                x /= len;
                y /= len;
                z /= len;
                w = 0.0f;
            } else {
                w = -glm::sqrt(1.0f - dot);
            }

            float time = data[rowTimeIdx];
            track.add(time, glm::quat {w, x, y, z});
        }
        break;
    case 4:
        for (uint16_t i = 0; i < key.numRows; ++i) {
            int rowTimeIdx = key.timeIndex + i;
            int rowDataIdx = key.dataIndex + 4 * i;

            float time = data[rowTimeIdx];

            float x = data[rowDataIdx + 0];
            float y = data[rowDataIdx + 1];
            float z = data[rowDataIdx + 2];
            float w = data[rowDataIdx + 3];
            track.add(time, glm::quat {w, x, y, z});
        }
        break;
    default:
        throw ValidationException("Unexpected number of columns: " + std::to_string(key.numColumns));
    }
    track.update();
}

} // namespace graphics

} // namespace reone
