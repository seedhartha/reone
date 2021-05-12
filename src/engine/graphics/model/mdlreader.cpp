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

#include "../texture/textures.h"

#include "models.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace graphics {

static constexpr int kMdlDataOffset = 12;
static constexpr uint32_t kFunctionPtrTslPC = 4285200;
static constexpr uint32_t kFunctionPtrTslXbox = 4285872;

struct NodeFlags {
    static constexpr int header = 1;
    static constexpr int light = 2;
    static constexpr int emitter = 4;
    static constexpr int camera = 8;
    static constexpr int reference = 0x10;
    static constexpr int mesh = 0x20;
    static constexpr int skin = 0x40;
    static constexpr int anim = 0x80;
    static constexpr int danglymesh = 0x100;
    static constexpr int aabb = 0x200;
    static constexpr int saber = 0x800;
};

// Classificaiton

static unordered_map<uint8_t, Model::Classification> g_classifications {
    { 0, Model::Classification::Other },
    { 1, Model::Classification::Effect },
    { 2, Model::Classification::Tile },
    { 4, Model::Classification::Character },
    { 8, Model::Classification::Door },
    { 0x10, Model::Classification::Lightsaber },
    { 0x20, Model::Classification::Placeable },
    { 0x40, Model::Classification::Flyer }
};

static Model::Classification getClassification(uint8_t ordinal) {
    return getFromLookupOrElse(g_classifications, ordinal, Model::Classification::Other);
}

// END Classification

// Controllers

struct ControllerKey {
    uint32_t type { 0 };
    uint16_t numRows { 0 };
    uint16_t timeIndex { 0 };
    uint16_t dataIndex { 0 };
    uint8_t numColumns { 0 };
};

typedef function<void(const ControllerKey &, const vector<float> &, ModelNode &)> ControllerFn;

static void readPositionController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    bool bezier = key.numColumns & 16;

    switch (key.numColumns) {
        case 3:
        case 19:
            for (uint16_t i = 0; i < key.numRows; ++i) {
                int rowTimeIdx = key.timeIndex + i;
                int rowDataIdx = key.dataIndex + i * (bezier ? 9 : 3);

                float time = data[rowTimeIdx];
                glm::vec3 position(glm::make_vec3(&data[rowDataIdx]));
                node.positions().addKeyframe(time, move(position));
            }
            break;
        default:
            break;
    }

    node.positions().update();
}

static void readOrientationController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
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
                node.orientations().addKeyframe(time, move(orientation));
            }
            break;

        case 4:
            for (uint16_t i = 0; i < key.numRows; ++i) {
                int rowTimeIdx = key.timeIndex + i;
                int rowDataIdx = key.dataIndex + i * 4;

                float time = data[rowTimeIdx];

                float x = data[rowDataIdx + 0];
                float y = data[rowDataIdx + 1];
                float z = data[rowDataIdx + 2];
                float w = data[rowDataIdx + 3];
                glm::quat orientation(w, x, y, z);

                node.orientations().addKeyframe(time, move(orientation));
            }
            break;
        default:
            break;
    }

    node.orientations().update();
}

static void readScaleController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    for (uint16_t i = 0; i < key.numRows; ++i) {
        float time = data[key.timeIndex + i];
        float scale = data[key.dataIndex + i];
        node.scales().addKeyframe(time, scale);
    }
    node.scales().update();
}

static void readAlphaController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    for (uint16_t i = 0; i < key.numRows; ++i) {
        float time = data[key.timeIndex + i];
        float alpha = data[key.dataIndex + i];
        node.alphas().addKeyframe(time, alpha);
    }
    node.alphas().update();
}

static void readSelfIllumColorController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    for (uint16_t i = 0; i < key.numRows; ++i) {
        float time = data[key.timeIndex + i];
        glm::vec3 color(glm::make_vec3(&data[key.dataIndex + 3 * i]));
        node.selfIllumColors().addKeyframe(time, move(color));
    }
    node.selfIllumColors().update();
}

static void readColorController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    for (uint16_t i = 0; i < key.numRows; ++i) {
        float time = data[key.timeIndex + i];
        glm::vec3 color(glm::make_vec3(&data[key.dataIndex + 3 * i]));
        node.lightColors().addKeyframe(time, move(color));
    }
    node.lightColors().update();
}

static void readRadiusController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    for (uint16_t i = 0; i < key.numRows; ++i) {
        float time = data[key.timeIndex + i];
        float radius = data[key.dataIndex + i];
        node.lightRadii().addKeyframe(time, radius);
    }
    node.lightRadii().update();
}

static void readShadowRadiusController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    // TODO: implement
}

static void readVerticalDisplacementController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    // TODO: implement
}

static void readMultiplierController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    for (uint16_t i = 0; i < key.numRows; ++i) {
        float time = data[key.timeIndex + i];
        float multiplier = data[key.dataIndex + i];
        node.lightMultipliers().addKeyframe(time, multiplier);
    }
    node.lightMultipliers().update();
}

static void readAlphaEndController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->alpha().end = data[key.dataIndex];
}

static void readAlphaStartController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->alpha().start = data[key.dataIndex];
}

static void readBirthrateController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->setBirthrate(static_cast<int>(data[key.dataIndex]));
}

static void readBounceCoController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    // TODO: implement
}

static void readCombineTimeController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    // TODO: implement
}

static void readDragController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    // TODO: implement
}

static void readFPSController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->setFPS(static_cast<int>(data[key.dataIndex]));
}

static void readFrameEndController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->setFrameEnd(static_cast<int>(data[key.dataIndex]));
}

static void readFrameStartController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->setFrameStart(static_cast<int>(data[key.dataIndex]));
}

static void readGravController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    // TODO: implement
}

static void readLifeExpController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->setLifeExpectancy(static_cast<int>(data[key.dataIndex]));
}

static void readMassController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    // TODO: implement
}

static void readP2PBezier2Controller(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    // TODO: implement
}

static void readP2PBezier3Controller(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    // TODO: implement
}

static void readParticleRotController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    // TODO: implement
}

static void readRandVelController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->setRandomVelocity(data[key.dataIndex]);
}

static void readSizeStartController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->particleSize().start = data[key.dataIndex];
}

static void readSizeEndController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->particleSize().end = data[key.dataIndex];
}

static void readSizeStartYController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    // TODO: implement
}

static void readSizeEndYController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    // TODO: implement
}

static void readSpreadController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->setSpread(data[key.dataIndex]);
}

static void readThresholdController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    // TODO: implement
}

static void readVelocityController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->setVelocity(data[key.dataIndex]);
}

static void readXSizeController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->size().x = data[key.dataIndex];
}

static void readYSizeController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->size().y = data[key.dataIndex];
}

static void readBlurLengthController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    // TODO: implement
}

static void readLightingDelayController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    // TODO: implement
}

static void readLightingRadiusController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    // TODO: implement
}

static void readLightingScaleController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    // TODO: implement
}

static void readLightingSubDivController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    // TODO: implement
}

static void readLightingZigZagController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    // TODO: implement
}

static void readAlphaMidController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->alpha().mid = data[key.dataIndex];
}

static void readPercentStartController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    // TODO: implement
}

static void readPercentMidController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    // TODO: implement
}

static void readPercentEndController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    // TODO: implement
}

static void readSizeMidController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->particleSize().mid = data[key.dataIndex];
}

static void readSizeMidYController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    // TODO: implement
}

static void readRandomBirthRateController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    // TODO: implement
}

static void readTargetSizeController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    // TODO: implement
}

static void readNumControlPtsController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    // TODO: implement
}

static void readControlPtRadiusController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    // TODO: implement
}

static void readControlPtDelayController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    // TODO: implement
}

static void readTangentSpreadController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    // TODO: implement
}

static void readTangentLengthController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    // TODO: implement
}

static void readColorMidController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->color().mid = glm::make_vec3(&data[key.dataIndex]);
}

static void readColorEndController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->color().end = glm::make_vec3(&data[key.dataIndex]);
}

static void readColorStartController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.emitter()->color().start = glm::make_vec3(&data[key.dataIndex]);
}

static void readDetonateController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    // TODO: implement
}

static const unordered_map<uint32_t, ControllerFn> g_genericControllers {
    { 8, &readPositionController },
    { 20, &readOrientationController },
    { 36, &readScaleController }
};

static const unordered_map<uint32_t, ControllerFn> g_meshControllers {
    { 100, &readSelfIllumColorController },
    { 132, &readAlphaController }
};

static const unordered_map<uint32_t, ControllerFn> g_lightControllers {
    { 76, &readColorController },
    { 88, &readRadiusController },
    { 96, &readShadowRadiusController },
    { 100, &readVerticalDisplacementController },
    { 140, &readMultiplierController }
};

static const unordered_map<uint32_t, ControllerFn> g_emitterControllers {
    { 80, &readAlphaEndController },
    { 84, &readAlphaStartController },
    { 88, &readBirthrateController },
    { 92, &readBounceCoController },
    { 96, &readCombineTimeController },
    { 100, &readDragController },
    { 104, &readFPSController },
    { 108, &readFrameEndController },
    { 112, &readFrameStartController },
    { 116, &readGravController },
    { 120, &readLifeExpController },
    { 124, &readMassController },
    { 128, &readP2PBezier2Controller },
    { 132, &readP2PBezier3Controller },
    { 136, &readParticleRotController },
    { 140, &readRandVelController },
    { 144, &readSizeStartController },
    { 148, &readSizeEndController },
    { 152, &readSizeStartYController },
    { 156, &readSizeEndYController },
    { 160, &readSpreadController },
    { 164, &readThresholdController },
    { 168, &readVelocityController },
    { 172, &readXSizeController },
    { 176, &readYSizeController },
    { 180, &readBlurLengthController },
    { 184, &readLightingDelayController },
    { 188, &readLightingRadiusController },
    { 192, &readLightingScaleController },
    { 196, &readLightingSubDivController },
    { 200, &readLightingZigZagController },
    { 216, &readAlphaMidController },
    { 220, &readPercentStartController },
    { 224, &readPercentMidController },
    { 228, &readPercentEndController },
    { 232, &readSizeMidController },
    { 236, &readSizeMidYController },
    { 240, &readRandomBirthRateController },
    { 252, &readTargetSizeController },
    { 256, &readNumControlPtsController },
    { 260, &readControlPtRadiusController },
    { 264, &readControlPtDelayController },
    { 268, &readTangentSpreadController },
    { 272, &readTangentLengthController },
    { 284, &readColorMidController },
    { 380, &readColorEndController },
    { 392, &readColorStartController },
    { 502, &readDetonateController }
};

// END Controllers

MdlReader::MdlReader() : BinaryReader(4, "\000\000\000\000") {
}

void MdlReader::load(const shared_ptr<istream> &mdl, const shared_ptr<istream> &mdx) {
    _mdxReader = make_unique<StreamReader>(mdx);

    BinaryReader::load(mdl);
}

static bool isTSLFunctionPointer(uint32_t ptr) {
    return ptr == kFunctionPtrTslPC || ptr == kFunctionPtrTslXbox;
}

void MdlReader::doLoad() {
    // File Header
    uint32_t mdlSize = readUint32();
    uint32_t mdxSize = readUint32();

    // Geometry Header
    uint32_t funcPtr1 = readUint32();
    uint32_t funcPtr2 = readUint32();
    string name(readCString(32));
    uint32_t offRootNode = readUint32();
    uint32_t numNodes = readUint32();
    ignore(6 * 4); // unknown
    uint32_t refCount = readUint32();
    uint8_t modelType = readByte();
    ignore(3); // padding

    // Model Header
    uint8_t classification = readByte();
    uint8_t subclassification = readByte();
    ignore(1); // unknown
    uint8_t affectedByFog = readByte();
    uint32_t numChildModels = readUint32();
    ArrayDefinition animationArrayDef(readArrayDefinition());
    uint32_t superModelRef = readUint32();
    vector<float> boundingBox(readFloatArray(6));
    float radius = readFloat();
    float animationScale = readFloat();
    string superModelName(boost::to_lower_copy(readCString(32)));
    uint32_t offHeadRootNode = readUint32();
    ignore(4); // unknown
    uint32_t mdxSize2 = readUint32();
    uint32_t mdxOffset = readUint32();
    ArrayDefinition nameArrayDef(readArrayDefinition());

    _tsl = isTSLFunctionPointer(funcPtr1);

    vector<uint32_t> nameOffsets(readUint32Array(kMdlDataOffset + nameArrayDef.offset, nameArrayDef.count));
    readNodeNames(nameOffsets);

    unique_ptr<ModelNode> rootNode(readNode(kMdlDataOffset + offRootNode, nullptr));
    rootNode->computeAbsoluteTransforms();

    vector<uint32_t> animOffsets(readUint32Array(kMdlDataOffset + animationArrayDef.offset, animationArrayDef.count));
    vector<shared_ptr<Animation>> anims(readAnimations(animOffsets));

    shared_ptr<Model> superModel;
    if (!superModelName.empty() && superModelName != "null") {
        superModel = Models::instance().get(superModelName);
    }

    _model = make_unique<Model>(name, getClassification(classification), animationScale, move(rootNode), anims, superModel);
    _model->setAffectedByFog(affectedByFog != 0);
}

MdlReader::ArrayDefinition MdlReader::readArrayDefinition() {
    ArrayDefinition result;
    result.offset = readUint32();
    result.count = readUint32();
    result.count2 = readUint32();
    return move(result);
}

void MdlReader::readNodeNames(const vector<uint32_t> &offsets) {
    map<string, int> nameHits;
    for (uint32_t offset : offsets) {
        string name(boost::to_lower_copy(readCStringAt(kMdlDataOffset + offset)));
        int hitCount = nameHits[name]++;
        if (hitCount > 0) {
            debug("Duplicate model node name: " + name);
            name = str(boost::format("%s_dup%d") % name % hitCount);
        }
        _nodeNames.push_back(move(name));
    }
}

unique_ptr<ModelNode> MdlReader::readNode(uint32_t offset, ModelNode *parent) {
    size_t pos = tell();
    seek(offset);

    uint16_t flags = readUint16();
    uint16_t index = readUint16(); // TODO: rename to "super node index"
    uint16_t nodeNumber = readUint16(); // TODO: rename to "node index"
    ignore(2); // padding
    uint32_t offRootNode = readUint32();
    uint32_t offParentNode = readUint32();
    vector<float> positionValues(readFloatArray(3));
    vector<float> orientationValues(readFloatArray(4));
    ArrayDefinition childArrayDef(readArrayDefinition());
    ArrayDefinition controllerArrayDef(readArrayDefinition());
    ArrayDefinition controllerDataArrayDef(readArrayDefinition());

    if (flags & 0xf408) {
        throw runtime_error("Unsupported MDL node flags: " + to_string(flags));
    }

    string name(_nodeNames[nodeNumber]);
    glm::vec3 position(glm::make_vec3(&positionValues[0]));
    glm::quat orientation(orientationValues[0], orientationValues[1], orientationValues[2], orientationValues[3]);

    glm::mat4 localTransform(1.0f);
    localTransform = glm::translate(localTransform, position);
    localTransform *= glm::mat4_cast(orientation);

    auto node = make_unique<ModelNode>(_nodeIndex++, parent);
    node->_flags = flags;
    node->_nodeNumber = nodeNumber;
    node->_name = move(name);
    node->_position = move(position);
    node->_orientation = move(orientation);
    node->_localTransform = move(localTransform);

    if (flags & NodeFlags::light) {
        node->_light = make_shared<ModelNode::Light>();
    }
    if (flags & NodeFlags::emitter) {
        node->_emitter = make_shared<Emitter>();
    }
    if (flags & NodeFlags::reference) {
        node->_reference = make_shared<ModelNode::Reference>();
    }
    if (flags & NodeFlags::skin) {
        node->_skin = make_shared<ModelNode::Skin>();
    }
    if (flags & NodeFlags::danglymesh) {
        node->_danglymesh = make_shared<ModelNode::Danglymesh>();
    }
    if (flags & NodeFlags::saber) {
        node->_saber = true;
    }

    // When reading animation node controllers, take node flags from the
    // corresponding model node. This is needed to determine which controller
    // lookup table to use.
    int controllerNodeFlags;
    if (_readingAnimations) {
        controllerNodeFlags = _nodeFlags.find(nodeNumber)->second;
    } else {
        controllerNodeFlags = flags;
    }
    vector<float> controllerData(readFloatArray(kMdlDataOffset + controllerDataArrayDef.offset, controllerDataArrayDef.count));
    readControllers(controllerNodeFlags, controllerArrayDef.offset, controllerArrayDef.count, controllerData, *node);

    if (flags & NodeFlags::light) {
        readLight(*node);
    }
    if (flags & NodeFlags::emitter) {
        readEmitter(*node);
    }
    if (flags & NodeFlags::reference) {
        readReference(*node);
    }
    // Mesh will be loaded by either readMesh here, or readSaber below
    if ((flags & NodeFlags::mesh) && !(flags & NodeFlags::saber)) {
        readMesh(*node);
    }
    if (flags & NodeFlags::skin) {
        readSkin(*node);
    }
    if (flags & NodeFlags::danglymesh) {
        readDanglymesh(*node);
    }
    if (flags & NodeFlags::aabb) {
        readAABB(*node);
    }
    if (flags & NodeFlags::saber) {
        readSaber(*node);
    }

    vector<uint32_t> childOffsets(readUint32Array(kMdlDataOffset + childArrayDef.offset, childArrayDef.count));
    for (uint32_t offset : childOffsets) {
        unique_ptr<ModelNode> child(readNode(kMdlDataOffset + offset, node.get()));
        node->_children.push_back(move(child));
    }

    seek(pos);

    // When reading model (not animation) nodes, fill node flags lookup table
    if (!_readingAnimations) {
        _nodeFlags.insert(make_pair(nodeNumber, flags));
    }

    return move(node);
}

static function<void(const ControllerKey &, const vector<float> &, ModelNode &)> getControllerFn(uint32_t type, int nodeFlags) {
    ControllerFn fn;
    if (nodeFlags & NodeFlags::mesh) {
        fn = getFromLookupOrNull(g_meshControllers, type);
    } else if (nodeFlags & NodeFlags::light) {
        fn = getFromLookupOrNull(g_lightControllers, type);
    } else if (nodeFlags & NodeFlags::emitter) {
        fn = getFromLookupOrNull(g_emitterControllers, type);
    }
    if (!fn) {
        fn = getFromLookupOrNull(g_genericControllers, type);
    }
    return move(fn);
}

void MdlReader::readControllers(int nodeFlags, uint32_t keyOffset, uint32_t keyCount, const vector<float> &data, ModelNode &node) {
    // TODO: enable animating emitter properties
    if (_readingAnimations && (nodeFlags & NodeFlags::emitter)) return;

    size_t pos = tell();
    seek(kMdlDataOffset + keyOffset);

    for (uint32_t i = 0; i < keyCount; ++i) {
        uint32_t type = readUint32();
        ignore(2); // unknown
        uint16_t numRows = readUint16();
        uint16_t timeIndex = readUint16();
        uint16_t dataIndex = readUint16();
        uint8_t numColumns = readByte();
        ignore(3); // padding

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
            debug(boost::format("Unsupported MDL controller type: %d") % static_cast<int>(key.type), 3);
        }
    }

    seek(pos);
}

void MdlReader::readLight(ModelNode &node) {
    float flareRadius  = readFloat();
    ignore(3 * 4); // unknown
    ArrayDefinition flareSizesArrayDef(readArrayDefinition());
    ArrayDefinition flarePositionsArrayDef(readArrayDefinition());
    ArrayDefinition flareColorShiftsArrayDef(readArrayDefinition());
    ArrayDefinition flareTexturesArrayDef(readArrayDefinition());
    uint32_t priority = readUint32();
    uint32_t ambientOnly = readUint32();
    uint32_t dynamicType = readUint32();
    uint32_t affectDynamic = readUint32();
    uint32_t shadow = readUint32();
    uint32_t flare = readUint32();
    uint32_t fading = readUint32();

    node._light->priority = priority;
    node._light->ambientOnly = static_cast<bool>(ambientOnly);
    node._light->dynamicType = dynamicType;
    node._light->affectDynamic = static_cast<bool>(affectDynamic);
    node._light->shadow = static_cast<bool>(shadow);
    node._light->flareRadius = flareRadius;

    int numFlares = static_cast<int>(flareTexturesArrayDef.count);
    if (numFlares > 0) {
        vector<float> flareSizes(readFloatArray(kMdlDataOffset + flareSizesArrayDef.offset, flareSizesArrayDef.count));
        vector<float> flarePositions(readFloatArray(kMdlDataOffset + flarePositionsArrayDef.offset, flarePositionsArrayDef.count));
        vector<uint32_t> texNameOffsets(readUint32Array(kMdlDataOffset + flareTexturesArrayDef.offset, flareTexturesArrayDef.count));

        vector<glm::vec3> colorShifts;
        for (int i = 0; i < numFlares; ++i) {
            seek(kMdlDataOffset + flareColorShiftsArrayDef.offset + 12 * i);
            glm::vec3 colorShift(readFloat(), readFloat(), readFloat());
            colorShifts.push_back(move(colorShift));
        }

        vector<shared_ptr<Texture>> flareTextures;
        for (int i = 0; i < numFlares; ++i) {
            seek(kMdlDataOffset + texNameOffsets[i]);
            string textureName(boost::to_lower_copy(readCString(12)));
            shared_ptr<Texture> texture(Textures::instance().get(textureName));
            flareTextures.push_back(move(texture));
        }

        for (int i = 0; i < numFlares; ++i) {
            LensFlare lensFlare;
            lensFlare.texture = flareTextures[i];
            lensFlare.colorShift = colorShifts[i];
            lensFlare.position = flarePositions[i];
            lensFlare.size = flareSizes[i];
            node._light->flares.push_back(move(lensFlare));
        }
    }
}

static Emitter::UpdateMode parseEmitterUpdate(const string &str) {
    auto result = Emitter::UpdateMode::Invalid;
    if (str == "fountain") {
        result = Emitter::UpdateMode::Fountain;
    } else if (str == "single") {
        result = Emitter::UpdateMode::Single;
    } else if (str == "explosion") {
        result = Emitter::UpdateMode::Explosion;
    } else {
        warn("parseEmitterUpdate: unsupported value: " + str);
    }
    return result;
}

static Emitter::RenderMode parseEmitterRender(const string &str) {
    auto result = Emitter::RenderMode::Invalid;
    if (str == "normal") {
        result = Emitter::RenderMode::Normal;
    } else if (str == "billboard_to_world_z") {
        result = Emitter::RenderMode::BillboardToWorldZ;
    } else if (str == "motion_blur") {
        result = Emitter::RenderMode::MotionBlur;
    } else if (str == "billboard_to_local_z") {
        result = Emitter::RenderMode::BillboardToLocalZ;
    } else if (str == "aligned_to_particle_dir") {
        result = Emitter::RenderMode::AlignedToParticleDir;
    } else {
        warn("parseEmitterRender: unsupported value: " + str);
    }
    return result;
}

static Emitter::BlendMode parseEmitterBlend(const string &str) {
    auto result = Emitter::BlendMode::Invalid;
    if (str == "normal") {
        result = Emitter::BlendMode::Normal;
    } else if (str == "punch") {
        result = Emitter::BlendMode::Punch;
    } else if (str == "lighten") {
        result = Emitter::BlendMode::Lighten;
    } else {
        warn("parseEmitterBlend: unsupported value: " + str);
    }
    return result;
}

void MdlReader::readEmitter(ModelNode &node) {
    float deadSpace = readFloat();
    float blastRadius = readFloat();
    float blastLength = readFloat();
    uint32_t branchCount = readUint32();
    float controlPointSmoothing = readFloat();
    uint32_t xGrid = readUint32();
    uint32_t yGrid = readUint32();
    ignore(4); // unknown
    string update(boost::to_lower_copy(readCString(32)));
    string render(boost::to_lower_copy(readCString(32)));
    string blend(boost::to_lower_copy(readCString(32)));
    string texture(boost::to_lower_copy(readCString(32)));
    string chunkName(boost::to_lower_copy(readCString(16)));
    uint32_t twosided = readUint32();
    uint32_t loop = readUint32();
    uint32_t renderOrder = readUint32();
    uint32_t frameBlending = readUint32();
    string depthTexture(boost::to_lower_copy(readCString(32)));
    ignore(1); // padding
    uint32_t flags = readUint32();

    node._emitter->_gridWidth = glm::max(xGrid, 1u);
    node._emitter->_gridHeight = glm::max(yGrid, 1u);
    node._emitter->_updateMode = parseEmitterUpdate(update);
    node._emitter->_renderMode = parseEmitterRender(render);
    node._emitter->_blendMode = parseEmitterBlend(blend);
    node._emitter->_texture = Textures::instance().get(texture, TextureUsage::Diffuse);
    node._emitter->_loop = static_cast<bool>(loop);
    node._emitter->_renderOrder = renderOrder;
}

void MdlReader::readReference(ModelNode &node) {
    string modelResRef(boost::to_lower_copy(readCString(32)));
    uint32_t reattachable = readUint32();

    node._reference->model = Models::instance().get(modelResRef);
    node._reference->reattachable = static_cast<bool>(reattachable);
}

void MdlReader::readMesh(ModelNode &node) {
    MeshHeader header(readMeshHeader());

    if (_tsl) ignore(8);

    uint32_t offMdxData = readUint32();
    uint32_t offVertices = readUint32();

    size_t endPos = tell();

    if (header.numFaces == 0 || header.mdxVertexSize == 0) return;

    vector<float> vertices;
    _mdxReader->seek(offMdxData);
    vertices = _mdxReader->getFloatArray(header.numVertices * header.mdxVertexSize / sizeof(float));

    vector<uint16_t> indices;
    seek(kMdlDataOffset + header.offOffIndices);
    uint32_t offIndices = readUint32();
    seek(kMdlDataOffset + offIndices);
    indices = readUint16Array(3 * header.numFaces);

    unordered_map<uint32_t, vector<uint32_t>> materialFaces;
    seek(kMdlDataOffset + header.offFaces);
    for (int i = 0; i < header.numFaces; ++i) {
        vector<float> normalValues(readFloatArray(3));
        float distance = readFloat();
        uint32_t material = readUint32();
        vector<uint16_t> adjacentFaces(readUint16Array(3));
        vector<uint16_t> faceIndices(readUint16Array(3));
        materialFaces[material].push_back(i);
    }

    seek(endPos);

    VertexAttributes attributes;
    attributes.stride = header.mdxVertexSize;
    attributes.offCoords = header.offMdxVertices;
    attributes.offNormals = header.offMdxNormals;
    attributes.offTexCoords1 = header.offMdxTexCoords1;
    attributes.offTexCoords2 = header.offMdxTexCoords2;
    if (header.offMdxTanSpace != -1) {
        attributes.offBitangents = header.offMdxTanSpace + 0 * sizeof(float);
        attributes.offTangents = header.offMdxTanSpace + 3 * sizeof(float);
    }

    loadMesh(header, move(vertices), move(indices), move(attributes), move(materialFaces), node);
}

MdlReader::MeshHeader MdlReader::readMeshHeader() {
    uint32_t funcPtr1 = readUint32();
    uint32_t funcPtr2 = readUint32();
    ArrayDefinition faceArrayDef(readArrayDefinition());
    vector<float> boundingBox(readFloatArray(6));
    float radius = readFloat();
    vector<float> average(readFloatArray(3));
    vector<float> diffuse(readFloatArray(3));
    vector<float> ambient(readFloatArray(3));
    uint32_t transprencyHint = readUint32();
    string texture1(boost::to_lower_copy(readCString(32)));
    string texture2(boost::to_lower_copy(readCString(32)));
    string texture3(boost::to_lower_copy(readCString(12)));
    string texture4(boost::to_lower_copy(readCString(12)));
    ArrayDefinition indicesCountArrayDef(readArrayDefinition());
    ArrayDefinition indicesOffsetArrayDef(readArrayDefinition());
    ArrayDefinition invCounterArrayDef(readArrayDefinition());
    ignore(3 * 4 + 8); // unknown
    uint32_t animateUV = readUint32();
    float uvDirectionX = readFloat();
    float uvDirectionY = readFloat();
    float uvJitter = readFloat();
    float uvJitterSpeed = readFloat();
    uint32_t mdxVertexSize = readUint32();
    uint32_t mdxDataFlags = readUint32();
    int offMdxVertices = readInt32();
    int offMdxNormals = readInt32();
    int offMdxVertexColors = readInt32();
    int offMdxTexCoords1 = readInt32();
    int offMdxTexCoords2 = readInt32();
    int offMdxTexCoords3 = readInt32();
    int offMdxTexCoords4 = readInt32();
    int offMdxTanSpace = readInt32();
    ignore(3 * 4); // unknown
    uint16_t numVertices = readUint16();
    uint16_t numTextures = readUint16();
    uint8_t lightmapped = readByte();
    uint8_t rotateTexture = readByte();
    uint8_t backgroundGeometry = readByte();
    uint8_t shadow = readByte();
    uint8_t beaming = readByte();
    uint8_t render = readByte();
    ignore(2); // unknown
    float totalArea = readFloat();
    ignore(4); // unknown

    MeshHeader result;
    result.ambient = glm::make_vec3(&ambient[0]);
    result.diffuse = glm::make_vec3(&diffuse[0]);
    result.texture1 = move(texture1);
    result.texture2 = move(texture2);
    result.numVertices = numVertices;
    result.numFaces = faceArrayDef.count;
    result.offFaces = faceArrayDef.offset;
    result.offOffIndices = indicesOffsetArrayDef.offset;
    result.render = static_cast<bool>(render);
    result.shadow = static_cast<bool>(shadow);
    result.backgroundGeometry = static_cast<bool>(backgroundGeometry);
    result.transparencyHint = static_cast<int>(transprencyHint);
    result.mdxVertexSize = mdxVertexSize;
    result.offMdxVertices = offMdxVertices;
    result.offMdxNormals = offMdxNormals;
    result.offMdxTexCoords1 = offMdxTexCoords1;
    result.offMdxTexCoords2 = offMdxTexCoords2;
    result.offMdxTanSpace = offMdxTanSpace;
    result.animateUV = static_cast<bool>(animateUV);
    result.uvDirectionX = uvDirectionX;
    result.uvDirectionY = uvDirectionY;

    return move(result);
}

void MdlReader::loadMesh(const MeshHeader &header, vector<float> &&vertices, vector<uint16_t> &&indices, VertexAttributes &&attributes, MaterialMap &&materialFaces, ModelNode &node) {
    auto mesh = make_unique<Mesh>(vertices, indices, attributes);

    node._mesh = make_unique<ModelNode::Trimesh>();
    node._mesh->mesh = move(mesh);
    node._mesh->materialFaces = materialFaces;
    node._mesh->render = header.render;
    node._mesh->transparency = header.transparencyHint;
    node._mesh->shadow = header.shadow;
    node._mesh->backgroundGeometry = header.backgroundGeometry;
    node._mesh->diffuseColor = header.diffuse;
    node._mesh->ambientColor = header.ambient;

    if (!header.texture1.empty() && header.texture1 != "null") {
        node._mesh->diffuse = Textures::instance().get(header.texture1, TextureUsage::Diffuse);
    }
    if (!header.texture2.empty()) {
        node._mesh->lightmap = Textures::instance().get(header.texture2, TextureUsage::Lightmap);
    }
    if (header.animateUV) {
        node._mesh->uvAnimation.animated = true;
        node._mesh->uvAnimation.directionX = header.uvDirectionX;
        node._mesh->uvAnimation.directionY = header.uvDirectionY;
    }
}

void MdlReader::readSkin(ModelNode &node) {
    ignore(3 * 4); // unknown
    uint32_t offMdxBoneWeights = readUint32();
    uint32_t offMdxBoneIndices = readUint32();
    uint32_t offBones = readUint32();
    uint32_t numBones = readUint32();
    ArrayDefinition qBoneArrayDef(readArrayDefinition());
    ArrayDefinition tBoneArrayDef(readArrayDefinition());
    ignore(3 * 4); // unknown
    vector<uint16_t> boneIndices(readUint16Array(16));
    ignore(4); // padding

    node._mesh->mesh->attributes().offBoneWeights = offMdxBoneWeights;
    node._mesh->mesh->attributes().offBoneIndices = offMdxBoneIndices;

    unordered_map<uint16_t, uint16_t> nodeIdxByBoneIdx;
    seek(kMdlDataOffset + offBones);

    for (uint32_t i = 0; i < numBones; ++i) {
        uint16_t boneIdx = static_cast<uint16_t>(readFloat());
        if (boneIdx == 0xffff) continue;

        uint16_t nodeIdx = i;
        nodeIdxByBoneIdx.insert(make_pair(boneIdx, nodeIdx));
    }

    node._skin = make_unique<ModelNode::Skin>();
    node._skin->nodeIdxByBoneIdx = move(nodeIdxByBoneIdx);
}

void MdlReader::readSaber(ModelNode &node) {
    // Lightsaber blade is a special case. It consists of four to eight
    // planes. Some of these planes are normal meshes, but some differ in
    // that their geometry is stored in the MDL, not MDX.
    //
    // Values stored in the MDL are vertex coordinates, texture coordinates
    // and normals. However, most of the vertex coordinates seem to be
    // procedurally generated based on vertices 0-7 and 88-95.

    MeshHeader header(readMeshHeader());

    if (_tsl) ignore(8);

    uint32_t offMdxData = readUint32();
    uint32_t offVertices = readUint32();
    uint32_t offSaberVertices = readUint32();
    uint32_t offTexCoords = readUint32();
    uint32_t offNormals = readUint32();
    ignore(2 * 4); // unknown

    static int referenceIndices[] { 0, 1, 2, 3, 4, 5, 6, 7, 88, 89, 90, 91, 92, 93, 94, 95 };

    seek(static_cast<size_t>(kMdlDataOffset) + offSaberVertices);
    vector<float> saberVertices(readFloatArray(3 * header.numVertices));

    seek(static_cast<size_t>(kMdlDataOffset) + offTexCoords);
    vector<float> texCoords(readFloatArray(2 * header.numVertices));

    seek(static_cast<size_t>(kMdlDataOffset) + offNormals);
    vector<float> normals(readFloatArray(3 * header.numVertices));

    int numVertices = 16;
    vector<float> vertices;
    vertices.resize(8ll * numVertices);
    float *verticesPtr = &vertices[0];

    for (int i = 0; i < numVertices; ++i) {
        int referenceIdx = referenceIndices[i];

        // Vertex coordinates
        float *vertexCoordsPtr = &saberVertices[3ll * referenceIdx];
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

    VertexAttributes attributes;
    attributes.stride = 8 * sizeof(float);
    attributes.offCoords = 0;
    attributes.offTexCoords1 = 3 * sizeof(float);
    attributes.offNormals = 5 * sizeof(float);

    vector<uint16_t> indices {
        0, 13, 12, 0, 1, 13,
        1, 14, 13, 1, 2, 14,
        2, 15, 14, 2, 3, 15,
        8, 4, 5, 8, 5, 9,
        9, 5, 6, 9, 6, 10,
        10, 6, 7, 10, 7, 11
    };

    loadMesh(header, move(vertices), move(indices), move(attributes), MaterialMap(), node);
}

void MdlReader::readDanglymesh(ModelNode &node) {
    ArrayDefinition constraintArrayDef(readArrayDefinition());
    float displacement = readFloat();
    float tightness = readFloat();
    float period = readFloat();
    ignore(4); // unknown

    node._danglymesh->displacement = 0.5f * displacement;  // displacement is allegedly 1/2 meters per unit
    node._danglymesh->tightness = tightness;
    node._danglymesh->period = period;

    size_t pos = tell();
    seek(kMdlDataOffset + constraintArrayDef.offset);
    for (uint32_t i = 0; i < constraintArrayDef.count; ++i) {
        float multiplier = readFloat();
        vector<float> positionValues(readFloatArray(3));
        ModelNode::DanglymeshConstraint constraint;
        constraint.multiplier = glm::clamp(multiplier / 255.0f, 0.0f, 1.0f);
        constraint.position = glm::make_vec3(&positionValues[0]);
        node._danglymesh->constraints.push_back(move(constraint));
    }
    seek(pos);
}

void MdlReader::readAABB(ModelNode &node) {
    uint32_t offTree = readUint32();
    size_t pos = tell();

    node._aabb = readAABBNode(offTree);

    seek(pos);
}

shared_ptr<AABBNode> MdlReader::readAABBNode(uint32_t offset) {
    seek(kMdlDataOffset + offset);

    vector<float> boundingBox(readFloatArray(6));
    uint32_t offChildLeft = readUint32();
    uint32_t offChildRight = readUint32();
    int faceIndex = readInt32();
    uint32_t mostSignificantPlane = readUint32();

    auto node = make_shared<AABBNode>();
    node->faceIndex = faceIndex;
    node->mostSignificantPlane = static_cast<AABBNode::Plane>(mostSignificantPlane);
    node->aabb.expand(glm::make_vec3(&boundingBox[0]));
    node->aabb.expand(glm::make_vec3(&boundingBox[3]));

    if (faceIndex == -1) {
        node->leftChild = readAABBNode(offChildLeft);
        node->rightChild = readAABBNode(offChildRight);
    }

    return move(node);
}

vector<shared_ptr<Animation>> MdlReader::readAnimations(const vector<uint32_t> &offsets) {
    vector<shared_ptr<Animation>> anims;
    anims.reserve(offsets.size());

    _readingAnimations = true;
    for (uint32_t offset : offsets) {
        _nodeIndex = 0;
        anims.push_back(readAnimation(offset));
    }

    return move(anims);
}

unique_ptr<Animation> MdlReader::readAnimation(uint32_t offset) {
    seek(kMdlDataOffset + offset);

    // Geometry Header
    uint32_t funcPtr1 = readUint32();
    uint32_t funcPtr2 = readUint32();
    string name(boost::to_lower_copy(readCString(32)));
    uint32_t offRootNode = readUint32();
    uint32_t numNodes = readUint32();
    ignore(6 * 4); // unknown
    uint32_t refCount = readUint32();
    uint8_t modelType = readByte();
    ignore(3); // padding

    // Animation Header
    float length = readFloat();
    float transitionTime = readFloat();
    string root(readCString(32));
    ArrayDefinition eventArrayDef(readArrayDefinition());
    ignore(4); // unknown

    unique_ptr<ModelNode> rootNode(readNode(kMdlDataOffset + offRootNode, nullptr));

    vector<Animation::Event> events;
    if (eventArrayDef.count > 0) {
        seek(kMdlDataOffset + eventArrayDef.offset);
        for (uint32_t i = 0; i < eventArrayDef.count; ++i) {
            Animation::Event event;
            event.time = readFloat();
            event.name = boost::to_lower_copy(readCString(32));
            events.push_back(move(event));
        }
        sort(events.begin(), events.end(), [](auto &left, auto &right) { return left.time < right.time; });
    }

    return make_unique<Animation>(name, length, transitionTime, move(events), move(rootNode));
}

} // namespace graphics

} // namespace reone
