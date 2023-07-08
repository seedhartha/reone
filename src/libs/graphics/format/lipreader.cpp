/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/graphics/format/lipreader.h"

#include "reone/graphics/lipanimation.h"
#include "reone/resource/format/signutil.h"

using namespace reone::resource;

namespace reone {

namespace graphics {

void LipReader::load() {
    // based on https://github.com/KobaltBlu/KotOR.js/blob/master/js/resource/LIPObject.js

    checkSignature(_lip, std::string("LIP V1.0", 8));

    float length = _lip.readFloat();
    uint32_t entryCount = _lip.readUint32();

    std::vector<LipAnimation::Keyframe> keyframes;
    for (uint32_t i = 0; i < entryCount; ++i) {
        LipAnimation::Keyframe keyframe;
        keyframe.time = _lip.readFloat();
        keyframe.shape = _lip.readByte();
        keyframes.push_back(std::move(keyframe));
    }

    _animation = std::make_shared<LipAnimation>(_name, length, std::move(keyframes));
}

} // namespace graphics

} // namespace reone
