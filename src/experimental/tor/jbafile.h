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

#pragma once

#include "glm/gtc/quaternion.hpp"
#include "glm/vec3.hpp"

#include "../../render/model/animation.h"
#include "../../render/model/model.h"
#include "../../resource/format/binfile.h"

namespace reone {

namespace tor {

class JbaFile : public resource::BinaryFile {
public:
    JbaFile(const std::string &resRef, std::shared_ptr<render::Model> skeleton);

    std::shared_ptr<render::Animation> animation() const { return _animation; }

private:
    struct JbaKeyframe {
        glm::vec3 translation { 0.0f };
        glm::quat orientation { 1.0f, 0.0f, 0.0, 0.0f };
    };

    struct JbaPart {
        uint32_t keyframeIdx { 0 };
        uint32_t keyframesSize;
        std::vector<std::vector<JbaKeyframe>> keyframes;
    };

    struct JbaBone {
        glm::vec3 translationStride { 0.0f };
        glm::vec3 translationBase { 0.0f };
        glm::vec3 orientationStride { 0.0f };
        glm::vec3 orientationBase { 0.0f };
        uint32_t index { 0 };
        std::string name;
    };

    std::string _resRef;
    std::shared_ptr<render::Model> _skeleton;

    float _length { 0.0f };
    float _fps { 0 };
    uint32_t _numParts { 0 };
    uint32_t _numKeyframes { 0 };
    uint32_t _numBones { 0 };

    glm::vec3 _translationStride { 0.0f };
    glm::vec3 _translationBase { 0.0f };
    glm::vec3 _orientationStride { 0.0f };
    glm::vec3 _orientationBase { 0.0f };

    std::vector<JbaPart> _parts;
    std::vector<JbaKeyframe> _keyframes;
    std::vector<JbaBone> _bones;
    std::shared_ptr<render::Animation> _animation;

    void doLoad();

    void loadHeader();
    void loadPartHeaders();
    void loadBoneData();
    void loadPartData();
    void loadKeyframes();
    void loadBones();
    void loadAnimation();

    std::vector<std::vector<JbaKeyframe>> readPartKeyframes();
};

} // namespace tor

} // namespace reone
