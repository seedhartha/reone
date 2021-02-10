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

#include "lipfile.h"

using namespace std;

namespace reone {

namespace render {

LipFile::LipFile() : BinaryFile(8, "LIP V1.0") {
}

void LipFile::doLoad() {
    // based on https://github.com/KobaltBlu/KotOR.js/blob/master/js/resource/LIPObject.js

    float length = readFloat();
    uint32_t entryCount = readUint32();

    vector<LipAnimation::Keyframe> keyframes;
    for (uint32_t i = 0; i < entryCount; ++i) {
        LipAnimation::Keyframe keyframe;
        keyframe.time = readFloat();
        keyframe.shape = readByte();
        keyframes.push_back(move(keyframe));
    }

    _animation = make_shared<LipAnimation>(move(keyframes));
}

} // namespace render

} // namespace reone
