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

#include "jbafile.h"

#include <algorithm>
#include <stack>

#include "glm/gtc/type_ptr.hpp"

#include "../../common/log.h"
#include "../../render/model/model.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace render {

JbaFile::JbaFile(const string &resRef, const shared_ptr<Model> &skeleton) :
    BinaryFile(4, "\0\0\0\0"),
    _resRef(resRef),
    _skeleton(skeleton) {
}

void JbaFile::doLoad() {
    // This is a reference implementation of https://github.com/seedhartha/reone/wiki/SWTOR-Research

    loadHeader();
    loadPartHeaders();
    loadBoneData();
    loadPartData();
    loadTimestamps();
    loadBones();
    loadAnimation();
}

void JbaFile::loadHeader() {
    _length = readFloat();
    _fps = readFloat();
    _numParts = readUint32();

    ignore(8);

    _numBones = readUint32();

    ignore(12);

    debug(boost::format("JBA: length=%.06f numParts=%d, numBones=%d") % _length % _numParts % _numBones);
}

void JbaFile::loadPartHeaders() {
    for (uint32_t i = 0; i < _numParts; ++i) {
        JbaPart part;
        part.timestampIndex = readUint32();
        part.dataSize = readUint32();
        debug(boost::format("JBA: part %d header: %u %u") % i % part.timestampIndex % part.dataSize);
        _parts.push_back(move(part));
    }
    ignore(4 * _numParts);
}

static constexpr size_t alignAt80(size_t offset) {
    size_t result = 0;
    while (result < offset) {
        result += 0x80;
    }
    return result;
}

static string describeVector(const glm::vec3 &vec) {
    return str(boost::format("(%.06f, %.06f, %.06f)") % vec.x % vec.y % vec.z);
}

void JbaFile::loadBoneData() {
    seek(alignAt80(tell()));
    vector<uint8_t> data(readArray<uint8_t>(48ll * _numBones));
    const float *dataPtr = reinterpret_cast<float *>(&data[0]);

    for (uint32_t i = 0; i < _numBones; ++i) {
        glm::vec3 minpos(glm::make_vec3(dataPtr + 0));
        glm::vec3 maxpos(glm::make_vec3(dataPtr + 3));
        glm::vec3 minrot(glm::make_vec3(dataPtr + 6));
        glm::vec3 maxrot(glm::make_vec3(dataPtr + 9));

        debug(boost::format("JBA: bone %d data: %s, %s, %s, %s")
            % i
            % describeVector(minpos) % describeVector(maxpos)
            % describeVector(minrot) % describeVector(maxrot), 2);

        JbaBone bone;
        bone.data.minPosition = move(minpos);
        bone.data.maxPosition = move(maxpos);
        bone.data.minRotation = move(minrot);
        bone.data.maxRotation = move(maxrot);
        _bones.push_back(move(bone));

        dataPtr += 12;
    }
}

void JbaFile::loadPartData() {
    for (uint32_t i = 0; i < _numParts; ++i) {
        uint32_t start = alignAt80(tell());
        seek(start);

        _parts[i].data = readPartData();

        seek(start + _parts[i].dataSize);
    }
}

glm::vec3 JbaFile::decompressPosition(int boneIdx, uint32_t value) const {
    float x = (value & 0x3ff) / 1023.0f;
    float y = ((value >> 10) & 0xffe) / 4094.0f;
    float z = ((value >> 21) & 0xffe) / 4094.0f;
    return glm::vec3(x, y, z);
}

glm::quat JbaFile::decompressOrientation(int boneIdx, uint16_t *values) const {
    float nx = values[0] / 65535.0f;
    float ny = values[1] / 65535.0f;
    float nz = values[2] / 65535.0f;

    const BoneData &boneData = _bones[boneIdx].data;
    glm::vec3 v(glm::mix(boneData.minRotation, boneData.maxRotation, glm::vec3(nx, ny, nz)));
    float w;

    float dot = glm::dot(v, v);
    if (dot >= 1.0f) {
        float len = glm::sqrt(dot);
        v /= len;
        w = 0.0f;
    } else {
        w = -glm::sqrt(1.0f - dot);
    }

    return glm::quat(w, v);
}

static string describeQuaternion(const glm::quat &q) {
    return str(boost::format("(%.06f, %.06f, %.06f, %.06f)") % q.x % q.y % q.z % q.w);
}

unique_ptr<JbaFile::PartData> JbaFile::readPartData() {
    auto partData = make_unique<JbaFile::PartData>();

    ignore(8); // number of bones again

    // Determine the size of the keyframes section
    int dataSize = 0;
    vector<uint32_t> keyframeLayout(readArray<uint32_t>(4 * _numBones));
    for (uint32_t i = 0; i < 4 * _numBones; i += 4) {
        debug(boost::format("JBA: bone %d keyframe layout: %u %u %u %u") % (i / 4) % keyframeLayout[i + 0] % keyframeLayout[i + 1ll] % keyframeLayout[i + 2ll] % keyframeLayout[i + 3ll], 2);
        dataSize += 6 * keyframeLayout[i + 0] + 4 * keyframeLayout[i + 2ll];
    }

    uint32_t pos = tell();
    debug(boost::format("JBA: part data: reading %d bytes from %X to %X") % dataSize % pos % (pos + dataSize - 1));

    const uint32_t *keyframeLayoutPtr = &keyframeLayout[0];
    for (uint32_t i = 0; i < _numBones; ++i) {
        vector<JbaKeyframe> keyframes;
        keyframes.resize(keyframeLayoutPtr[0]);

        // Decompress 48-bit orientation poses
        for (uint32_t j = 0; j < keyframeLayoutPtr[0]; ++j) {
            vector<uint16_t> values(readArray<uint16_t>(3));
            keyframes[j].orientation = decompressOrientation(i, &values[0]);
            debug(boost::format("JBA: bone %u: keyframe %u: orientation: %04X %04X %04X -> %s") % i % j % values[0] % values[1] % values[2] % describeQuaternion(keyframes[j].orientation), 2);
        }
        // Decompress 32-bit translation poses, if any
        for (uint32_t j = 0; j < keyframeLayoutPtr[2]; ++j) {
            uint32_t value = readUint32();
            keyframes[j].position = decompressPosition(i, value);
            debug(boost::format("JBA: bone %u: keyframe %u: translation: %08X -> %s") % i % j % value % describeVector(keyframes[j].position), 2);
        }

        partData->keyframes.push_back(move(keyframes));

        keyframeLayoutPtr += 4;
    }

    return move(partData);
}

void JbaFile::loadTimestamps() {
    uint32_t pos = alignAt80(tell());
    seek(pos);

    ignore(8);

    vector<float> valuesAt08(readArray<float>(12));
    debug(boost::format("JBA: timestamps: %.06f %.06f %.06f %.06f %.06f %.06f %.06f %.06f %.06f %.06f %.06f %.06f")
        % valuesAt08[0] % valuesAt08[1] % valuesAt08[2] % valuesAt08[3]
        % valuesAt08[4] % valuesAt08[5] % valuesAt08[6] % valuesAt08[7]
        % valuesAt08[8] % valuesAt08[9] % valuesAt08[10] % valuesAt08[11]);

    _numTimestamps = readUint32();
    debug("JBA: numTimestamps=" + to_string(_numTimestamps));

    ignore(12);

    vector<uint16_t> valuesAt48(readArray<uint16_t>(3 * _numTimestamps));
    const uint16_t *valuesAt48Ptr = &valuesAt48[0];
    for (uint32_t i = 0; i < _numTimestamps; ++i) {
        debug(boost::format("JBA: timestamp %d: %X %X %X") % i % valuesAt48Ptr[0] % valuesAt48Ptr[1] % valuesAt48Ptr[2], 2);
        valuesAt48Ptr += 3;
    }
    if (_numTimestamps % 2 != 0) {
        ignore(2);
    }

    vector<uint16_t> keyframeValues(readArray<uint16_t>(2 * _numTimestamps));
    for (uint32_t i = 0; i < _numTimestamps; ++i) {
        debug(boost::format("JBA: timestamp %d: %X %X") % i % keyframeValues[2ll * i + 0] % keyframeValues[2ll * i + 1], 2);
    }
}

void JbaFile::loadBones() {
    uint32_t numBones = readUint32();

    ignore(16);

    for (uint32_t i = 0; i < numBones; ++i) {
        _bones[i].index = readUint32();
    }
    vector<uint32_t> nameOffsets(readArray<uint32_t>(numBones));

    for (uint32_t i = 0; i < numBones; ++i) {
        _bones[i].name = _reader->getCString();
        debug(boost::format("JBA: bone %d: %s") % i % _bones[i].name);
    }
}

void JbaFile::loadAnimation() {
    if (_numParts == 0) return;

    if (_numParts > 1) {
        warn("JBA: animations with more than one part are not supported");
        return;
    }

    int index = 0;
    auto rootNode = make_shared<ModelNode>(index++);
    float step = _length / static_cast<float>(_numTimestamps - 1);

    stack<ModelNode *> modelNodes;
    modelNodes.push(&_skeleton->rootNode());

    while (!modelNodes.empty()) {
        ModelNode *modelNode = modelNodes.top();
        modelNodes.pop();

        // Match a skeleton model node to a bone
        auto maybeBone = find_if(_bones.begin(), _bones.end(), [this, &modelNode](auto &bone) {
            return bone.name == modelNode->_name;
        });
        if (maybeBone != _bones.end()) {
            // Convert a skeleton model node to an animation model node
            auto animNode = make_shared<ModelNode>(index++);
            animNode->_name = modelNode->_name;

            uint32_t boneIdx = maybeBone->index;
            const vector<JbaKeyframe> &keyframes = _parts[0].data->keyframes[boneIdx];

            for (size_t i = 0; i < _numTimestamps + 2ll; ++i) {
                const JbaKeyframe &keyframe = keyframes[i];
                float time = i * step;

                ModelNode::OrientationKeyframe orient;
                orient.time = time;
                orient.orientation = modelNode->_orientation * keyframe.orientation;
                animNode->_orientationFrames.push_back(move(orient));
            }

            rootNode->_children.push_back(move(animNode));
        }

        for (auto &child : modelNode->_children) {
            modelNodes.push(child.get());
        }
    }

    _animation = make_shared<Animation>("pause1" /* _resRef */, _length, 0.5f * _length, vector<Animation::Event>(), rootNode);
}

} // namespace render

} // namespace reone
