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

#include <memory>
#include <unordered_map>

#include <boost/noncopyable.hpp>

#include "glm/gtx/quaternion.hpp"

#include "aabbnode.h"
#include "animatedproperty.h"
#include "emitter.h"
#include "modelmesh.h"

namespace reone {

namespace render {

class Model;

/**
 * Node of a 3D model or an animation, which are tree-like data structures.
 * Model nodes have spatial properties and can have an arbitary number of
 * children.
 *
 * When part of an animation, certain properties of a model node can be
 * animated. These are position, orientation, scale, alpha and self-illumination
 * color.
 *
 * Model nodes can be specialized to represent meshes, lights, emitters, etc.
 *
 * @see reone::render::Model
 * @see reone::render::Animation
 */
class ModelNode : boost::noncopyable {
public:
    struct Light {
        int priority { 0 };
        int dynamicType { 0 };
        bool ambientOnly { false };
        bool affectDynamic { false };
        bool shadow { false };
    };

    struct Reference {
        std::shared_ptr<Model> model;
        bool reattachable { false };
    };

    struct Skin {
        std::unordered_map<uint16_t, uint16_t> nodeIdxByBoneIdx;
    };

    struct Danglymesh {
        float displacement { 0.0f };
        float tightness { 0.0f };
        float period { 0.0f };
        std::vector<float> constraints;
    };

    ModelNode(int index, const ModelNode *parent = nullptr);

    void init();

    /**
     * Adds the specified node to the list of this nodes children. Also sets the
     * specified nodes parent pointer to this node.
     */
    void addChild(std::shared_ptr<ModelNode> node);

    /**
     * Recursively computes the local transform, position and orientation of
     * this node and its children. Absolute transform must be set prior to
     * calling this method.
     */
    void computeLocalTransforms();

    /**
     * Recursively computes the absolute transform of this node and its
     * children. Local transform must be set prior to calling this method.
     */
    void computeAbsoluteTransforms();

    bool isAABB() const { return static_cast<bool>(_aabb); }
    bool isSaber() const { return _saber; }

    bool getPosition(int leftFrameIdx, int rightFrameIdx, float factor, glm::vec3 &position) const;
    bool getOrientation(int leftFrameIdx, int rightFrameIdx, float factor, glm::quat &orientation) const;
    bool getScale(int leftFrameIx, int rightFrameIdx, float factor, float &scale) const;

    const glm::vec3 &getCenterOfAABB() const;

    int index() const { return _index; }
    const ModelNode *parent() const { return _parent; }
    uint16_t nodeNumber() const { return _nodeNumber; }
    const std::string &name() const { return _name; }
    const std::vector<std::shared_ptr<ModelNode>> &children() const { return _children; }

    // Transformation

    const glm::vec3 &position() const { return _position; }
    const glm::quat &orientation() const { return _orientation; }
    const glm::mat4 &localTransform() const { return _localTransform; }
    const glm::mat4 &absoluteTransform() const { return _absTransform; }
    const glm::mat4 &absoluteTransformInverse() const { return _absTransformInv; }

    // END Transformation

    // Components

    std::shared_ptr<Light> light() const { return _light; }
    std::shared_ptr<Emitter> emitter() const { return _emitter; }
    std::shared_ptr<Reference> reference() const { return _reference; }
    std::shared_ptr<ModelMesh> mesh() const { return _mesh; }
    std::shared_ptr<Skin> skin() const { return _skin; }
    std::shared_ptr<Danglymesh> danglymesh() const { return _danglymesh; }
    std::shared_ptr<AABBNode> aabb() const { return _aabb; }

    // END Components

    // Animation

    const AnimatedProperty<glm::vec3> &positions() const { return _positions; }
    AnimatedProperty<glm::vec3> &positions() { return _positions; }
    const AnimatedProperty<glm::quat, SlerpInterpolator> &orientations() const { return _orientations; }
    AnimatedProperty<glm::quat, SlerpInterpolator> &orientations() { return _orientations; }
    const AnimatedProperty<float> &scales() const { return _scales; }
    AnimatedProperty<float> &scales() { return _scales; }
    const AnimatedProperty<float> &alphas() const { return _alphas; }
    AnimatedProperty<float> &alphas() { return _alphas; }
    const AnimatedProperty<glm::vec3> &selfIllumColors() const { return _selfIllumColors; }
    AnimatedProperty<glm::vec3> &selfIllumColors() { return _selfIllumColors; }
    const AnimatedProperty<glm::vec3> &lightColors() const { return _lightColors; }
    AnimatedProperty<glm::vec3> &lightColors() { return _lightColors; }
    const AnimatedProperty<float> &lightMultipliers() const { return _lightMultipliers; }
    AnimatedProperty<float> &lightMultipliers() { return _lightMultipliers; }
    const AnimatedProperty<float> &lightRadii() const { return _lightRadii; }
    AnimatedProperty<float> &lightRadii() { return _lightRadii; }

    // END Animation

private:
    int _index;
    const ModelNode *_parent;

    uint16_t _flags { 0 };
    uint16_t _nodeNumber { 0 };
    std::string _name;
    std::vector<std::shared_ptr<ModelNode>> _children;

    // Transformation

    glm::vec3 _position { 0.0f };
    glm::quat _orientation { 1.0f, 0.0f, 0.0f, 0.0f };
    glm::mat4 _localTransform { 1.0f };
    glm::mat4 _absTransform { 1.0f };
    glm::mat4 _absTransformInv { 1.0f };

    // END Transformation

    // Components

    std::shared_ptr<Light> _light;
    std::shared_ptr<Emitter> _emitter;
    std::shared_ptr<Reference> _reference;
    std::shared_ptr<ModelMesh> _mesh;
    std::shared_ptr<Skin> _skin;
    std::shared_ptr<Danglymesh> _danglymesh;
    std::shared_ptr<AABBNode> _aabb;
    bool _saber { false };

    // END Components

    // Animation

    AnimatedProperty<glm::vec3> _positions;
    AnimatedProperty<glm::quat, SlerpInterpolator> _orientations;
    AnimatedProperty<float> _scales;
    AnimatedProperty<float> _alphas;
    AnimatedProperty<glm::vec3> _selfIllumColors;
    AnimatedProperty<glm::vec3> _lightColors;
    AnimatedProperty<float> _lightMultipliers;
    AnimatedProperty<float> _lightRadii;

    // END Animation

    friend class MdlReader;
};

} // namespace render

} // namespace reone
