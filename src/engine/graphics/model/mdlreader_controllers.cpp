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

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "glm/ext.hpp"

#include "../../common/collectionutil.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace graphics {

static constexpr int kFlagBezier = 16;

void MdlReader::initControllerFn() {
    _genericControllers = unordered_map<uint32_t, ControllerFn> {
        { 8, &readPositionController },
        { 20, &readOrientationController },
        { 36, &readScaleController }
    };
    _meshControllers = unordered_map<uint32_t, ControllerFn> {
        { 100, &readSelfIllumColorController },
        { 132, &readAlphaController }
    };
    _lightControllers = unordered_map<uint32_t, ControllerFn> {
        { 76, &readColorController },
        { 88, &readRadiusController },
        { 96, &readShadowRadiusController },
        { 100, &readVerticalDisplacementController },
        { 140, &readMultiplierController }
    };
    _emitterControllers = unordered_map<uint32_t, ControllerFn> {
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
}

MdlReader::ControllerFn MdlReader::getControllerFn(uint32_t type, int nodeFlags) {
    ControllerFn fn;
    if (nodeFlags & NodeFlags::mesh) {
        fn = getFromLookupOrNull(_meshControllers, type);
    } else if (nodeFlags & NodeFlags::light) {
        fn = getFromLookupOrNull(_lightControllers, type);
    } else if (nodeFlags & NodeFlags::emitter) {
        fn = getFromLookupOrNull(_emitterControllers, type);
    }
    if (!fn) {
        fn = getFromLookupOrNull(_genericControllers, type);
    }
    return move(fn);
}

static inline void ensureNumColumnsEquals(int type, int expected, int actual) {
    if (actual != expected) {
        throw runtime_error(str(boost::format("Controller %d: number of columns is %d, expected %d") % type % actual % expected));
    }
}

void MdlReader::readFloatController(const ControllerKey &key, const vector<float> &data, AnimatedProperty<float> &prop) {
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

void MdlReader::readVectorController(const ControllerKey &key, const vector<float> &data, AnimatedProperty<glm::vec3> &prop) {
    bool bezier = key.numColumns & kFlagBezier;
    int numColumns = key.numColumns & ~kFlagBezier;
    ensureNumColumnsEquals(key.type, 3, numColumns);

    for (uint16_t i = 0; i < key.numRows; ++i) {
        float time = data[key.timeIndex + i];
        glm::vec3 value(glm::make_vec3(&data[key.dataIndex + (bezier ? 9 : 3) * i]));
        prop.addFrame(time, value);
    }
    prop.update();
}

void MdlReader::readPositionController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readVectorController(key, data, node.position());
}

void MdlReader::readOrientationController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
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
                node.orientation().addFrame(time, move(orientation));
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

                node.orientation().addFrame(time, move(orientation));
            }
            break;
        default:
            throw runtime_error("Unexpected number of columns: " + to_string(key.numColumns));
    }

    node.orientation().update();
}

void MdlReader::readScaleController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.scale());
}

void MdlReader::readSelfIllumColorController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readVectorController(key, data, node.selfIllumColor());
}

void MdlReader::readAlphaController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.alpha());
}

void MdlReader::readColorController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readVectorController(key, data, node.color());
}

void MdlReader::readRadiusController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.radius());
}

void MdlReader::readShadowRadiusController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.shadowRadius());
}

void MdlReader::readVerticalDisplacementController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.verticalDisplacement());
}

void MdlReader::readMultiplierController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.multiplier());
}

void MdlReader::readAlphaEndController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.alphaEnd());
}

void MdlReader::readAlphaStartController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.alphaStart());
}

void MdlReader::readBirthrateController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.birthrate());
}

void MdlReader::readBounceCoController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.bounceCo());
}

void MdlReader::readCombineTimeController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.combineTime());
}

void MdlReader::readDragController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.drag());
}

void MdlReader::readFPSController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.fps());
}

void MdlReader::readFrameEndController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.frameEnd());
}

void MdlReader::readFrameStartController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.frameStart());
}

void MdlReader::readGravController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.grav());
}

void MdlReader::readLifeExpController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.lifeExp());
}

void MdlReader::readMassController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.mass());
}

void MdlReader::readP2PBezier2Controller(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.p2pBezier2());
}

void MdlReader::readP2PBezier3Controller(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.p2pBezier3());
}

void MdlReader::readParticleRotController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.particleRot());
}

void MdlReader::readRandVelController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.randVel());
}

void MdlReader::readSizeStartController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.sizeStart());
}

void MdlReader::readSizeEndController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.sizeEnd());
}

void MdlReader::readSizeStartYController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.sizeStartY());
}

void MdlReader::readSizeEndYController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.sizeEndY());
}

void MdlReader::readSpreadController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.spread());
}

void MdlReader::readThresholdController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.threshold());
}

void MdlReader::readVelocityController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.velocity());
}

void MdlReader::readXSizeController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.xSize());
}

void MdlReader::readYSizeController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.ySize());
}

void MdlReader::readBlurLengthController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.blurLength());
}

void MdlReader::readLightingDelayController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.lightingDelay());
}

void MdlReader::readLightingRadiusController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.lightingRadius());
}

void MdlReader::readLightingScaleController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.lightingScale());
}

void MdlReader::readLightingSubDivController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.lightingSubDiv());
}

void MdlReader::readLightingZigZagController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.lightingZigZag());
}

void MdlReader::readAlphaMidController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.alphaMid());
}

void MdlReader::readPercentStartController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.percentStart());
}

void MdlReader::readPercentMidController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.percentMid());
}

void MdlReader::readPercentEndController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.percentEnd());
}

void MdlReader::readSizeMidController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.sizeMid());
}

void MdlReader::readSizeMidYController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.sizeMidY());
}

void MdlReader::readRandomBirthRateController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.randomBirthRate());
}

void MdlReader::readTargetSizeController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.targetSize());
}

void MdlReader::readNumControlPtsController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.numControlPts());
}

void MdlReader::readControlPtRadiusController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.controlPtRadius());
}

void MdlReader::readControlPtDelayController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.controlPtDelay());
}

void MdlReader::readTangentSpreadController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.tangentSpread());
}

void MdlReader::readTangentLengthController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.tangentLength());
}

void MdlReader::readColorMidController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readVectorController(key, data, node.colorMid());
}

void MdlReader::readColorEndController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readVectorController(key, data, node.colorEnd());
}

void MdlReader::readColorStartController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readVectorController(key, data, node.colorStart());
}

void MdlReader::readDetonateController(const ControllerKey &key, const vector<float> &data, ModelNode &node) {
    readFloatController(key, data, node.detonate());
}

} // namespace graphics

} // namespace reone
