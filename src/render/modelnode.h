/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include <map>

#include "glm/gtx/quaternion.hpp"

#include "mesh/modelmesh.h"

namespace reone {

namespace render {

class Model;

/**
 * Part of a 3D model, which is a tree-like data structure. Contains
 * position and orientation keyframes, which are used in animation.
 * May have a mesh associated with it.
 *
 * @see reone::render::Model
 * @see reone::render::ModelMesh
 */
class ModelNode {
public:
    struct Skin {
        std::map<uint16_t, uint16_t> nodeIdxByBoneIdx;
    };

    ModelNode(int index, const ModelNode *parent = nullptr);

    void initGL();

    int index() const;
    const ModelNode *parent() const;
    uint16_t nodeNumber() const;
    const std::string &name() const;
    const glm::vec3 &position() const;
    const glm::quat &orientation() const;
    const glm::mat4 &absoluteTransform() const;
    const glm::mat4 &absoluteTransformInverse() const;
    bool getPosition(float time, glm::vec3 &position) const;
    bool getOrientation(float time, glm::quat &orientation) const;
    float alpha() const;
    std::shared_ptr<ModelMesh> mesh() const;
    std::shared_ptr<Skin> skin() const;
    const std::vector<std::shared_ptr<ModelNode>> &children() const;

private:
    struct PositionKeyframe {
        float time { 0.0f };
        glm::vec3 position { 0.0f };
    };

    struct OrientationKeyframe {
        float time { 0.0f };
        glm::quat orientation { 1.0f, 0.0f, 0.0f, 0.0f };
    };

    int _index { 0 };
    const ModelNode *_parent { nullptr };
    uint16_t _nodeNumber { 0 };
    std::string _name;
    glm::vec3 _position { 0.0f };
    glm::quat _orientation { 1.0f, 0.0f, 0.0f, 0.0f };
    glm::mat4 _absTransform { 1.0f };
    glm::mat4 _absTransformInv { 1.0f };
    std::vector<PositionKeyframe> _positionFrames;
    std::vector<OrientationKeyframe> _orientationFrames;
    float _alpha { 1.0f };
    std::shared_ptr<ModelMesh> _mesh;
    std::shared_ptr<Skin> _skin;
    std::vector<std::shared_ptr<ModelNode>> _children;

    ModelNode(const ModelNode &) = delete;
    ModelNode &operator=(const ModelNode &) = delete;

    friend class resources::MdlFile;
};

} // namespace render

} // namespace reone
