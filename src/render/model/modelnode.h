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

#include <unordered_map>

#include "glm/gtx/quaternion.hpp"

#include "../emitter.h"
#include "../mesh/modelmesh.h"

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
    struct Light {
        int priority { 1 };
        bool ambientOnly { false };
        bool affectDynamic { false };
        bool shadow { false };
    };

    struct Skin {
        std::unordered_map<uint16_t, uint16_t> nodeIdxByBoneIdx;
    };

    ModelNode(int index, const ModelNode *parent = nullptr);

    void initGL();

    bool isSelfIllumEnabled() const { return _selfIllumEnabled; }

    bool getPosition(float time, glm::vec3 &position, float scale = 1.0f) const;
    bool getOrientation(float time, glm::quat &orientation) const;
    bool getScale(float time, float &scale) const;

    const glm::vec3 &getCenterOfAABB() const;

    int index() const { return _index; }
    const ModelNode *parent() const { return _parent; }
    uint16_t nodeNumber() const { return _nodeNumber; }
    const std::string &name() const { return _name; }
    const glm::vec3 &position() const { return _position; }
    const glm::quat &orientation() const { return _orientation; }
    const glm::mat4 &localTransform() const { return _localTransform; }
    const glm::mat4 &absoluteTransform() const { return _absTransform; }
    const glm::mat4 &absoluteTransformInverse() const { return _absTransformInv; }
    const glm::vec3 &color() const { return _color; }
    const glm::vec3 &selfIllumColor() const { return _selfIllumColor; }
    float alpha() const { return _alpha; }
    float radius() const { return _radius; }
    float multiplier() const { return _multiplier; }
    std::shared_ptr<Light> light() const { return _light; }
    std::shared_ptr<ModelMesh> mesh() const { return _mesh; }
    std::shared_ptr<Skin> skin() const { return _skin; }
    std::shared_ptr<Emitter> emitter() const { return _emitter; }
    const std::vector<std::shared_ptr<ModelNode>> &children() const { return _children; }

private:
    struct PositionKeyframe {
        float time { 0.0f };
        glm::vec3 position { 0.0f };
    };

    struct OrientationKeyframe {
        float time { 0.0f };
        glm::quat orientation { 1.0f, 0.0f, 0.0f, 0.0f };
    };

    struct ScaleKeyframe {
        float time { 0.0f };
        float scale { 0.0f };
    };

    int _index { 0 };
    const ModelNode *_parent { nullptr };
    uint16_t _flags { 0 };
    uint16_t _nodeNumber { 0 };
    std::string _name;
    glm::vec3 _position { 0.0f };
    glm::quat _orientation { 1.0f, 0.0f, 0.0f, 0.0f };
    glm::mat4 _localTransform { 1.0f };
    glm::mat4 _absTransform { 1.0f };
    glm::mat4 _absTransformInv { 1.0f };
    std::vector<PositionKeyframe> _positionFrames;
    std::vector<OrientationKeyframe> _orientationFrames;
    std::vector<ScaleKeyframe> _scaleFrames;
    glm::vec3 _color { 0.0f };
    bool _selfIllumEnabled { false };
    glm::vec3 _selfIllumColor { 0.0f };
    float _alpha { 1.0f };
    float _radius { 0.0f };
    float _multiplier { 1.0f };
    std::shared_ptr<Light> _light;
    std::shared_ptr<ModelMesh> _mesh;
    std::shared_ptr<Skin> _skin;
    std::shared_ptr<Emitter> _emitter;
    std::vector<std::shared_ptr<ModelNode>> _children;

    ModelNode(const ModelNode &) = delete;
    ModelNode &operator=(const ModelNode &) = delete;

    friend class MdlFile;
    friend class Gr2File;
};

} // namespace render

} // namespace reone
