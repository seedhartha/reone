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
#include "../../common/stringutil.h"
#include "../../resource/gameidutil.h"
#include "../../resource/resources.h"

#include "../textures.h"

#include "models.h"

using namespace std;

using namespace reone::resource;

namespace fs = boost::filesystem;

namespace reone {

namespace render {

static constexpr int kMdlDataOffset = 12;

struct NodeFlags {
    static constexpr int header = 1;
    static constexpr int light = 2;
    static constexpr int emitter = 4;
    static constexpr int camera = 8;
    static constexpr int reference = 0x10;
    static constexpr int mesh = 0x20;
    static constexpr int skin = 0x40;
    static constexpr int anim = 0x80;
    static constexpr int dangly = 0x100;
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
    return getFromLookupOrDefault(g_classifications, ordinal, Model::Classification::Other);
}

// END Classification

// Controllers

struct ControllerKey {
    uint32_t type { 0 };
    uint16_t unknown1 { 0 };
    uint16_t rowCount { 0 };
    uint16_t timeIndex { 0 };
    uint16_t dataIndex { 0 };
    uint8_t columnCount { 0 };
};

typedef function<void(const ControllerKey &, const vector<float> &, ModelNode &)> ControllerFn;

static void readPositionController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
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

static void readOrientationController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
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

static void readScaleController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    for (uint16_t i = 0; i < key.rowCount; ++i) {
        ModelNode::Keyframe frame;
        frame.time = data[key.timeIndex + i];
        frame.scale = data[key.dataIndex + i];

        node.addScaleKeyframe(move(frame));
    }
}

static void readAlphaController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
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

static void readSelfIllumColorController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
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

static void readColorController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.setColor(glm::make_vec3(&data[key.dataIndex]));
}

static void readRadiusController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.light()->radius = data[key.dataIndex];
}

static void readShadowRadiusController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    // TODO: implement
}

static void readVerticalDisplacementController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    // TODO: implement
}

static void readMultiplierController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    node.light()->multiplier = data[key.dataIndex];
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
    { 36, &readScaleController },
    { 132, &readAlphaController }
};

static const unordered_map<uint32_t, ControllerFn> g_meshControllers {
    { 100, &readSelfIllumColorController }
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

// END Controllers

MdlReader::MdlReader(GameID gameId) : BinaryReader(0), _gameId(gameId) {
}

void MdlReader::load(const shared_ptr<istream> &mdl, const shared_ptr<istream> &mdx) {
    _mdxReader = make_unique<StreamReader>(mdx);

    BinaryReader::load(mdl);
}

void MdlReader::doLoad() {
    _fileHeader = readStruct<FileHeader>();
    _modelHeader = readStruct<ModelHeader>();
    _namesHeader = readStruct<NamesHeader>();

    string name(getStringLower(_modelHeader.geometry.name, 32));

    vector<uint32_t> nameOffsets(readArray<uint32_t>(kMdlDataOffset + _namesHeader.names.offset, _namesHeader.names.count));
    readNodeNames(nameOffsets);

    unique_ptr<ModelNode> rootNode(readNode(kMdlDataOffset + _modelHeader.geometry.offRootNode, nullptr));
    rootNode->computeAbsoluteTransforms();

    vector<uint32_t> animOffsets(readArray<uint32_t>(kMdlDataOffset + _modelHeader.animations.offset, _modelHeader.animations.count));
    vector<shared_ptr<Animation>> anims(readAnimations(animOffsets));

    shared_ptr<Model> superModel;
    string superModelName(getStringLower(_modelHeader.superModel, 32));
    if (!superModelName.empty() && superModelName != "null") {
        superModel = Models::instance().get(superModelName);
    }

    _model = make_unique<Model>(name, getClassification(_modelHeader.classification), move(rootNode), anims, superModel);
    _model->setAnimationScale(_modelHeader.animationScale);
}

void MdlReader::readNodeNames(const vector<uint32_t> &offsets) {
    unordered_map<string, int> nameHits;

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

    NodeHeader header(readStruct<NodeHeader>());
    if (header.flags & 0xf408) {
        throw runtime_error("Unsupported model node flags: " + to_string(header.flags));
    }

    string name(_nodeNames[header.nodeNumber]);

    glm::vec3 position(glm::make_vec3(header.position));
    glm::quat orientation(header.orientation[0], header.orientation[1], header.orientation[2], header.orientation[3]);

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, position);
    transform *= glm::mat4_cast(orientation);

    auto node = make_unique<ModelNode>(_nodeIndex++, parent);
    node->_flags = header.flags;
    node->_nodeNumber = header.nodeNumber;
    node->_name = name;
    node->_position = position;
    node->_orientation = orientation;
    node->_localTransform = transform;

    if (header.flags & NodeFlags::light) {
        node->_light = make_shared<ModelNode::Light>();
    }
    if (header.flags & NodeFlags::emitter) {
        node->_emitter = make_shared<Emitter>();
    }
    if (header.flags & NodeFlags::reference) {
        node->_reference = make_shared<ModelNode::Reference>();
    }
    if (header.flags & NodeFlags::skin) {
        node->_skin = make_shared<ModelNode::Skin>();
    }
    if (header.flags & NodeFlags::saber) {
        node->_saber = true;
    }

    vector<float> controllerData(readArray<float>(kMdlDataOffset + header.controllerData.offset, header.controllerData.count));
    readControllers(header.controllers.count, header.controllers.offset, controllerData, *node);

    if (header.flags & NodeFlags::light) {
        readLight(*node);
    }
    if (header.flags & NodeFlags::emitter) {
        readEmitter(*node);
    }
    if (header.flags & NodeFlags::reference) {
        readReference(*node);
    }
    // Mesh will be loaded by either readMesh here, or readSaber below
    if ((header.flags & NodeFlags::mesh) && !(header.flags & NodeFlags::saber)) {
        readMesh(*node);
    }
    if (header.flags & NodeFlags::skin) {
        readSkin(*node);
    }
    if (header.flags & NodeFlags::dangly) {
        readDanglymesh(*node);
    }
    if (header.flags & NodeFlags::aabb) {
        readAABB(*node);
    }
    if (header.flags & NodeFlags::saber) {
        readSaber(*node);
    }

    vector<uint32_t> childOffsets(readArray<uint32_t>(kMdlDataOffset + header.children.offset, header.children.count));
    for (auto offset : childOffsets) {
        unique_ptr<ModelNode> child(readNode(kMdlDataOffset + offset, node.get()));
        node->_children.push_back(move(child));
    }

    seek(pos);

    return move(node);
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
    LightHeader header(readStruct<LightHeader>());

    node._light->priority = header.priority;
    node._light->ambientOnly = static_cast<bool>(header.ambientOnly);
    node._light->dynamicType = header.dynamicType;
    node._light->affectDynamic = static_cast<bool>(header.affectDynamic);
    node._light->shadow = static_cast<bool>(header.shadow);
}

static Emitter::UpdateMode parseEmitterUpdate(const char *cString) {
    string str(getStringLower(cString, 32));

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

static Emitter::RenderMode parseEmitterRender(const char *cString) {
    string str(getStringLower(cString, 32));

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

static Emitter::BlendMode parseEmitterBlend(const char *cString) {
    string str(getStringLower(cString, 32));

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
    EmitterHeader header(readStruct<EmitterHeader>());
    string texResRef(getStringLower(header.texture, 32));

    node._emitter->_gridWidth = glm::max(header.xGrid, 1u);
    node._emitter->_gridHeight = glm::max(header.yGrid, 1u);
    node._emitter->_updateMode = parseEmitterUpdate(header.update);
    node._emitter->_renderMode = parseEmitterRender(header.render);
    node._emitter->_blendMode = parseEmitterBlend(header.blend);
    node._emitter->_texture = Textures::instance().get(texResRef, TextureUsage::Diffuse);
    node._emitter->_loop = static_cast<bool>(header.loop);
    node._emitter->_renderOrder = header.renderOrder;
}

void MdlReader::readReference(ModelNode &node) {
    ReferenceHeader header(readStruct<ReferenceHeader>());
    string modelResRef(getStringLower(header.modelResRef, 32));

    node._reference->model = Models::instance().get(modelResRef);
    node._reference->reattachable = static_cast<bool>(header.reattachable);
}

void MdlReader::readMesh(ModelNode &node) {
    MeshHeader header(readStruct<MeshHeader>());

    if (isTSL(_gameId)) ignore(8);

    uint32_t offMdxData = readUint32();
    uint32_t offVertices = readUint32();

    size_t endPos = tell();

    if (header.faces.count == 0 || header.mdxVertexSize == 0) return;

    vector<float> vertices;
    _mdxReader->seek(offMdxData);
    vertices = _mdxReader->getArray<float>(header.numVertices * header.mdxVertexSize / sizeof(float));

    Mesh::VertexOffsets offsets;
    offsets.stride = header.mdxVertexSize;
    offsets.vertexCoords = header.offMdxVertices;
    offsets.normals = header.offMdxNormals != 0xffffffff ? header.offMdxNormals : -1;
    offsets.texCoords1 = header.offMdxTexCoords1 != 0xffffffff ? header.offMdxTexCoords1 : -1;
    offsets.texCoords2 = header.offMdxTexCoords2 != 0xffffffff ? header.offMdxTexCoords2 : -1;
    if (header.offMdxTanSpace != 0xffffffff) {
        offsets.bitangents = header.offMdxTanSpace + 0 * sizeof(float);
        offsets.tangents = header.offMdxTanSpace + 3 * sizeof(float);
    }

    vector<uint16_t> indices;
    seek(kMdlDataOffset + header.indicesOffsets.offset);
    uint32_t offIndices = readUint32();
    seek(kMdlDataOffset + offIndices);
    indices = readArray<uint16_t>(3 * header.faces.count);

    seek(endPos);

    loadMesh(header, header.numVertices, move(vertices), move(indices), move(offsets), node);
}

void MdlReader::loadMesh(const MeshHeader &header, int numVertices, vector<float> &&vertices, vector<uint16_t> &&indices, Mesh::VertexOffsets &&offsets, ModelNode &node) {
    auto mesh = make_unique<Mesh>(numVertices, vertices, indices, offsets);
    mesh->computeAABB();

    node._mesh = make_unique<ModelMesh>(move(mesh));
    node._mesh->setRender(static_cast<bool>(header.render));
    node._mesh->setTransparency(static_cast<int>(header.transparencyHint));
    node._mesh->setShadow(static_cast<bool>(header.shadow));
    node._mesh->setBackgroundGeometry(static_cast<bool>(header.backgroundGeometry) != 0);
    node._mesh->setDiffuseColor(glm::make_vec3(header.diffuse));
    node._mesh->setAmbientColor(glm::make_vec3(header.ambient));

    string tex1ResRef(getStringLower(header.texture1, 32));
    string tex2ResRef(getStringLower(header.texture2, 32));
    string tex3ResRef(getStringLower(header.texture3, 12));
    string tex4ResRef(getStringLower(header.texture4, 12));

    if (!tex1ResRef.empty() && tex1ResRef != "null") {
        node._mesh->_diffuse = Textures::instance().get(tex1ResRef, TextureUsage::Diffuse);
    }
    if (!tex2ResRef.empty()) {
        node._mesh->_lightmap = Textures::instance().get(tex2ResRef, TextureUsage::Lightmap);
    }
    if (header.animateUV) {
        node._mesh->_uvAnimation.animated = true;
        node._mesh->_uvAnimation.directionX = header.uvDirectionX;
        node._mesh->_uvAnimation.directionY = header.uvDirectionY;
        node._mesh->_uvAnimation.jitter = header.uvJitter;
        node._mesh->_uvAnimation.jitterSpeed = header.uvJitterSpeed;
    }
}

void MdlReader::readSkin(ModelNode &node) {
    SkinHeader header(readStruct<SkinHeader>());

    node._mesh->_mesh->_offsets.boneWeights = header.offMdxBoneWeights;
    node._mesh->_mesh->_offsets.boneIndices = header.offMdxBoneIndices;

    unordered_map<uint16_t, uint16_t> nodeIdxByBoneIdx;
    seek(kMdlDataOffset + header.offBones);

    for (uint32_t i = 0; i < header.numBones; ++i) {
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

    MeshHeader meshHeader(readStruct<MeshHeader>());

    if (isTSL(_gameId)) ignore(8);

    uint32_t offMdxData = readUint32();
    uint32_t offVertices = readUint32();

    SaberHeader saberHeader(readStruct<SaberHeader>());

    static int referenceIndices[] { 0, 1, 2, 3, 4, 5, 6, 7, 88, 89, 90, 91, 92, 93, 94, 95 };

    seek(static_cast<size_t>(kMdlDataOffset) + saberHeader.offVertices);
    vector<float> saberVertices(readArray<float>(3 * meshHeader.numVertices));

    seek(static_cast<size_t>(kMdlDataOffset) + saberHeader.offTexCoords);
    vector<float> texCoords(readArray<float>(2 * meshHeader.numVertices));

    seek(static_cast<size_t>(kMdlDataOffset) + saberHeader.offNormals);
    vector<float> normals(readArray<float>(3 * meshHeader.numVertices));

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

    Mesh::VertexOffsets offsets;
    offsets.vertexCoords = 0;
    offsets.texCoords1 = 3 * sizeof(float);
    offsets.normals = 5 * sizeof(float);
    offsets.stride = 8 * sizeof(float);

    vector<uint16_t> indices {
        0, 13, 12, 0, 1, 13,
        1, 14, 13, 1, 2, 14,
        2, 15, 14, 2, 3, 15,
        8, 4, 5, 8, 5, 9,
        9, 5, 6, 9, 6, 10,
        10, 6, 7, 10, 7, 11
    };

    loadMesh(meshHeader, numVertices, move(vertices), move(indices), move(offsets), node);
}

void MdlReader::readDanglymesh(ModelNode &node) {
    DanglymeshHeader header(readStruct<DanglymeshHeader>());
    // TODO: fill ModelNode
}

void MdlReader::readAABB(ModelNode &node) {
    uint32_t offTree = readUint32();
    size_t pos = tell();

    node._aabb = readAABBNode(offTree);

    seek(pos);
}

shared_ptr<AABBNode> MdlReader::readAABBNode(uint32_t offset) {
    auto node = make_shared<AABBNode>();

    seek(kMdlDataOffset + offset);
    AABBNodeHeader header(readStruct<AABBNodeHeader>());

    node->faceIndex = header.faceIndex;
    node->mostSignificantPlane = static_cast<AABBNode::Plane>(header.mostSignificantPlane);
    node->aabb.expand(glm::make_vec3(header.bbMin));
    node->aabb.expand(glm::make_vec3(header.bbMax));

    if (header.faceIndex == -1) {
        node->leftChild = readAABBNode(header.offChildLeft);
        node->rightChild = readAABBNode(header.offChildRight);
    }

    return move(node);
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

    AnimationHeader header(readStruct<AnimationHeader>());

    string name(getStringLower(header.geometry.name, 32));

    _nodeIndex = 0;
    unique_ptr<ModelNode> rootNode(readNode(kMdlDataOffset + header.geometry.offRootNode, nullptr));

    vector<Animation::Event> events;
    if (header.events.count > 0) {
        for (uint32_t i = 0; i < header.events.count; ++i) {
            Animation::Event event;
            event.time = readFloat();
            event.name = boost::to_lower_copy(readCString(32));
        }
        sort(events.begin(), events.end(), [](auto &left, auto &right) { return left.time < right.time; });
    }

    return make_unique<Animation>(name, header.length, header.transitionTime, move(events), move(rootNode));
}

shared_ptr<Model> MdlModelLoader::loadModel(GameID gameId, const string &resRef) {
    shared_ptr<ByteArray> mdlData(Resources::instance().get(resRef, ResourceType::Mdl));
    shared_ptr<ByteArray> mdxData(Resources::instance().get(resRef, ResourceType::Mdx));
    shared_ptr<Model> model;

    if (mdlData && mdxData) {
        MdlReader mdl(gameId);
        mdl.load(wrap(mdlData), wrap(mdxData));
        model = mdl.model();
    }

    return move(model);
}

} // namespace render

} // namespace reone
