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
    loadKeyframes();
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
        part.keyframeIdx = readUint32();
        part.dataSize = readUint32();
        debug(boost::format("JBA: part %d header: %u %u") % i % part.keyframeIdx % part.dataSize);
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
        JbaBone bone;
        bone.minTranslation = glm::make_vec3(dataPtr + 0);
        bone.maxTranslation = glm::make_vec3(dataPtr + 3);
        bone.minOrientation = glm::make_vec3(dataPtr + 6);
        bone.maxOrientation = glm::make_vec3(dataPtr + 9);

        debug(boost::format("JBA: bone %d data: %s, %s, %s, %s")
            % i
            % describeVector(bone.minTranslation) % describeVector(bone.maxTranslation)
            % describeVector(bone.minOrientation) % describeVector(bone.maxOrientation), 2);

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

static glm::vec3 decompressPosition(uint32_t value, const glm::vec3 &min, const glm::vec3 &max) {
    float nx = (value & 0x3ff) / 1023.0f;
    float ny = ((value >> 10) & 0x7ff) / 2047.0f;
    float nz = ((value >> 21) & 0x7ff) / 2047.0f;
    return min + max * glm::vec3(nx, ny, nz);
}

static glm::quat decompressOrientation(const uint16_t *values, const glm::vec3 &min, const glm::vec3 &max) {
    float nx = (values[0] & 0x7fff) / 32767.0f;
    float ny = (values[1] & 0xffff) / 65535.0f;
    float nz = (values[2] & 0xffff) / 65535.0f;
    glm::vec3 v(min + max * glm::vec3(nx, ny, nz));
    float w = glm::sqrt(1.0f - glm::dot(v, v));
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
            keyframes[j].orientation = decompressOrientation(&values[0], _bones[i].minOrientation, _bones[i].maxOrientation);
            debug(boost::format("JBA: bone %u: keyframe %u: orientation: %04X %04X %04X -> %s") % i % j % values[0] % values[1] % values[2] % describeQuaternion(keyframes[j].orientation), 2);
        }
        // Decompress 32-bit translation poses, if any
        for (uint32_t j = 0; j < keyframeLayoutPtr[2]; ++j) {
            uint32_t value = readUint32();
            keyframes[j].translation = decompressPosition(value, _bones[i].minTranslation, _bones[i].maxTranslation);
            debug(boost::format("JBA: bone %u: keyframe %u: translation: %08X -> %s") % i % j % value % describeVector(keyframes[j].translation), 2);
        }

        partData->keyframes.push_back(move(keyframes));

        keyframeLayoutPtr += 4;
    }

    return move(partData);
}

void JbaFile::loadKeyframes() {
    uint32_t pos = alignAt80(tell());
    seek(pos);

    ignore(8);

    vector<float> valuesAt08(readArray<float>(12));
    _maxTranslation = glm::make_vec3(&valuesAt08[0]);
    _minTranslation = glm::make_vec3(&valuesAt08[3]);
    _maxOrientation = glm::make_vec3(&valuesAt08[6]);
    _minOrientation = glm::make_vec3(&valuesAt08[9]);
    debug(boost::format("JBA: maxTranslation=%s minTranslation=%s maxOrientation=%s minOrientation=%s") %
        describeVector(_maxTranslation) % describeVector(_minTranslation) %
        describeVector(_maxOrientation) % describeVector(_minOrientation));

    _numKeyframes = readUint32();
    debug("JBA: numKeyframes=" + to_string(_numKeyframes));

    ignore(3 * sizeof(uint32_t));

    vector<uint16_t> valuesAt48(readArray<uint16_t>(3 * _numKeyframes));
    const uint16_t *valuesAt48Ptr = &valuesAt48[0];
    for (uint32_t i = 0; i < _numKeyframes; ++i) {
        glm::quat orientation(decompressOrientation(valuesAt48Ptr, _minOrientation, _maxOrientation));
        debug(boost::format("JBA: keyframe %d orientation: %04X %04X %04X -> %s") % i %
            valuesAt48Ptr[0] % valuesAt48Ptr[1] % valuesAt48Ptr[2] %
            describeQuaternion(orientation), 2);

        valuesAt48Ptr += 3;
    }
    if (_numKeyframes % 2 != 0) {
        ignore(2);
    }

    vector<uint32_t> keyframeValues(readArray<uint32_t>(_numKeyframes));
    for (uint32_t i = 0; i < _numKeyframes; ++i) {
        glm::vec3 translation(decompressPosition(keyframeValues[i], _minTranslation, _maxTranslation));
        debug(boost::format("JBA: keyframe %d translation: %08X -> %s") % i % keyframeValues[i] % describeVector(translation), 2);
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

    int index = 0;
    auto rootNode = make_shared<ModelNode>(index++);

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

            for (size_t i = 0; i < _numKeyframes; ++i) {
                int partIdx = getPartByKeyframe(i);
                if (partIdx == -1) break;

                const vector<JbaKeyframe> &keyframes = _parts[partIdx].data->keyframes[boneIdx];
                const JbaKeyframe &keyframe = keyframes[i - _parts[partIdx].keyframeIdx];
                float step = _length / static_cast<float>(keyframes.size() - 1);
                float time = i * step;

                ModelNode::PositionKeyframe pos;
                pos.time = time;
                pos.position = keyframe.translation;
                animNode->_positionFrames.push_back(move(pos));

                //ModelNode::OrientationKeyframe orient;
                //orient.time = time;
                //orient.orientation = modelNode->_orientation * keyframe.orientation;
                //animNode->_orientationFrames.push_back(move(orient));
            }

            rootNode->_children.push_back(move(animNode));
        }

        for (auto &child : modelNode->_children) {
            modelNodes.push(child.get());
        }
    }

    _animation = make_shared<Animation>("pause1" /* _resRef */, _length, 0.5f * _length, vector<Animation::Event>(), rootNode);
}

int JbaFile::getPartByKeyframe(int keyframeIdx) const {
    for (uint32_t i = 0; i < _numParts; ++i) {
        if (keyframeIdx >= _parts[i].keyframeIdx) return i;
    }
    return -1;
}

} // namespace render

} // namespace reone
