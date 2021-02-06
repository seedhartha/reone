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
        glm::vec3 translation { 0.0f };
        glm::quat orientation { 1.0f, 0.0f, 0.0, 0.0f };
    };

    struct PartData {
        std::vector<std::vector<JbaKeyframe>> keyframes;
    };

    struct JbaPart {
        uint32_t keyframeIdx { 0 };
        uint32_t dataSize;
        std::unique_ptr<PartData> data;
    };

    struct JbaBone {
        glm::vec3 minTranslation { 0.0f };
        glm::vec3 maxTranslation { 0.0f };
        glm::vec3 minOrientation { 0.0f };
        glm::vec3 maxOrientation { 0.0f };
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

    glm::vec3 _maxTranslation { 0.0f };
    glm::vec3 _minTranslation { 0.0f };
    glm::vec3 _maxOrientation { 0.0f };
    glm::vec3 _minOrientation { 0.0f };

    std::vector<JbaPart> _parts;
    std::vector<glm::quat> _orientationFrames;
    std::vector<glm::vec3> _positionFrames;
    std::vector<JbaBone> _bones;
    std::shared_ptr<Animation> _animation;

    void doLoad();

    void loadHeader();
    void loadPartHeaders();
    void loadBoneData();
    void loadPartData();
    void loadKeyframes();
    void loadBones();
    void loadAnimation();

    std::unique_ptr<PartData> readPartData();

    int getPartByKeyframe(int keyframeIdx) const;
};

} // namespace render

} // namespace reone
