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

#include "reone/resource/format/mdlmdxreader.h"

#include "reone/graphics/animation.h"
#include "reone/graphics/mesh.h"
#include "reone/graphics/model.h"
#include "reone/resource/exception/format.h"
#include "reone/resource/models.h"
#include "reone/resource/textures.h"
#include "reone/system/logutil.h"

using namespace reone::graphics;

namespace reone {

namespace resource {

static constexpr int kFlagBezier = 16;

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
    _modelName = name;
    _offAnimRoot = offAnimRoot;

    // Read node names
    std::vector<uint32_t> nameOffsets(_mdl.readUint32ArrayAt(kMdlDataOffset + nameArrayDef.offset, nameArrayDef.count));
    readNodeNames(nameOffsets);

    // Read nodes
    std::shared_ptr<ModelNode> rootNode(readNodes(offRootNode, nullptr, false));
    prepareSkinMeshes();

    // Load supermodel
    std::shared_ptr<Model> superModel;
    if (!superModelName.empty() && superModelName != "null") {
        superModel = _models.get(superModelName);
    }

    // Read animations
    std::vector<uint32_t> animOffsets(_mdl.readUint32ArrayAt(kMdlDataOffset + animationArrayDef.offset, animationArrayDef.count));
    std::vector<std::shared_ptr<Animation>> animations(readAnimations(animOffsets));

    _model = std::make_unique<Model>(
        name,
        classification,
        std::move(rootNode),
        std::move(animations),
        std::move(superModel),
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
        throw FormatException("Unsupported MDL node flags: " + std::to_string(flags));
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
        _nodeFlags.insert(std::make_pair(nodeNumber, flags));
    }

    std::vector<float> controllerData(_mdl.readFloatArrayAt(kMdlDataOffset + controllerDataArrayDef.offset, controllerDataArrayDef.count));
    readControllers(controllerArrayDef.offset, controllerArrayDef.count, controllerData, animNode, *node);

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

    Mesh::VertexSpec spec;
    spec.stride = mdxVertexSize;
    spec.offCoords = offMdxVertices;
    spec.offNormals = offMdxNormals;
    spec.offUV1 = offMdxTexCoords1;
    spec.offUV2 = offMdxTexCoords2;
    spec.offTanSpace = offMdxTanSpace;

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

        spec.offBoneIndices = static_cast<int>(offMdxBoneIndices);
        spec.offBoneWeights = static_cast<int>(offMdxBoneWeights);

    } else if (flags & MdlNodeFlags::dangly) {
        // Dangly Mesh Header
        ArrayDefinition constraintArrayDef(readArrayDefinition());
        float displacement = _mdl.readFloat();
        float tightness = _mdl.readFloat();
        float period = _mdl.readFloat();
        uint32_t offDanglyVertices = _mdl.readUint32();

        danglymesh = std::make_shared<ModelNode::Danglymesh>();
        danglymesh->displacement = 0.5f * displacement; // displacement is allegedly 1/2 meters per unit
        danglymesh->tightness = tightness;
        danglymesh->period = period;

        danglymesh->constraints.resize(constraintArrayDef.count);
        _mdl.seek(kMdlDataOffset + constraintArrayDef.offset);
        for (uint32_t i = 0; i < constraintArrayDef.count; ++i) {
            float multiplier = _mdl.readFloat();
            danglymesh->constraints[i].multiplier = glm::clamp(multiplier / 255.0f, 0.0f, 1.0f);
        }
        _mdl.seek(kMdlDataOffset + offDanglyVertices);
        for (uint32_t i = 0; i < constraintArrayDef.count; ++i) {
            std::vector<float> positionValues(_mdl.readFloatArray(3));
            danglymesh->constraints[i].position = glm::make_vec3(&positionValues[0]);
        }

    } else if (flags & MdlNodeFlags::aabb) {
        // AABB Mesh Header
        uint32_t offTree = _mdl.readUint32();
        aabbTree = readAABBTree(offTree);

    } else if (flags & MdlNodeFlags::saber) {
        // Lightsaber blade is a special case. It consists of four to eight
        // planes. Some of these planes are normal meshes, but some differ in
        // that their geometry is stored in the MDL, not MDX.
        //
        // Values stored in the MDL are vertex coordinates, texture coordinates
        // and normals. However, most of the vertex coordinates seem to be
        // procedurally generated based on vertices 0-7 and 88-95.

        // Saber Mesh Header
        uint32_t offSaberVertices = _mdl.readUint32();
        uint32_t offTexCoords = _mdl.readUint32();
        uint32_t offNormals = _mdl.readUint32();
        _mdl.skipBytes(2 * 4); // unknown

        static int referenceIndices[] {0, 1, 2, 3, 4, 5, 6, 7, 88, 89, 90, 91, 92, 93, 94, 95};

        _mdl.seek(static_cast<size_t>(kMdlDataOffset) + offSaberVertices);
        std::vector<float> saberVertices(_mdl.readFloatArray(3 * numVertices));

        _mdl.seek(static_cast<size_t>(kMdlDataOffset) + offTexCoords);
        std::vector<float> texCoords(_mdl.readFloatArray(2 * numVertices));

        _mdl.seek(static_cast<size_t>(kMdlDataOffset) + offNormals);
        std::vector<float> normals(_mdl.readFloatArray(3 * numVertices));

        int numVertices = 16;
        vertices.resize(8ll * numVertices);
        float *verticesPtr = &vertices[0];

        for (int i = 0; i < numVertices; ++i) {
            int referenceIdx = referenceIndices[i];

            // Vertex coordinates
            float *vertexCoordsPtr = &saberVertices[3ll * referenceIdx];
            *(verticesPtr++) = vertexCoordsPtr[0];
            *(verticesPtr++) = vertexCoordsPtr[1];
            *(verticesPtr++) = vertexCoordsPtr[2];

            // Normals
            float *normalsPtr = &normals[3ll * referenceIdx];
            *(verticesPtr++) = normalsPtr[0];
            *(verticesPtr++) = normalsPtr[1];
            *(verticesPtr++) = normalsPtr[2];

            // Texture coordinates
            float *texCoordsPtr = &texCoords[2ll * referenceIdx];
            *(verticesPtr++) = texCoordsPtr[0];
            *(verticesPtr++) = texCoordsPtr[1];
        }

        spec.stride = 8 * sizeof(float);
        spec.offCoords = 0;
        spec.offNormals = 3 * sizeof(float);
        spec.offUV1 = 6 * sizeof(float);
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
            face.indices[0] = faceIndices[0];
            face.indices[1] = faceIndices[1];
            face.indices[2] = faceIndices[2];
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
        faces.emplace_back(0, 13, 12);
        faces.emplace_back(0, 1, 13);
        faces.emplace_back(1, 14, 13);
        faces.emplace_back(1, 2, 14);
        faces.emplace_back(2, 15, 14);
        faces.emplace_back(2, 3, 15);
        faces.emplace_back(8, 4, 5);
        faces.emplace_back(8, 5, 9);
        faces.emplace_back(9, 5, 6);
        faces.emplace_back(9, 6, 10);
        faces.emplace_back(10, 6, 7);
        faces.emplace_back(10, 7, 11);
    }

    auto mesh = std::make_unique<Mesh>(std::move(vertices), std::move(faces), spec);

    ModelNode::UVAnimation uvAnimation;
    if (animateUV) {
        uvAnimation.dir = glm::vec2(uvDirectionX, uvDirectionY);
    }
    std::shared_ptr<Texture> diffuseMap;
    if (!texture1.empty() && texture1 != "null") {
        diffuseMap = _textures.get(texture1, TextureUsage::Diffuse);
    }
    std::shared_ptr<Texture> lightmap;
    if (!texture2.empty()) {
        lightmap = _textures.get(texture2, TextureUsage::Lightmap);
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

        std::vector<std::shared_ptr<Texture>> flareTextures;
        for (int i = 0; i < numFlares; ++i) {
            _mdl.seek(kMdlDataOffset + texNameOffsets[i]);
            std::string textureName(boost::to_lower_copy(_mdl.readString(12)));
            std::shared_ptr<Texture> texture(_textures.get(textureName));
            flareTextures.push_back(std::move(texture));
        }

        for (int i = 0; i < numFlares; ++i) {
            ModelNode::LensFlare lensFlare;
            lensFlare.texture = flareTextures[i];
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
    emitter->texture = _textures.get(texture, TextureUsage::Diffuse);
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
    reference->model = _models.get(modelResRef);
    reference->reattachable = static_cast<bool>(reattachable);

    return reference;
}

void MdlMdxReader::readControllers(uint32_t keyOffset, uint32_t keyCount, const std::vector<float> &data, bool animNode, ModelNode &node) {
    uint16_t nodeFlags;
    if (animNode) {
        if (_nodeFlags.count(node.number()) == 0) {
            return;
        } else {
            nodeFlags = 0;
        }
    } else {
        nodeFlags = node.flags();
    }

    _mdl.seek(kMdlDataOffset + keyOffset);
    for (uint32_t i = 0; i < keyCount; ++i) {
        uint32_t type = _mdl.readUint32();
        _mdl.skipBytes(2); // unknown
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

        auto fn = getControllerFn(key.type, nodeFlags);
        if (fn) {
            fn(key, data, node);
        } else {
            debug(boost::format("Unsupported MDL controller type: %d") % static_cast<int>(key.type), LogChannel::Graphics);
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

void MdlMdxReader::initControllerFn() {
    _genericControllers = std::unordered_map<uint32_t, ControllerFn> {
        {8, &readPositionController},
        {20, &readOrientationController},
        {36, &readScaleController}};
    _meshControllers = std::unordered_map<uint32_t, ControllerFn> {
        {100, &readSelfIllumColorController},
        {132, &readAlphaController}};
    _lightControllers = std::unordered_map<uint32_t, ControllerFn> {
        {76, &readColorController},
        {88, &readRadiusController},
        {96, &readShadowRadiusController},
        {100, &readVerticalDisplacementController},
        {140, &readMultiplierController}};
    _emitterControllers = std::unordered_map<uint32_t, ControllerFn> {
        {80, &readAlphaEndController},
        {84, &readAlphaStartController},
        {88, &readBirthrateController},
        {92, &readBounceCoController},
        {96, &readCombineTimeController},
        {100, &readDragController},
        {104, &readFPSController},
        {108, &readFrameEndController},
        {112, &readFrameStartController},
        {116, &readGravController},
        {120, &readLifeExpController},
        {124, &readMassController},
        {128, &readP2PBezier2Controller},
        {132, &readP2PBezier3Controller},
        {136, &readParticleRotController},
        {140, &readRandVelController},
        {144, &readSizeStartController},
        {148, &readSizeEndController},
        {152, &readSizeStartYController},
        {156, &readSizeEndYController},
        {160, &readSpreadController},
        {164, &readThresholdController},
        {168, &readVelocityController},
        {172, &readXSizeController},
        {176, &readYSizeController},
        {180, &readBlurLengthController},
        {184, &readLightingDelayController},
        {188, &readLightingRadiusController},
        {192, &readLightingScaleController},
        {196, &readLightingSubDivController},
        {200, &readLightingZigZagController},
        {216, &readAlphaMidController},
        {220, &readPercentStartController},
        {224, &readPercentMidController},
        {228, &readPercentEndController},
        {232, &readSizeMidController},
        {236, &readSizeMidYController},
        {240, &readRandomBirthRateController},
        {252, &readTargetSizeController},
        {256, &readNumControlPtsController},
        {260, &readControlPtRadiusController},
        {264, &readControlPtDelayController},
        {268, &readTangentSpreadController},
        {272, &readTangentLengthController},
        {284, &readColorMidController},
        {380, &readColorEndController},
        {392, &readColorStartController},
        {502, &readDetonateController}};
}

MdlMdxReader::ControllerFn MdlMdxReader::getControllerFn(uint32_t type, int nodeFlags) {
    ControllerFn fn;
    if (nodeFlags & MdlNodeFlags::mesh) {
        auto it = _meshControllers.find(type);
        fn = it != _meshControllers.end() ? it->second : nullptr;
    } else if (nodeFlags & MdlNodeFlags::light) {
        auto it = _lightControllers.find(type);
        fn = it != _lightControllers.end() ? it->second : nullptr;
    } else if (nodeFlags & MdlNodeFlags::emitter) {
        auto it = _emitterControllers.find(type);
        fn = it != _emitterControllers.end() ? it->second : nullptr;
    }
    if (!fn) {
        auto it = _genericControllers.find(type);
        fn = it != _genericControllers.end() ? it->second : nullptr;
    }
    return fn;
}

static inline void ensureNumColumnsEquals(int type, int expected, int actual) {
    if (actual != expected) {
        throw FormatException(str(boost::format("Controller %d: number of columns is %d, expected %d") % type % actual % expected));
    }
}

void MdlMdxReader::readFloatController(const ControllerKey &key, const std::vector<float> &data, AnimatedProperty<float> &prop) {
    bool bezier = key.numColumns & kFlagBezier;
    int numColumns = key.numColumns & ~kFlagBezier;
    ensureNumColumnsEquals(key.type, 1, numColumns);

    for (uint16_t i = 0; i < key.numRows; ++i) {
        float time = data[key.timeIndex + i];
        float value = data[key.dataIndex + (bezier ? 3 : 1) * i];
        prop.addFrame(time, value);
    }
    prop.update();
}

void MdlMdxReader::readVectorController(const ControllerKey &key, const std::vector<float> &data, AnimatedProperty<glm::vec3> &prop) {
    bool bezier = key.numColumns & kFlagBezier;
    int numColumns = key.numColumns & ~kFlagBezier;

    // HACK: workaround for s_male02 from TSLRCM
    if (numColumns == 9) {
        numColumns = 3;
    }

    ensureNumColumnsEquals(key.type, 3, numColumns);

    for (uint16_t i = 0; i < key.numRows; ++i) {
        float time = data[key.timeIndex + i];
        glm::vec3 value(glm::make_vec3(&data[key.dataIndex + (bezier ? 9 : 3) * i]));
        prop.addFrame(time, value);
    }
    prop.update();
}

void MdlMdxReader::readPositionController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readVectorController(key, data, node.position());
}

void MdlMdxReader::readOrientationController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
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
            glm::quat orientation(w, x, y, z);
            node.orientation().addFrame(time, std::move(orientation));
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
            glm::quat orientation(w, x, y, z);

            node.orientation().addFrame(time, std::move(orientation));
        }
        break;
    default:
        throw FormatException("Unexpected number of columns: " + std::to_string(key.numColumns));
    }

    node.orientation().update();
}

void MdlMdxReader::readScaleController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.scale());
}

void MdlMdxReader::readSelfIllumColorController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readVectorController(key, data, node.selfIllumColor());
}

void MdlMdxReader::readAlphaController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.alpha());
}

void MdlMdxReader::readColorController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readVectorController(key, data, node.color());
}

void MdlMdxReader::readRadiusController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.radius());
}

void MdlMdxReader::readShadowRadiusController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.shadowRadius());
}

void MdlMdxReader::readVerticalDisplacementController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.verticalDisplacement());
}

void MdlMdxReader::readMultiplierController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.multiplier());
}

void MdlMdxReader::readAlphaEndController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.alphaEnd());
}

void MdlMdxReader::readAlphaStartController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.alphaStart());
}

void MdlMdxReader::readBirthrateController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.birthrate());
}

void MdlMdxReader::readBounceCoController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.bounceCo());
}

void MdlMdxReader::readCombineTimeController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.combineTime());
}

void MdlMdxReader::readDragController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.drag());
}

void MdlMdxReader::readFPSController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.fps());
}

void MdlMdxReader::readFrameEndController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.frameEnd());
}

void MdlMdxReader::readFrameStartController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.frameStart());
}

void MdlMdxReader::readGravController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.grav());
}

void MdlMdxReader::readLifeExpController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.lifeExp());
}

void MdlMdxReader::readMassController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.mass());
}

void MdlMdxReader::readP2PBezier2Controller(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.p2pBezier2());
}

void MdlMdxReader::readP2PBezier3Controller(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.p2pBezier3());
}

void MdlMdxReader::readParticleRotController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.particleRot());
}

void MdlMdxReader::readRandVelController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.randVel());
}

void MdlMdxReader::readSizeStartController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.sizeStart());
}

void MdlMdxReader::readSizeEndController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.sizeEnd());
}

void MdlMdxReader::readSizeStartYController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.sizeStartY());
}

void MdlMdxReader::readSizeEndYController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.sizeEndY());
}

void MdlMdxReader::readSpreadController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.spread());
}

void MdlMdxReader::readThresholdController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.threshold());
}

void MdlMdxReader::readVelocityController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.velocity());
}

void MdlMdxReader::readXSizeController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.xSize());
}

void MdlMdxReader::readYSizeController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.ySize());
}

void MdlMdxReader::readBlurLengthController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.blurLength());
}

void MdlMdxReader::readLightingDelayController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.lightingDelay());
}

void MdlMdxReader::readLightingRadiusController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.lightingRadius());
}

void MdlMdxReader::readLightingScaleController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.lightingScale());
}

void MdlMdxReader::readLightingSubDivController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.lightingSubDiv());
}

void MdlMdxReader::readLightingZigZagController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.lightingZigZag());
}

void MdlMdxReader::readAlphaMidController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.alphaMid());
}

void MdlMdxReader::readPercentStartController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.percentStart());
}

void MdlMdxReader::readPercentMidController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.percentMid());
}

void MdlMdxReader::readPercentEndController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.percentEnd());
}

void MdlMdxReader::readSizeMidController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.sizeMid());
}

void MdlMdxReader::readSizeMidYController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.sizeMidY());
}

void MdlMdxReader::readRandomBirthRateController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.randomBirthRate());
}

void MdlMdxReader::readTargetSizeController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.targetSize());
}

void MdlMdxReader::readNumControlPtsController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.numControlPts());
}

void MdlMdxReader::readControlPtRadiusController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.controlPtRadius());
}

void MdlMdxReader::readControlPtDelayController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.controlPtDelay());
}

void MdlMdxReader::readTangentSpreadController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.tangentSpread());
}

void MdlMdxReader::readTangentLengthController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.tangentLength());
}

void MdlMdxReader::readColorMidController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readVectorController(key, data, node.colorMid());
}

void MdlMdxReader::readColorEndController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readVectorController(key, data, node.colorEnd());
}

void MdlMdxReader::readColorStartController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readVectorController(key, data, node.colorStart());
}

void MdlMdxReader::readDetonateController(const ControllerKey &key, const std::vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.detonate());
}

} // namespace resource

} // namespace reone
