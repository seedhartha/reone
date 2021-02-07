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

#include "glm/gtc/type_ptr.hpp"

#include "../../common/log.h"
#include "../../render/model/model.h"

using namespace std;

using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace tor {

static const glm::vec3 g_translationModifier { -0.001f, 0.001f, -0.001f };

JbaFile::JbaFile(const string &resRef) :
    BinaryFile(4, "\0\0\0\0"),
    _resRef(resRef) {
}

void JbaFile::doLoad() {
    // This is a reference implementation of https://github.com/seedhartha/reone/wiki/SWTOR-Research

    loadHeader();
    loadPartHeaders();
    loadBoneData();
    loadPartData();
    loadKeyframes();
    loadBones();
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

static string describeQuaternion(const glm::quat &q) {
    return str(boost::format("(%.06f, %.06f, %.06f, %.06f)") % q.x % q.y % q.z % q.w);
}

static glm::quat getUnitQuaternion(const glm::vec3 &axis) {
    float w = glm::sqrt(1.0f - glm::dot(axis, axis));
    return glm::normalize(glm::quat(w, axis));
}

void JbaFile::loadBoneData() {
    seek(alignAt80(tell()));
    vector<uint8_t> data(readArray<uint8_t>(48ll * _numBones));
    const float *dataPtr = reinterpret_cast<float *>(&data[0]);

    for (uint32_t i = 0; i < _numBones; ++i) {
        JbaBone bone;
        bone.minTranslation = glm::make_vec3(dataPtr + 0) * g_translationModifier;
        bone.maxTranslation = glm::make_vec3(dataPtr + 3) * g_translationModifier;
        bone.minOrientation = getUnitQuaternion(glm::make_vec3(dataPtr + 6));
        bone.maxOrientation = getUnitQuaternion(glm::make_vec3(dataPtr + 9));

        debug(boost::format("JBA: bone %d data: %s, %s, %s, %s")
            % i
            % describeVector(bone.minTranslation) % describeVector(bone.maxTranslation)
            % describeQuaternion(bone.minOrientation) % describeQuaternion(bone.maxOrientation), 2);

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

static glm::vec3 decompressPosition(uint32_t value, const glm::vec3 &min, const glm::vec3 &max) {
    uint32_t z = (value & 0x3ff);
    uint32_t y = ((value >> 10) & 0x7ff);
    uint32_t x = ((value >> 21) & 0x7ff);

    float nx = (x & 0x1ff) / 511.0f;
    float ny = (y & 0x3ff) / 1023.0f;
    float nz = (z & 0x3ff) / 1023.0f;

    bool sx = (x & !0x3ff) != 0;
    bool sy = (y & !0x3ff) != 0;
    bool sz = (z & !0x1ff) != 0;

    glm::vec3 result(min);
    result.x += (sx ? -1.0f : 1.0f) * nx * max.x;
    result.y += (sy ? -1.0f : 1.0f) * ny * max.y;
    result.z += (sz ? -1.0f : 1.0f) * nz * max.z;

    return move(result);
}

static glm::quat decompressOrientation(const uint16_t *values, const glm::quat &min, const glm::quat &max) {
    float nx = (values[0] & 0x7fff) / 32767.0f;
    float ny = (values[1] & 0x7fff) / 32767.0f;
    float nz = (values[2] & 0x7fff) / 32767.0f;

    bool sx = (values[0] & 0x8000) != 0;
    bool sy = (values[1] & 0x8000) != 0;
    bool sz = (values[2] & 0x8000) != 0;

    glm::vec3 axis(min.x, min.y, min.z);
    axis.x += (sx ? -1.0f : 1.0f) * nx * max.x;
    axis.y += (sy ? -1.0f : 1.0f) * ny * max.y;
    axis.z += (sz ? -1.0f : 1.0f) * nz * max.z;

    return getUnitQuaternion(axis);
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
            boneKeyframes[j].orientation = decompressOrientation(&values[0], _bones[i].minOrientation, _bones[i].maxOrientation);
            debug(boost::format("JBA: bone %u: keyframe %u: orientation: %04X %04X %04X -> %s") % i % j % values[0] % values[1] % values[2] % describeQuaternion(boneKeyframes[j].orientation), 2);
        }
        // Decompress 32-bit translation poses, if any
        for (uint32_t j = 0; j < keyframeLayoutPtr[2]; ++j) {
            uint32_t value = readUint32();
            boneKeyframes[j].translation = decompressPosition(value, _bones[i].minTranslation, _bones[i].maxTranslation);
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
    _maxTranslation = glm::make_vec3(&valuesAt08[0]) * g_translationModifier;
    _minTranslation = glm::make_vec3(&valuesAt08[3]) * g_translationModifier;
    _maxOrientation = getUnitQuaternion(glm::make_vec3(&valuesAt08[6]));
    _minOrientation = getUnitQuaternion(glm::make_vec3(&valuesAt08[9]));
    debug(boost::format("JBA: maxTranslation=%s minTranslation=%s maxOrientation=%s minOrientation=%s") %
        describeVector(_maxTranslation) % describeVector(_minTranslation) %
        describeQuaternion(_maxOrientation) % describeQuaternion(_minOrientation));

    _numKeyframes = readUint32();
    debug("JBA: numKeyframes=" + to_string(_numKeyframes));
    _keyframes.resize(_numKeyframes);

    ignore(3 * sizeof(uint32_t));

    vector<uint16_t> valuesAt48(readArray<uint16_t>(3 * _numKeyframes));
    const uint16_t *valuesAt48Ptr = &valuesAt48[0];
    for (uint32_t i = 0; i < _numKeyframes; ++i) {
        glm::quat orientation(decompressOrientation(valuesAt48Ptr, _minOrientation, _maxOrientation));
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
        glm::vec3 translation(decompressPosition(keyframeValues[i], _minTranslation, _maxTranslation));
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

} // namespace tor

} // namespace reone
