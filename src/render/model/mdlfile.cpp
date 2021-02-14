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

#include "mdlfile.h"

#include <boost/algorithm/string.hpp>

#include "glm/ext.hpp"

#include "../../common/log.h"
#include "../../common/streamutil.h"
#include "../../resource/resources.h"

#include "../materials.h"
#include "../model/models.h"
#include "../textures.h"

using namespace std;

using namespace reone::resource;

namespace fs = boost::filesystem;

namespace reone {

namespace render {

static constexpr int kSignatureSize = 4;
static constexpr int kMdlDataOffset = 12;

static constexpr int kNodeHasHeader = 1;
static constexpr int kNodeHasLight = 2;
static constexpr int kNodeHasEmitter = 4;
static constexpr int kNodeHasReference = 16;
static constexpr int kNodeHasMesh = 32;
static constexpr int kNodeHasSkin = 64;
static constexpr int kNodeHasAnim = 128;
static constexpr int kNodeHasDangly = 256;
static constexpr int kNodeHasAABB = 512;
static constexpr int kNodeHasSaber = 2048;

static const char kSignature[] = "\0\0\0\0";

enum class ControllerType {
    Position = 8,
    Orientation = 20,
    Scale = 36,
    Color = 76,
    AlphaEnd = 80,
    AlphaStart = 84,
    Radius_Birthrate = 88,
    SelfIllumColor = 100,
    FPS = 104,
    FrameEnd = 108,
    FrameStart = 112,
    LifeExpectancy = 120,
    Alpha = 132,
    Multiplier_RandomVelocity = 140,
    SizeStart = 144,
    SizeEnd = 148,
    Spread = 160,
    Velocity = 168,
    SizeX = 172,
    SizeY = 176,
    AlphaMid = 216,
    SizeMid = 232,
    ColorMid = 284,
    ColorEnd = 380,
    ColorStart = 392
};

MdlFile::MdlFile(GameID gameId) : BinaryFile(kSignatureSize, kSignature), _gameId(gameId) {
}

void MdlFile::load(const shared_ptr<istream> &mdl, const shared_ptr<istream> &mdx) {
    _mdx = mdx;
    _mdxReader = make_unique<StreamReader>(mdx);

    BinaryFile::load(mdl);
}

void MdlFile::doLoad() {
    if (!_mdx) openMDX();

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

void MdlFile::openMDX() {
    fs::path mdxPath(_path);
    mdxPath.replace_extension(".mdx");

    if (!fs::exists(mdxPath)) {
        throw runtime_error("MDL: MDX file not found: " + mdxPath.string());
    }
    _mdx = make_unique<fs::ifstream>(mdxPath, ios::binary);
    _mdxReader = make_unique<StreamReader>(_mdx);
}

void MdlFile::readArrayDefinition(uint32_t &offset, uint32_t &count) {
    offset = readUint32();
    count = readUint32();
    ignore(4);
}

void MdlFile::readNodeNames(const vector<uint32_t> &offsets) {
    map<string, int> nameHits;

    for (uint32_t offset : offsets) {
        string name(readCStringAt(kMdlDataOffset + offset));
        boost::to_lower(name);

        int hitCount = nameHits[name]++;
        if (hitCount > 0) {
            warn(boost::format("MDL: duplicate node name: %s, model %s") % name % _name);
            name = str(boost::format("%s_dup%d") % name % hitCount);
        }
        _nodeNames.push_back(move(name));
    }
}

unique_ptr<ModelNode> MdlFile::readNode(uint32_t offset, ModelNode *parent) {
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

    if (flags & kNodeHasEmitter) {
        node->_emitter = make_shared<Emitter>();
    }

    readControllers(controllerKeyCount, controllerKeyOffset, controllerData, *node);

    if (flags & kNodeHasLight) {
        readLight(*node);
    }
    if (flags & kNodeHasEmitter) {
        readEmitter(*node);
    }
    if (flags & kNodeHasReference) {
        ignore(68);
    }
    if (flags & kNodeHasMesh) {
        node->_mesh = readMesh(name, flags);
    }
    if (flags & kNodeHasSkin) {
        readSkin(*node);
    }
    if (flags & kNodeHasSaber) {
        node->_saber = true;
    }

    for (auto offset : childOffsets) {
        unique_ptr<ModelNode> child(readNode(kMdlDataOffset + offset, node.get()));
        node->_children.push_back(move(child));
    }

    seek(pos);

    return move(node);
}

void MdlFile::readControllers(uint32_t keyCount, uint32_t keyOffset, const vector<float> &data, ModelNode &node) {
    size_t pos = tell();
    seek(kMdlDataOffset + keyOffset);

    for (uint32_t i = 0; i < keyCount; ++i) {
        ControllerType type = static_cast<ControllerType>(readUint32());

        ignore(2);

        uint16_t rowCount = readUint16();
        uint16_t timeIndex = readUint16();
        uint16_t dataIndex = readUint16();
        uint8_t columnCount = readByte();

        ignore(3);

        switch (type) {
            case ControllerType::Position:
                readPositionController(rowCount, columnCount, timeIndex, dataIndex, data, node);
                break;
            case ControllerType::Orientation:
                readOrientationController(rowCount, columnCount, timeIndex, dataIndex, data, node);
                break;
            case ControllerType::Scale:
                readScaleController(rowCount, timeIndex, dataIndex, data, node);
                break;
            case ControllerType::Color:
                readColorController(dataIndex, data, node);
                break;
            case ControllerType::Radius_Birthrate:
                if (node._flags & kNodeHasLight) {
                    readRadiusController(dataIndex, data, node);
                } else if (node._flags & kNodeHasEmitter) {
                    readBirthrateController(dataIndex, data, node);
                }
                break;
            case ControllerType::SelfIllumColor:
                if (node._flags & kNodeHasMesh) {
                    readSelfIllumColorController(dataIndex, data, node);
                    node._selfIllumEnabled = glm::length(node._selfIllumColor) > 0.0f;
                }
                break;
            case ControllerType::FPS:
                if (node._flags & kNodeHasEmitter) {
                    readFPSController(dataIndex, data, node);
                }
                break;
            case ControllerType::FrameEnd:
                if (node._flags & kNodeHasEmitter) {
                    readFrameEndController(dataIndex, data, node);
                }
                break;
            case ControllerType::FrameStart:
                if (node._flags & kNodeHasEmitter) {
                    readFrameStartController(dataIndex, data, node);
                }
                break;
            case ControllerType::LifeExpectancy:
                if (node._flags & kNodeHasEmitter) {
                    readLifeExpectancyController(dataIndex, data, node);
                }
                break;
            case ControllerType::Alpha:
                readAlphaController(dataIndex, data, node);
                break;
            case ControllerType::Multiplier_RandomVelocity:
                if (node._flags & kNodeHasLight) {
                    readMultiplierController(dataIndex, data, node);
                } else if (node._flags & kNodeHasEmitter) {
                    readRandomVelocityController(dataIndex, data, node);
                }
                break;
            case ControllerType::Spread:
                if (node._flags & kNodeHasEmitter) {
                    readSpreadController(dataIndex, data, node);
                }
                break;
            case ControllerType::Velocity:
                if (node._flags & kNodeHasEmitter) {
                    readVelocityController(dataIndex, data, node);
                }
                break;
            case ControllerType::SizeX:
                if (node._flags & kNodeHasEmitter) {
                    readSizeXController(dataIndex, data, node);
                }
                break;
            case ControllerType::SizeY:
                if (node._flags & kNodeHasEmitter) {
                    readSizeYController(dataIndex, data, node);
                }
                break;

            case ControllerType::SizeStart:
                if (node._flags & kNodeHasEmitter) {
                    readSizeStartController(dataIndex, data, node);
                }
                break;
            case ControllerType::SizeMid:
                if (node._flags & kNodeHasEmitter) {
                    readSizeMidController(dataIndex, data, node);
                }
                break;
            case ControllerType::SizeEnd:
                if (node._flags & kNodeHasEmitter) {
                    readSizeEndController(dataIndex, data, node);
                }
                break;

            case ControllerType::ColorStart:
                if (node._flags & kNodeHasEmitter) {
                    readColorStartController(dataIndex, data, node);
                }
                break;
            case ControllerType::ColorMid:
                if (node._flags & kNodeHasEmitter) {
                    readColorMidController(dataIndex, data, node);
                }
                break;
            case ControllerType::ColorEnd:
                if (node._flags & kNodeHasEmitter) {
                    readColorEndController(dataIndex, data, node);
                }
                break;

            case ControllerType::AlphaStart:
                if (node._flags & kNodeHasEmitter) {
                    readAlphaStartController(dataIndex, data, node);
                }
                break;
            case ControllerType::AlphaMid:
                if (node._flags & kNodeHasEmitter) {
                    readAlphaMidController(dataIndex, data, node);
                }
                break;
            case ControllerType::AlphaEnd:
                if (node._flags & kNodeHasEmitter) {
                    readAlphaEndController(dataIndex, data, node);
                }
                break;

            default:
                debug(boost::format("MDL: unsupported controller type: \"%s\" %d") % _name % static_cast<int>(type), 3);
                break;
        }
    }

    seek(pos);
}

void MdlFile::readPositionController(uint16_t rowCount, uint8_t columnCount, uint16_t timeIndex, uint16_t dataIndex, const vector<float> &data, ModelNode &node) {
    bool bezier = columnCount & 16;
    node._translationFrames.reserve(rowCount);

    switch (columnCount) {
        case 3:
        case 19:
            for (int i = 0; i < rowCount; ++i) {
                int rowTimeIdx = timeIndex + i;
                int rowDataIdx = dataIndex + i * (bezier ? 9 : 3);

                ModelNode::TranslationKeyframe frame;
                frame.time = data[rowTimeIdx];
                frame.translation = glm::make_vec3(&data[rowDataIdx]);

                node._translationFrames.push_back(move(frame));
            }
            break;
        default:
            break;
    }
}

void MdlFile::readOrientationController(uint16_t rowCount, uint8_t columnCount, uint16_t timeIndex, uint16_t dataIndex, const vector<float> &data, ModelNode &node) {
    node._orientationFrames.reserve(rowCount);

    switch (columnCount) {
        case 2:
            for (int i = 0; i < rowCount; ++i) {
                int rowTimeIdx = timeIndex + i;
                int rowDataIdx = dataIndex + i;

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

                ModelNode::OrientationKeyframe frame;
                frame.time = data[rowTimeIdx];
                frame.orientation = glm::quat(w, x, y, z);

                node._orientationFrames.push_back(move(frame));
            }
            break;

        case 4:
            for (int i = 0; i < rowCount; ++i) {
                int rowTimeIdx = timeIndex + i;
                int rowDataIdx = dataIndex + i * 4;

                ModelNode::OrientationKeyframe frame;
                frame.time = data[rowTimeIdx];
                frame.orientation.x = data[rowDataIdx + 0];
                frame.orientation.y = data[rowDataIdx + 1];
                frame.orientation.z = data[rowDataIdx + 2];
                frame.orientation.w = data[rowDataIdx + 3];

                node._orientationFrames.push_back(move(frame));
            }
            break;
        default:
            break;
    }
}

void MdlFile::readScaleController(uint16_t rowCount, uint16_t timeIndex, uint16_t dataIndex, const vector<float> &data, ModelNode &node) {
    node._scaleFrames.resize(rowCount);

    for (int i = 0; i < rowCount; ++i) {
        ModelNode::ScaleKeyframe frame;
        frame.time = data[timeIndex + i];
        frame.scale = data[dataIndex + i];

        node._scaleFrames[i] = move(frame);
    }
}

void MdlFile::readColorController(uint16_t dataIndex, const vector<float> &data, ModelNode &node) {
    node._color.r = data[dataIndex + 0];
    node._color.g = data[dataIndex + 1];
    node._color.b = data[dataIndex + 2];
}

void MdlFile::readSelfIllumColorController(uint16_t dataIndex, const vector<float> &data, ModelNode &node) {
    node._selfIllumColor.r = data[dataIndex + 0];
    node._selfIllumColor.g = data[dataIndex + 1];
    node._selfIllumColor.b = data[dataIndex + 2];
}

void MdlFile::readAlphaController(uint16_t dataIndex, const vector<float> &data, ModelNode &node) {
    node._alpha = data[dataIndex];
}

void MdlFile::readRadiusController(uint16_t dataIndex, const vector<float> &data, ModelNode &node) {
    node._radius = data[dataIndex];
}

void MdlFile::readBirthrateController(uint16_t dataIndex, const vector<float> &data, ModelNode &node) {
    node._emitter->_birthrate = data[dataIndex];
}

void MdlFile::readMultiplierController(uint16_t dataIndex, const vector<float> &data, ModelNode &node) {
    node._multiplier = data[dataIndex];
}

void MdlFile::readLifeExpectancyController(uint16_t dataIndex, const vector<float> &data, ModelNode &node) {
    node._emitter->_lifeExpectancy = data[dataIndex];
}

void MdlFile::readSizeStartController(uint16_t dataIndex, const vector<float> &data, ModelNode &node) {
    node._emitter->_particleSize.start = data[dataIndex];
}

void MdlFile::readSizeMidController(uint16_t dataIndex, const vector<float> &data, ModelNode &node) {
    node._emitter->_particleSize.mid = data[dataIndex];
}

void MdlFile::readSizeEndController(uint16_t dataIndex, const vector<float> &data, ModelNode &node) {
    node._emitter->_particleSize.end = data[dataIndex];
}

void MdlFile::readColorStartController(uint16_t dataIndex, const vector<float> &data, ModelNode &node) {
    node._emitter->_color.start.r = data[dataIndex + 0ll];
    node._emitter->_color.start.g = data[dataIndex + 1ll];
    node._emitter->_color.start.b = data[dataIndex + 2ll];
}

void MdlFile::readColorMidController(uint16_t dataIndex, const vector<float> &data, ModelNode &node) {
    node._emitter->_color.mid.r = data[dataIndex + 0ll];
    node._emitter->_color.mid.g = data[dataIndex + 1ll];
    node._emitter->_color.mid.b = data[dataIndex + 2ll];
}

void MdlFile::readColorEndController(uint16_t dataIndex, const vector<float> &data, ModelNode &node) {
    node._emitter->_color.end.r = data[dataIndex + 0ll];
    node._emitter->_color.end.g = data[dataIndex + 1ll];
    node._emitter->_color.end.b = data[dataIndex + 2ll];
}

void MdlFile::readAlphaStartController(uint16_t dataIndex, const vector<float> &data, ModelNode &node) {
    node._emitter->_alpha.start = data[dataIndex];
}

void MdlFile::readAlphaMidController(uint16_t dataIndex, const vector<float> &data, ModelNode &node) {
    node._emitter->_alpha.mid = data[dataIndex];
}

void MdlFile::readAlphaEndController(uint16_t dataIndex, const vector<float> &data, ModelNode &node) {
    node._emitter->_alpha.end = data[dataIndex];
}

void MdlFile::readSizeXController(uint16_t dataIndex, const vector<float> &data, ModelNode &node) {
    node._emitter->_size.x = data[dataIndex];
}

void MdlFile::readSizeYController(uint16_t dataIndex, const vector<float> &data, ModelNode &node) {
    node._emitter->_size.y = data[dataIndex];
}

void MdlFile::readFrameEndController(uint16_t dataIndex, const vector<float> &data, ModelNode &node) {
    node._emitter->_frameEnd = data[dataIndex];
}

void MdlFile::readFrameStartController(uint16_t dataIndex, const vector<float> &data, ModelNode &node) {
    node._emitter->_frameStart = data[dataIndex];
}

void MdlFile::readRandomVelocityController(uint16_t dataIndex, const vector<float> &data, ModelNode &node) {
    node._emitter->_randomVelocity = data[dataIndex];
}

void MdlFile::readVelocityController(uint16_t dataIndex, const vector<float> &data, ModelNode &node) {
    node._emitter->_velocity = data[dataIndex];
}

void MdlFile::readSpreadController(uint16_t dataIndex, const vector<float> &data, ModelNode &node) {
    node._emitter->_spread = data[dataIndex];
}

void MdlFile::readFPSController(uint16_t dataIndex, const vector<float> &data, ModelNode &node) {
    node._emitter->_fps = data[dataIndex];
}

void MdlFile::readLight(ModelNode &node) {
    node._light = make_shared<ModelNode::Light>();

    ignore(64);

    node._light->priority = readUint32();
    node._light->ambientOnly = static_cast<bool>(readUint32());

    ignore(4);

    node._light->affectDynamic = static_cast<bool>(readUint32());
    node._light->shadow = static_cast<bool>(readUint32());

    ignore(8);
}

unique_ptr<ModelMesh> MdlFile::readMesh(const string &nodeName, int nodeFlags) {
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

    if (_gameId == GameID::TSL) ignore(8);

    uint32_t mdxDataOffset = readUint32();
    uint32_t vertCoordsOffset = readUint32();
    size_t endPos = tell();

    if (faceCount == 0) {
        warn(boost::format("MDL: invalid face count: %d, model %s") % to_string(faceCount) % _name);
        return nullptr;
    }
    if (mdxVertexSize == 0 && !(nodeFlags & kNodeHasSaber)) {
        warn(boost::format("MDL: invalid MDX vertex size: %d, model %s") % to_string(mdxVertexSize) % _name);
        return nullptr;
    }

    vector<float> vertices;
    Mesh::VertexOffsets offsets;
    vector<uint16_t> indices;

    if (nodeFlags & kNodeHasSaber) {
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
    modelMesh->setRender(render && (_classification == Model::Classification::Character || (!diffuse.empty() && diffuse != "null")));
    modelMesh->setTransparency(transparency);
    modelMesh->setShadow(shadow);
    modelMesh->setBackgroundGeometry(backgroundGeometry != 0);
    modelMesh->setDiffuseColor(glm::make_vec3(&diffuseColor[0]));
    modelMesh->setAmbientColor(glm::make_vec3(&ambientColor[0]));

    if (!diffuse.empty() && diffuse != "null") {
        modelMesh->_diffuse = Textures::instance().get(diffuse, TextureUsage::Diffuse);
        if (modelMesh->_diffuse) {
            shared_ptr<Material> material(Materials::instance().get(diffuse));
            if (material) {
                modelMesh->_material = *material;
            }
            const Texture::Features &features = modelMesh->_diffuse->features();
            if (!features.envmapTexture.empty()) {
                modelMesh->_envmap = Textures::instance().get(features.envmapTexture, TextureUsage::EnvironmentMap);
            } else if (!features.bumpyShinyTexture.empty()) {
                modelMesh->_envmap = Textures::instance().get(features.bumpyShinyTexture, TextureUsage::EnvironmentMap);
            }
            if (!features.bumpmapTexture.empty()) {
                modelMesh->_bumpmap = Textures::instance().get(features.bumpmapTexture, TextureUsage::Bumpmap);
            }
        }
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

void MdlFile::readSkin(ModelNode &node) {
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

vector<shared_ptr<Animation>> MdlFile::readAnimations(const vector<uint32_t> &offsets) {
    vector<shared_ptr<Animation>> anims;
    anims.reserve(offsets.size());

    for (uint32_t offset : offsets) {
        anims.push_back(readAnimation(offset));
    }

    return move(anims);
}

unique_ptr<Animation> MdlFile::readAnimation(uint32_t offset) {
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

Model::Classification MdlFile::getClassification(int value) const {
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

void MdlFile::readEmitter(ModelNode &node) {
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
        MdlFile mdl(gameId);
        mdl.load(wrap(mdlData), wrap(mdxData));
        return mdl.model();
    }
    return nullptr;
}

} // namespace render

} // namespace reone
