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

#include <stdexcept>

#include "glm/gtc/type_ptr.hpp"

#include "../../common/log.h"
#include "../../render/model/model.h"

using namespace std;

using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace tor {

JbaFile::JbaFile(const string &resRef, shared_ptr<Model> skeleton) :
    BinaryFile(4, "\0\0\0\0"),
    _resRef(resRef),
    _skeleton(skeleton) {

    if (!skeleton) {
        throw invalid_argument("skeleton must not be null");
    }
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
    _length = 10.0f * readFloat();
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
        part.keyframesSize = readUint32();
        debug(boost::format("JBA: part %d header: %u %u") % i % part.keyframeIdx % part.keyframesSize);
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
        bone.translationStride = glm::make_vec3(dataPtr + 0);
        bone.translationBase = glm::make_vec3(dataPtr + 3);
        bone.orientationStride = glm::make_vec3(dataPtr + 6);
        bone.orientationBase = glm::make_vec3(dataPtr + 9);

        debug(boost::format("JBA: bone %d data: %s, %s, %s, %s")
            % i
            % describeVector(bone.translationStride) % describeVector(bone.translationBase)
            % describeVector(bone.orientationStride) % describeVector(bone.orientationBase), 2);

        _bones.push_back(move(bone));

        dataPtr += 12;
    }
}

void JbaFile::loadPartData() {
    for (uint32_t i = 0; i < _numParts; ++i) {
        uint32_t start = alignAt80(tell());
        seek(start);

        _parts[i].keyframes = readPartKeyframes();

        seek(start + _parts[i].keyframesSize);
    }
}

static glm::vec3 decompressPosition(uint32_t value, const glm::vec3 &base, const glm::vec3 &stride) {
    float x = ((value >> 21) & 0x7ff);
    float y = ((value >> 10) & 0x7ff);
    float z = (value & 0x3ff);

    return glm::vec3(base + stride * glm::vec3(x, y, z));
}

static glm::quat decompressOrientation(const uint16_t *values, const glm::vec3 &base, const glm::vec3 &stride) {
    bool sign = (values[0] & 0x8000) != 0;

    float x = (values[0] & 0x7fff);
    float y = (values[1] & 0xffff);
    float z = (values[2] & 0xffff);

    glm::vec3 axis(base + stride * glm::vec3(x, y, z));
    float w;

    float dot = glm::dot(axis, axis);
    if (dot > 1.0f) {
        w = 0.0f;
    } else {
        w = glm::sqrt(1.0f - dot);
        if (sign) {
            w *= -1.0f;
        }
    }

    return glm::normalize(glm::quat(w, axis));
}

static string describeQuaternion(const glm::quat &q) {
    return str(boost::format("(%.06f, %.06f, %.06f, %.06f)") % q.x % q.y % q.z % q.w);
}

vector<vector<JbaFile::JbaKeyframe>> JbaFile::readPartKeyframes() {
    vector<vector<JbaKeyframe>> keyframes;

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
        vector<JbaKeyframe> boneKeyframes;
        boneKeyframes.resize(keyframeLayoutPtr[0]);

        // Decompress 48-bit orientation poses
        for (uint32_t j = 0; j < keyframeLayoutPtr[0]; ++j) {
            vector<uint16_t> values(readArray<uint16_t>(3));
            boneKeyframes[j].orientation = decompressOrientation(&values[0], _bones[i].orientationBase, _bones[i].orientationStride);
            debug(boost::format("JBA: bone %u: keyframe %u: orientation: %04X %04X %04X -> %s") % i % j % values[0] % values[1] % values[2] % describeQuaternion(boneKeyframes[j].orientation), 2);
        }
        // Decompress 32-bit translation poses, if any
        for (uint32_t j = 0; j < keyframeLayoutPtr[2]; ++j) {
            uint32_t value = readUint32();
            boneKeyframes[j].translation = decompressPosition(value, _bones[i].translationBase, _bones[i].translationStride);
            debug(boost::format("JBA: bone %u: keyframe %u: translation: %08X -> %s") % i % j % value % describeVector(boneKeyframes[j].translation), 2);
        }

        keyframes.push_back(move(boneKeyframes));

        keyframeLayoutPtr += 4;
    }

    return move(keyframes);
}

void JbaFile::loadKeyframes() {
    uint32_t pos = alignAt80(tell());
    seek(pos);

    ignore(8);

    vector<float> valuesAt08(readArray<float>(12));
    _translationStride = glm::make_vec3(&valuesAt08[0]);
    _translationBase = glm::make_vec3(&valuesAt08[3]);
    _orientationStride = glm::make_vec3(&valuesAt08[6]);
    _orientationBase = glm::make_vec3(&valuesAt08[9]);
    debug(boost::format("JBA: translationStride=%s translationBase=%s orientationStride=%s orientationBase=%s") %
        describeVector(_translationStride) % describeVector(_translationBase) %
        describeVector(_orientationStride) % describeVector(_orientationBase));

    _numKeyframes = readUint32();
    debug("JBA: numKeyframes=" + to_string(_numKeyframes));
    _keyframes.resize(_numKeyframes);

    ignore(3 * sizeof(uint32_t));

    vector<uint16_t> valuesAt48(readArray<uint16_t>(3 * _numKeyframes));
    const uint16_t *valuesAt48Ptr = &valuesAt48[0];
    for (uint32_t i = 0; i < _numKeyframes; ++i) {
        glm::quat orientation(decompressOrientation(valuesAt48Ptr, _orientationBase, _orientationStride));
        debug(boost::format("JBA: keyframe %d orientation: %04X %04X %04X -> %s") % i %
            valuesAt48Ptr[0] % valuesAt48Ptr[1] % valuesAt48Ptr[2] %
            describeQuaternion(orientation), 2);

        _keyframes[i].orientation = move(orientation);
        valuesAt48Ptr += 3;
    }
    if (_numKeyframes % 2 != 0) {
        ignore(2);
    }

    vector<uint32_t> keyframeValues(readArray<uint32_t>(_numKeyframes));
    for (uint32_t i = 0; i < _numKeyframes; ++i) {
        glm::vec3 translation(decompressPosition(keyframeValues[i], _translationBase, _translationStride));
        debug(boost::format("JBA: keyframe %d translation: %08X -> %s") % i % keyframeValues[i] % describeVector(translation), 2);
        _keyframes[i].translation = move(translation);
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
    if (_numParts == 0 || _parts[0].keyframes.empty()) return;

    // Determine the total number of keyframes

    int numKeyframes = 0;
    for (uint32_t i = 0; i < _numParts; ++i) {
        numKeyframes += _parts[i].keyframes[0].size();
    }
    float step = _length / static_cast<float>(numKeyframes - 1);


    // Convert keyframes to model nodes

    int index = 0;
    auto rootNode = make_shared<ModelNode>(index++);
    rootNode->setName(_resRef + "_root");

    for (uint32_t i = 0; i < _numParts; ++i) {
        vector<vector<JbaKeyframe>> partKeyframes(_parts[i].keyframes);

        for (uint32_t j = 0; j < _numBones; ++j) {
            auto node = make_shared<ModelNode>(index);
            node->setNodeNumber(index);
            node->setName(_bones[j].name);

            auto skeletonNode = _skeleton->findNodeByName(_bones[j].name);
            if (!skeletonNode) continue;

            vector<JbaKeyframe> boneKeyframes(partKeyframes[j]);
            for (size_t k = 0; k < boneKeyframes.size(); ++k) {
                float time = k * step;

                ModelNode::Keyframe position;
                position.time = time;
                position.translation = boneKeyframes[k].translation * _translationBase;
                node->addTranslationKeyframe(move(position));

                ModelNode::Keyframe orientation;
                orientation.time = time;
                orientation.orientation = skeletonNode->orientation() * boneKeyframes[k].orientation;
                node->addOrientationKeyframe(move(orientation));
            }

            rootNode->addChild(move(node));
            ++index;
        }
    }


    // Create the animation

    vector<Animation::Event> events;
    _animation = make_shared<Animation>(_resRef, _length, 0.5f * _length, move(events), move(rootNode));
}

} // namespace tor

} // namespace reone
