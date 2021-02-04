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
#include "../../resource/format/binfile.h"

namespace reone {

namespace render {

class JbaFile : public resource::BinaryFile {
public:
    JbaFile(const std::string &resRef, const std::shared_ptr<Model> &skeleton);

    std::shared_ptr<Animation> animation() const { return _animation; }

private:
    struct JbaKeyframe {
        glm::vec3 position { 0.0f };
        glm::quat orientation { 1.0f, 0.0f, 0.0f, 0.0f };
    };

    struct PartData {
        std::vector<std::vector<JbaKeyframe>> keyframes;
    };

    struct JbaPart {
        uint32_t timestampIndex { 0 };
        uint32_t dataSize;
        std::unique_ptr<PartData> data;
    };

    struct BoneData {
        glm::vec3 minPosition;
        glm::vec3 maxPosition;
        glm::vec3 minRotation;
        glm::vec3 maxRotation;
    };

    struct JbaBone {
        BoneData data;
        uint32_t index { 0 };
        std::string name;
    };

    std::string _resRef;
    std::shared_ptr<render::Model> _skeleton;

    float _length { 0.0f };
    float _fps { 0 };
    uint32_t _numParts { 0 };
    uint32_t _numTimestamps { 0 };
    uint32_t _numBones { 0 };
    std::vector<JbaPart> _parts;
    std::vector<JbaBone> _bones;
    std::shared_ptr<Animation> _animation;

    void doLoad();

    void loadHeader();
    void loadPartHeaders();
    void loadBoneData();
    void loadPartData();
    void loadTimestamps();
    void loadBones();
    void loadAnimation();

    std::unique_ptr<PartData> readPartData();

    glm::vec3 decompressPosition(int boneIdx, uint32_t value) const;
    glm::quat decompressOrientation(int boneIdx, uint16_t *values) const;
};

} // namespace render

} // namespace reone
