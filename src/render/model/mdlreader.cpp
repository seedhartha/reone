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

#include "mdlreader.h"

#include <functional>

#include <boost/algorithm/string.hpp>

#include "glm/ext.hpp"

#include "../../common/collectionutil.h"
#include "../../common/log.h"
#include "../../common/streamutil.h"
#include "../../resource/gameidutil.h"
#include "../../resource/resources.h"

#include "../model/models.h"
#include "../textures.h"

using namespace std;

using namespace reone::resource;

namespace fs = boost::filesystem;

namespace reone {

namespace render {

static constexpr int kSignatureSize = 4;
static constexpr int kMdlDataOffset = 12;

struct NodeFlags {
    static constexpr int header = 1;
    static constexpr int light = 2;
    static constexpr int emitter = 4;
    static constexpr int reference = 0x10;
    static constexpr int mesh = 0x20;
    static constexpr int skin = 0x40;
    static constexpr int anim = 0x80;
    static constexpr int dangly = 0x100;
    static constexpr int aabb = 0x200;
    static constexpr int saber = 0x800;
};

static constexpr char kSignature[] = "\0\0\0\0";

typedef function<void(const MdlReader::ControllerKey &, const vector<float> &, ModelNode &)> ControllerFn;

static void readPositionController(const MdlReader::ControllerKey &key, const vector<float> &data, ModelNode &node) {
    bool bezier = key.columnCount & 16;

    switch (key.columnCount) {
        case 3:
        case 19:
            for (uint16_t i = 0; i < key.rowCount; ++i) {
                int rowTimeIdx = key.timeIndex + i;
                int rowDataIdx = key.dataIndex + i * (bezier ? 9 : 3);

                ModelNode::Keyframe frame;
                frame.time = data[rowTimeIdx];
                frame.translation = glm::make_vec3(&data[rowDataIdx]);

                node.addTranslationKeyframe(move(frame));
            }
            break;
        default:
            break;
    }
}

static void readOrientationController(const MdlReader::ControllerKey &key, const vector<float> &data, ModelNode &node) {
    switch (key.columnCount) {
        case 2:
            for (uint16_t i = 0; i < key.rowCount; ++i) {
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

                ModelNode::Keyframe frame;
                frame.time = data[rowTimeIdx];
                frame.orientation = glm::quat(w, x, y, z);

                node.addOrientationKeyframe(move(frame));
            }
            break;

        case 4:
            for (uint16_t i = 0; i < key.rowCount; ++i) {
                int rowTimeIdx = key.timeIndex + i;
                int rowDataIdx = key.dataIndex + i * 4;

                ModelNode::Keyframe frame;
                frame.time = data[rowTimeIdx];
                frame.orientation.x = data[rowDataIdx + 0];
                frame.orientation.y = data[rowDataIdx + 1];
                frame.orientation.z = data[rowDataIdx + 2];
                frame.orientation.w = data[rowDataIdx + 3];

                node.addOrientationKeyframe(move(frame));
            }
            break;
        default:
            break;
    }
}

static void readScaleController(const MdlReader::ControllerKey &key, const vector<float> &data, ModelNode &node) {
    for (uint16_t i = 0; i < key.rowCount; ++i) {
        ModelNode::Keyframe frame;
        frame.time = data[key.timeIndex + i];
        frame.scale = data[key.dataIndex + i];

        node.addScaleKeyframe(move(frame));
    }
}

static void readColorController(const MdlReader::ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.setColor(glm::make_vec3(&data[key.dataIndex]));
}

static void readSelfIllumColorController(const MdlReader::ControllerKey &key, const vector<float> &data, ModelNode &node) {
    if (key.rowCount == 1) {
        node.setSelfIllumColor(glm::make_vec3(&data[key.dataIndex]));
        return;
    }

    for (uint16_t i = 0; i < key.rowCount; ++i) {
        ModelNode::Keyframe frame;
        frame.time = data[key.timeIndex + i];
        frame.selfIllumColor = glm::make_vec3(&data[key.dataIndex + 3 * i]);

        node.addSelfIllumColorKeyframe(move(frame));
    }
}

static void readAlphaController(const MdlReader::ControllerKey &key, const vector<float> &data, ModelNode &node) {
    if (key.rowCount == 1) {
        node.setAlpha(data[key.dataIndex]);
        return;
    }

    for (uint16_t i = 0; i < key.rowCount; ++i) {
        ModelNode::Keyframe frame;
        frame.time = data[key.timeIndex + i];
        frame.alpha = data[key.dataIndex + i];

        node.addAlphaKeyframe(move(frame));
    }
}

static void readRadiusController(const MdlReader::ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.light()->radius = data[key.dataIndex];
}

static void readBirthrateController(const MdlReader::ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->setBirthrate(static_cast<int>(data[key.dataIndex]));
}

static void readMultiplierController(const MdlReader::ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.light()->multiplier = data[key.dataIndex];
}

static void readLifeExpectancyController(const MdlReader::ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->setLifeExpectancy(static_cast<int>(data[key.dataIndex]));
}

static void readSizeStartController(const MdlReader::ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->particleSize().start = data[key.dataIndex];
}

static void readSizeMidController(const MdlReader::ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->particleSize().mid = data[key.dataIndex];
}

static void readSizeEndController(const MdlReader::ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->particleSize().end = data[key.dataIndex];
}

static void readColorStartController(const MdlReader::ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->color().start = glm::make_vec3(&data[key.dataIndex]);
}

static void readColorMidController(const MdlReader::ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->color().mid = glm::make_vec3(&data[key.dataIndex]);
}

static void readColorEndController(const MdlReader::ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->color().end = glm::make_vec3(&data[key.dataIndex]);
}

static void readAlphaStartController(const MdlReader::ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->alpha().start = data[key.dataIndex];
}

static void readAlphaMidController(const MdlReader::ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->alpha().mid = data[key.dataIndex];
}

static void readAlphaEndController(const MdlReader::ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->alpha().end = data[key.dataIndex];
}

static void readSizeXController(const MdlReader::ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->size().x = data[key.dataIndex];
}

static void readSizeYController(const MdlReader::ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->size().y = data[key.dataIndex];
}

static void readFrameStartController(const MdlReader::ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->setFrameStart(static_cast<int>(data[key.dataIndex]));
}

static void readFrameEndController(const MdlReader::ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->setFrameEnd(static_cast<int>(data[key.dataIndex]));
}

static void readVelocityController(const MdlReader::ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->setVelocity(data[key.dataIndex]);
}

static void readRandomVelocityController(const MdlReader::ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->setRandomVelocity(data[key.dataIndex]);
}

static void readSpreadController(const MdlReader::ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->setSpread(data[key.dataIndex]);
}

static void readFPSController(const MdlReader::ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->setFPS(static_cast<int>(data[key.dataIndex]));
}

static const unordered_map<uint32_t, ControllerFn> g_genericControllers {
    { 8, &readPositionController },
    { 20, &readOrientationController },
    { 36, &readScaleController },
    { 76, &readColorController },
    { 100, &readSelfIllumColorController },
    { 132, &readAlphaController }
};

static const unordered_map<uint32_t, ControllerFn> g_lightControllers {
    { 88, &readRadiusController },
    { 140, &readMultiplierController }
};

static const unordered_map<uint32_t, ControllerFn> g_emitterControllers {
    { 88, &readBirthrateController },
    { 104, &readFPSController },
    { 108, &readFrameEndController },
    { 112, &readFrameStartController },
    { 120, &readLifeExpectancyController },
    { 140, &readRandomVelocityController },
    { 144, &readSizeStartController },
    { 148, &readSizeEndController },
    { 160, &readSpreadController },
    { 168, &readVelocityController },
    { 172, &readSizeXController },
    { 176, &readSizeYController },
    { 216, &readAlphaMidController },
    { 232, &readSizeMidController },
    { 284, &readColorMidController },
    { 380, &readColorEndController },
    { 392, &readColorStartController }
};

MdlReader::MdlReader(GameID gameId) : BinaryReader(kSignatureSize, kSignature), _gameId(gameId) {
}

void MdlReader::load(const shared_ptr<istream> &mdl, const shared_ptr<istream> &mdx) {
    _mdx = mdx;
    _mdxReader = make_unique<StreamReader>(mdx);

    BinaryReader::load(mdl);
}

void MdlReader::doLoad() {
    uint32_t mdlDataSize = readUint32();
    uint32_t mdxSize = readUint32();

    ignore(8);

    _name = readCString(32);
    boost::to_lower(_name);

    uint32_t rootNodeOffset = readUint32();
    uint32_t nodeCount = readUint32();

    ignore(28);

    uint8_t type = readByte();

    ignore(3);

    uint8_t classificationVal = readByte();
    _classification = getClassification(classificationVal);

    ignore(2);

    uint8_t fogged = readByte();

    ignore(4);

    uint32_t animOffOffset, animCount;
    readArrayDefinition(animOffOffset, animCount);
    vector<uint32_t> animOffsets(readArray<uint32_t>(kMdlDataOffset + animOffOffset, animCount));

    ignore(28);

    float radius = readFloat();
    float scale = readFloat();

    string superModelName(readCString(32));
    boost::to_lower(superModelName);

    ignore(16);

    uint32_t nameOffOffset, nameCount;
    readArrayDefinition(nameOffOffset, nameCount);
    vector<uint32_t> nameOffsets(readArray<uint32_t>(kMdlDataOffset + nameOffOffset, nameCount));
    readNodeNames(nameOffsets);

    unique_ptr<ModelNode> rootNode(readNode(kMdlDataOffset + rootNodeOffset, nullptr));
    rootNode->computeAbsoluteTransforms();

    vector<shared_ptr<Animation>> anims(readAnimations(animOffsets));
    shared_ptr<Model> superModel;

    if (!superModelName.empty() && superModelName != "null") {
        superModel = Models::instance().get(superModelName);
    }

    _model = make_unique<Model>(_name, _classification, move(rootNode), anims, superModel);
    _model->setAnimationScale(scale);
}

void MdlReader::readArrayDefinition(uint32_t &offset, uint32_t &count) {
    offset = readUint32();
    count = readUint32();
    ignore(4);
}

void MdlReader::readNodeNames(const vector<uint32_t> &offsets) {
    map<string, int> nameHits;

    for (uint32_t offset : offsets) {
        string name(readCStringAt(kMdlDataOffset + offset));
        boost::to_lower(name);

        int hitCount = nameHits[name]++;
        if (hitCount > 0) {
            warn("MDL: duplicate node name: " + name);
            name = str(boost::format("%s_dup%d") % name % hitCount);
        }
        _nodeNames.push_back(move(name));
    }
}

unique_ptr<ModelNode> MdlReader::readNode(uint32_t offset, ModelNode *parent) {
    size_t pos = tell();
    seek(offset);

    uint16_t flags = readUint16();
    if (flags & 0xf408) {
        throw runtime_error("MDL: unsupported node flags: " + to_string(flags));
    }

    ignore(2);

    uint16_t nodeNumber = readUint16();
    string name(_nodeNames[nodeNumber]);

    ignore(10);

    vector<float> positionValues(readArray<float>(3));
    glm::vec3 position(glm::make_vec3(&positionValues[0]));

    vector<float> orientationValues(readArray<float>(4));
    glm::quat orientation(orientationValues[0], orientationValues[1], orientationValues[2], orientationValues[3]);

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, position);
    transform *= glm::mat4_cast(orientation);

    uint32_t childOffOffset, childCount;
    readArrayDefinition(childOffOffset, childCount);
    vector<uint32_t> childOffsets(readArray<uint32_t>(kMdlDataOffset + childOffOffset, childCount));

    uint32_t controllerKeyOffset, controllerKeyCount;
    readArrayDefinition(controllerKeyOffset, controllerKeyCount);

    uint32_t controllerDataOffset, controllerDataCount;
    readArrayDefinition(controllerDataOffset, controllerDataCount);
    vector<float> controllerData(readArray<float>(kMdlDataOffset + controllerDataOffset, controllerDataCount));

    auto node = make_unique<ModelNode>(_nodeIndex++, parent);
    node->_flags = flags;
    node->_nodeNumber = nodeNumber;
    node->_name = name;
    node->_position = position;
    node->_orientation = orientation;
    node->_localTransform = transform;

    if (flags & NodeFlags::light) {
        node->_light = make_shared<ModelNode::Light>();
    }
    if (flags & NodeFlags::emitter) {
        node->_emitter = make_shared<Emitter>();
    }

    readControllers(controllerKeyCount, controllerKeyOffset, controllerData, *node);

    if (flags & NodeFlags::light) {
        readLight(*node);
    }
    if (flags & NodeFlags::emitter) {
        readEmitter(*node);
    }
    if (flags & NodeFlags::reference) {
        readReference(*node);
    }
    if (flags & NodeFlags::mesh) {
        node->_mesh = readMesh(name, flags);
    }
    if (flags & NodeFlags::skin) {
        readSkin(*node);
    }
    if (flags & NodeFlags::aabb) {
        node->_aabb = true;
    }
    if (flags & NodeFlags::saber) {
        node->_saber = true;
    }

    for (auto offset : childOffsets) {
        unique_ptr<ModelNode> child(readNode(kMdlDataOffset + offset, node.get()));
        node->_children.push_back(move(child));
    }

    seek(pos);

    return move(node);
}

static function<void(const MdlReader::ControllerKey &, const vector<float> &, ModelNode &)> getControllerFn(uint32_t type, int nodeFlags) {
    ControllerFn fn;
    if (nodeFlags & NodeFlags::light) {
        fn = getFromLookupOrNull(g_lightControllers, type);
    } else if (nodeFlags & NodeFlags::emitter) {
        fn = getFromLookupOrNull(g_emitterControllers, type);
    }
    if (!fn) {
        fn = getFromLookupOrNull(g_genericControllers, type);
    }
    return move(fn);
}

void MdlReader::readControllers(uint32_t keyCount, uint32_t keyOffset, const vector<float> &data, ModelNode &node) {
    size_t pos = tell();
    seek(kMdlDataOffset + keyOffset);

    vector<ControllerKey> keys(readArray<ControllerKey>(keyCount));
    for (ControllerKey key : keys) {
        auto fn = getControllerFn(key.type, node._flags);
        if (fn) {
            fn(key, data, node);
        } else {
            debug(boost::format("Unsupported MDL controller: %d") % static_cast<int>(key.type), 3);
        }
    }

    seek(pos);
}

void MdlReader::readLight(ModelNode &node) {
    ignore(64);

    node._light->priority = readUint32();
    node._light->ambientOnly = static_cast<bool>(readUint32());
    node._light->dynamicType = readUint32();
    node._light->affectDynamic = static_cast<bool>(readUint32());
    node._light->shadow = static_cast<bool>(readUint32());

    ignore(8);
}

void MdlReader::readReference(ModelNode &node) {
    string modelResRef(boost::to_lower_copy(readCString(32)));
    bool reattachable = static_cast<bool>(readUint32());

    node._reference = make_shared<ModelNode::Reference>();
    node._reference->model = Models::instance().get(modelResRef);
    node._reference->reattachable = reattachable;
}

unique_ptr<ModelMesh> MdlReader::readMesh(const string &nodeName, int nodeFlags) {
    ignore(8);

    uint32_t faceOffset, faceCount;
    readArrayDefinition(faceOffset, faceCount);

    ignore(40);

    vector<float> diffuseColor(readArray<float>(3));
    vector<float> ambientColor(readArray<float>(3));
    uint32_t transparency = readUint32();
    string diffuse(boost::to_lower_copy(readCString(32)));
    string lightmap(boost::to_lower_copy(readCString(32)));

    ignore(36);

    uint32_t indexOffOffset, indexCount;
    readArrayDefinition(indexOffOffset, indexCount);

    ignore(32);

    bool animateUv = readUint32() != 0;
    float uvDirectionX = readFloat();
    float uvDirectionY = readFloat();
    float uvJitter = readFloat();
    float uvJitterSpeed = readFloat();

    uint32_t mdxVertexSize = readUint32();

    ignore(4);

    uint32_t mdxVerticesOffset = readUint32();
    uint32_t mdxNormalsOffset = readUint32();

    ignore(4);

    uint32_t mdxTextureOffset = readUint32();
    uint32_t mdxLightmapOffset = readUint32();

    ignore(8);

    uint32_t mdxTanSpaceOffset = readUint32();

    ignore(12);

    uint16_t vertexCount = readUint16();
    uint16_t textureCount = readUint16();

    ignore(2);

    uint8_t backgroundGeometry = readByte();
    uint8_t shadow = readByte();

    ignore(1);

    uint8_t render = readByte();

    ignore(10);

    if (isTSL(_gameId)) ignore(8);

    uint32_t mdxDataOffset = readUint32();
    uint32_t vertCoordsOffset = readUint32();
    size_t endPos = tell();

    if (faceCount == 0) return nullptr;
    if (mdxVertexSize == 0 && !(nodeFlags & NodeFlags::saber)) return nullptr;

    vector<float> vertices;
    Mesh::VertexOffsets offsets;
    vector<uint16_t> indices;

    if (nodeFlags & NodeFlags::saber) {
        // Lightsaber blade is a special case. It consists of four to eight
        // planes. Some of these planes are normal meshes, but some differ in
        // that their geometry is stored in the MDL, not MDX.
        //
        // Values stored in the MDL are vertex coordinates, texture coordinates
        // and normals. However, most of the vertex coordinates seem to be
        // procedurally generated based on vertices 0-7 and 88-95.

        static int referenceIndices[] = { 0, 1, 2, 3, 4, 5, 6, 7, 88, 89, 90, 91, 92, 93, 94, 95 };

        uint32_t saberCoordsOffset = readUint32();
        uint32_t texCoordsOffset = readUint32();
        uint32_t normalsOffset = readUint32();

        seek(static_cast<size_t>(kMdlDataOffset) + saberCoordsOffset);
        vector<float> vertexCoords(readArray<float>(3 * vertexCount));

        seek(static_cast<size_t>(kMdlDataOffset) + texCoordsOffset);
        vector<float> texCoords(readArray<float>(2 * vertexCount));

        seek(static_cast<size_t>(kMdlDataOffset) + normalsOffset);
        vector<float> normals(readArray<float>(3 * vertexCount));

        vertexCount = 16;
        vertices.resize(8ll * vertexCount);
        float *verticesPtr = &vertices[0];

        for (int i = 0; i < vertexCount; ++i) {
            int referenceIdx = referenceIndices[i];

            // Vertex coordinates
            float *vertexCoordsPtr = &vertexCoords[3ll * referenceIdx];
            *(verticesPtr++) = vertexCoordsPtr[0];
            *(verticesPtr++) = vertexCoordsPtr[1];
            *(verticesPtr++) = vertexCoordsPtr[2];

            // Texture coordinates
            float *texCoordsPtr = &texCoords[2ll * referenceIdx];
            *(verticesPtr++) = texCoordsPtr[0];
            *(verticesPtr++) = texCoordsPtr[1];

            // Normals
            float *normalsPtr = &normals[3ll * referenceIdx];
            *(verticesPtr++) = normalsPtr[0];
            *(verticesPtr++) = normalsPtr[1];
            *(verticesPtr++) = normalsPtr[2];
        }

        offsets.vertexCoords = 0;
        offsets.texCoords1 = 3 * sizeof(float);
        offsets.normals = 5 * sizeof(float);
        offsets.stride = 8 * sizeof(float);

        indices = {
            0, 13, 12, 0, 1, 13,
            1, 14, 13, 1, 2, 14,
            2, 15, 14, 2, 3, 15,
            8, 4, 5, 8, 5, 9,
            9, 5, 6, 9, 6, 10,
            10, 6, 7, 10, 7, 11
        };

    } else {
        int valPerVert = mdxVertexSize / sizeof(float);
        int vertValCount = valPerVert * vertexCount;
        _mdxReader->seek(mdxDataOffset);
        vertices = _mdxReader->getArray<float>(vertValCount);

        offsets.vertexCoords = mdxVerticesOffset;
        offsets.normals = mdxNormalsOffset != 0xffffffff ? mdxNormalsOffset : -1;
        offsets.texCoords1 = mdxTextureOffset != 0xffffffff ? mdxTextureOffset : -1;
        offsets.texCoords2 = mdxLightmapOffset != 0xffffffff ? mdxLightmapOffset : -1;
        if (mdxTanSpaceOffset != 0xffffffff) {
            offsets.bitangents = mdxTanSpaceOffset + 0 * sizeof(float);
            offsets.tangents = mdxTanSpaceOffset + 3 * sizeof(float);
        }
        offsets.stride = mdxVertexSize;

        seek(kMdlDataOffset + indexOffOffset);
        uint32_t indexOffset = readUint32();
        seek(kMdlDataOffset + indexOffset);
        indices = readArray<uint16_t>(3 * faceCount);
    }

    seek(endPos);

    auto mesh = make_unique<Mesh>(vertexCount, move(vertices), move(indices), move(offsets));
    mesh->computeAABB();

    auto modelMesh = make_unique<ModelMesh>(move(mesh));
    modelMesh->setRender(render);
    modelMesh->setTransparency(transparency);
    modelMesh->setShadow(shadow);
    modelMesh->setBackgroundGeometry(backgroundGeometry != 0);
    modelMesh->setDiffuseColor(glm::make_vec3(&diffuseColor[0]));
    modelMesh->setAmbientColor(glm::make_vec3(&ambientColor[0]));

    if (!diffuse.empty() && diffuse != "null") {
        modelMesh->_diffuse = Textures::instance().get(diffuse, TextureUsage::Diffuse);
    }
    if (!lightmap.empty()) {
        modelMesh->_lightmap = Textures::instance().get(lightmap, TextureUsage::Lightmap);
    }
    if (animateUv) {
        modelMesh->_uvAnimation.animated = true;
        modelMesh->_uvAnimation.directionX = uvDirectionX;
        modelMesh->_uvAnimation.directionY = uvDirectionY;
        modelMesh->_uvAnimation.jitter = uvJitter;
        modelMesh->_uvAnimation.jitterSpeed = uvJitterSpeed;
    }

    return move(modelMesh);
}

void MdlReader::readSkin(ModelNode &node) {
    ignore(12);

    uint32_t boneWeightsOffset = readUint32();
    uint32_t boneIndicesOffset = readUint32();
    uint32_t bonesOffset = readUint32();
    uint32_t boneCount = readUint32();

    node._mesh->_mesh->_offsets.boneWeights = boneWeightsOffset;
    node._mesh->_mesh->_offsets.boneIndices = boneIndicesOffset;

    unordered_map<uint16_t, uint16_t> nodeIdxByBoneIdx;
    seek(kMdlDataOffset + bonesOffset);

    for (uint32_t i = 0; i < boneCount; ++i) {
        uint16_t boneIdx = static_cast<uint16_t>(readFloat());
        if (boneIdx == 0xffff) continue;

        uint16_t nodeIdx = i;
        nodeIdxByBoneIdx.insert(make_pair(boneIdx, nodeIdx));
    }

    node._skin = make_unique<ModelNode::Skin>();
    node._skin->nodeIdxByBoneIdx = move(nodeIdxByBoneIdx);
}

vector<shared_ptr<Animation>> MdlReader::readAnimations(const vector<uint32_t> &offsets) {
    vector<shared_ptr<Animation>> anims;
    anims.reserve(offsets.size());

    for (uint32_t offset : offsets) {
        anims.push_back(readAnimation(offset));
    }

    return move(anims);
}

unique_ptr<Animation> MdlReader::readAnimation(uint32_t offset) {
    seek(kMdlDataOffset + offset);
    ignore(8);

    string name(readCString(32));
    boost::to_lower(name);

    uint32_t rootNodeOffset = readUint32();

    ignore(36);

    float length = readFloat();
    float transitionTime = readFloat();

    ignore(32);

    vector<Animation::Event> events;
    int eventsOffset = readInt32();
    int eventsCount = readInt32();
    if (eventsCount > 0) {
        seek(kMdlDataOffset + eventsOffset);
        for (int i = 0; i < eventsCount; ++i) {
            Animation::Event event;
            event.time = readFloat();
            event.name = boost::to_lower_copy(readCString(32));
            events.push_back(move(event));
        }
        sort(events.begin(), events.end(), [](auto &left, auto &right) { return left.time < right.time; });
    }

    _nodeIndex = 0;
    unique_ptr<ModelNode> rootNode(readNode(kMdlDataOffset + rootNodeOffset, nullptr));

    return make_unique<Animation>(name, length, transitionTime, move(events), move(rootNode));
}

Model::Classification MdlReader::getClassification(int value) const {
    switch (value) {
        case 1:
            return Model::Classification::Effect;
        case 2:
            return Model::Classification::Tile;
        case 4:
            return Model::Classification::Character;
        case 8:
            return Model::Classification::Door;
        case 0x10:
            return Model::Classification::Lightsaber;
        case 0x20:
            return Model::Classification::Placeable;
        case 0x40:
            return Model::Classification::Flyer;
        default:
            return Model::Classification::Other;
    }
}

static Emitter::UpdateMode parseEmitterUpdate(const string &str) {
    auto result = Emitter::UpdateMode::Invalid;
    if (str == "Fountain") {
        result = Emitter::UpdateMode::Fountain;
    } else if (str == "Single") {
        result = Emitter::UpdateMode::Single;
    } else if (str == "Explosion") {
        result = Emitter::UpdateMode::Explosion;
    } else {
        warn("parseEmitterUpdate: unsupported value: " + str);
    }
    return result;
}

static Emitter::RenderMode parseEmitterRender(const string &str) {
    auto result = Emitter::RenderMode::Invalid;
    if (str == "Normal") {
        result = Emitter::RenderMode::Normal;
    } else if (str == "Billboard_to_World_Z") {
        result = Emitter::RenderMode::BillboardToWorldZ;
    } else if (str == "Motion_Blur") {
        result = Emitter::RenderMode::MotionBlur;
    } else if (str == "Billboard_to_Local_Z") {
        result = Emitter::RenderMode::BillboardToLocalZ;
    } else if (str == "Aligned_to_Particle_Dir") {
        result = Emitter::RenderMode::AlignedToParticleDir;
    } else {
        warn("parseEmitterRender: unsupported value: " + str);
    }
    return result;
}

static Emitter::BlendMode parseEmitterBlend(const string &str) {
    auto result = Emitter::BlendMode::Invalid;
    if (str == "Normal") {
        result = Emitter::BlendMode::Normal;
    } else if (str == "Punch") {
        result = Emitter::BlendMode::Punch;
    } else if (str == "Lighten") {
        result = Emitter::BlendMode::Lighten;
    } else {
        warn("parseEmitterBlend: unsupported value: " + str);
    }
    return result;
}

void MdlReader::readEmitter(ModelNode &node) {
    ignore(5 * 4);

    node._emitter->_gridWidth = glm::max(readUint32(), 1u);
    node._emitter->_gridHeight = glm::max(readUint32(), 1u);

    ignore(4);

    node._emitter->_updateMode = parseEmitterUpdate(readCString(32));
    node._emitter->_renderMode = parseEmitterRender(readCString(32));
    node._emitter->_blendMode = parseEmitterBlend(readCString(32));
    node._emitter->_texture = Textures::instance().get(boost::to_lower_copy(readCString(32)), TextureUsage::Diffuse);

    ignore(20);

    node._emitter->_loop = readUint32() != 0;
    node._emitter->_renderOrder = readUint16();

    ignore(30);
}

shared_ptr<Model> MdlModelLoader::loadModel(GameID gameId, const string &resRef) {
    shared_ptr<ByteArray> mdlData(Resources::instance().get(resRef, ResourceType::Mdl));
    shared_ptr<ByteArray> mdxData(Resources::instance().get(resRef, ResourceType::Mdx));
    if (mdlData && mdxData) {
        MdlReader mdl(gameId);
        mdl.load(wrap(mdlData), wrap(mdxData));
        return mdl.model();
    }
    return nullptr;
}

} // namespace render

} // namespace reone
