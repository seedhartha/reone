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

#pragma once

#include "aabb.h"
#include "animatedproperty.h"
#include "mesh.h"
#include "texture.h"

#define R_ANIMPROP(a, b, c)                \
    const AnimatedProperty<a> &b() const { \
        return c;                          \
    };                                     \
    AnimatedProperty<a> &b() {             \
        return c;                          \
    };

namespace reone {

namespace graphics {

class Model;

class ModelNode : boost::noncopyable {
public:
    struct Skin {
        std::vector<uint32_t> boneSerial;     /**< node index per bone in depth-first order */
        std::vector<uint16_t> boneNodeNumber; /**< node number per bone */
        std::vector<float> boneMap;           /**< bone index per node in depth-first order */
        std::vector<glm::mat4> boneMatrices;  /**< each matrix is inverse of bone transform in this node space */
    };

    struct UVAnimation {
        glm::vec2 dir {0.0f};
    };

    struct Danglymesh {
        float displacement {0.0f};
        float tightness {0.0f};
        float period {0.0f};
        std::vector<float> constraints;
        std::vector<glm::vec3> positions;
    };

    struct AABBTree {
        enum class Plane {
            None = 0,
            PositiveX = 1,
            PositiveY = 2,
            PositiveZ = 4
        };

        int faceIndex {0};
        Plane mostSignificantPlane {Plane::None};
        AABB aabb;
        std::shared_ptr<AABBTree> left;
        std::shared_ptr<AABBTree> right;
    };

    struct TriangleMesh {
        std::shared_ptr<Mesh> mesh;
        UVAnimation uvAnimation;
        glm::vec3 diffuse {1.0f};
        glm::vec3 ambient {1.0f};
        int transparency {0};

        // Flags

        bool render {false};
        bool shadow {false};
        bool backgroundGeometry {false};

        // END Flags

        // Textures

        std::string diffuseMap;
        std::string lightmap;
        std::string bumpmap;

        // END Textures

        // Specialization

        std::shared_ptr<Skin> skin;
        std::shared_ptr<Danglymesh> danglymesh;
        std::shared_ptr<AABBTree> aabbTree;
        bool saber {false};

        // END Specialization
    };

    struct LensFlare {
        std::string textureName;
        glm::vec3 colorShift {0.0f};
        float position {0.0f};
        float size {0.0f};
    };

    struct Light {
        int priority {0};
        int dynamicType {0};
        bool ambientOnly {false};
        bool affectDynamic {false};
        bool shadow {false};
        bool fading {false};
        float flareRadius {0.0f};
        std::vector<LensFlare> flares;
    };

    struct Emitter {
        enum class UpdateMode {
            Invalid,
            Fountain,
            Single,
            Explosion,
            Lightning
        };

        enum class RenderMode {
            Invalid = 0,
            Normal = 1,
            Linked = 2,
            BillboardToLocalZ = 3,
            BillboardToWorldZ = 4,
            AlignedToWorldZ = 5,
            AlignedToParticleDir = 6,
            MotionBlur = 7
        };

        enum class BlendMode {
            Invalid,
            Normal,
            PunchThrough,
            Lighten
        };

        UpdateMode updateMode {UpdateMode::Invalid};
        RenderMode renderMode {RenderMode::Invalid};
        BlendMode blendMode {BlendMode::Invalid};
        std::string textureName;
        glm::ivec2 gridSize {0};
        int renderOrder {0};
        bool twosided {false};
        bool loop {false};
        bool p2p {false};
        bool p2pBezier {false};
    };

    struct Reference {
        std::string modelName;
        bool reattachable {false};
    };

    using ControllerFrame = std::pair<float, std::vector<float>>;
    using Controllers = std::map<int, std::vector<ControllerFrame>>;

    ModelNode(
        uint16_t number,
        std::string name,
        glm::vec3 restPosition,
        glm::quat restOrientation,
        bool animated,
        ModelNode *parent = nullptr);

    void init();

    void addChild(std::shared_ptr<ModelNode> child);

    bool isAnimated() const { return _animated; }

    uint16_t number() const { return _number; }
    const std::string &name() const { return _name; }
    uint16_t flags() const { return _flags; }
    const ModelNode *parent() const { return _parent; }
    const std::vector<std::shared_ptr<ModelNode>> &children() const { return _children; }
    Controllers &controllers() { return _controllers; }

    void setFlags(uint16_t flags) { _flags = flags; }

    // Transformations

    const glm::vec3 &restPosition() const { return _restPosition; }
    const glm::quat &restOrientation() const { return _restOrientation; }
    const glm::mat4 &localTransform() const { return _localTransform; }
    const glm::mat4 &absoluteTransform() const { return _absTransform; }
    const glm::mat4 &absoluteTransformInverse() const { return _absTransformInv; }

    // END Transformations

    // Specialization

    bool isMesh() const { return static_cast<bool>(_mesh); }
    bool isLight() const { return static_cast<bool>(_light); }
    bool isEmitter() const { return static_cast<bool>(_emitter); }
    bool isReference() const { return static_cast<bool>(_reference); }
    bool isSkinMesh() const { return _mesh && _mesh->skin; }
    bool isDanglymesh() const { return _mesh && _mesh->danglymesh; }
    bool isAABBMesh() const { return _mesh && _mesh->aabbTree; }
    bool isSaberMesh() const { return _mesh && _mesh->saber; }

    std::shared_ptr<TriangleMesh> mesh() const { return _mesh; }
    std::shared_ptr<Light> light() const { return _light; }
    std::shared_ptr<Emitter> emitter() const { return _emitter; }
    std::shared_ptr<Reference> reference() const { return _reference; }

    void setMesh(std::shared_ptr<TriangleMesh> mesh) { _mesh = std::move(mesh); }
    void setLight(std::shared_ptr<Light> light) { _light = std::move(light); }
    void setEmitter(std::shared_ptr<Emitter> emitter) { _emitter = std::move(emitter); }
    void setReference(std::shared_ptr<Reference> reference) { _reference = std::move(reference); }

    // END Specialization

    // Keyframes

    bool getPosition(int leftFrameIdx, int rightFrameIdx, float factor, glm::vec3 &position) const;
    bool getOrientation(int leftFrameIdx, int rightFrameIdx, float factor, glm::quat &orientation) const;
    bool getScale(int leftFrameIx, int rightFrameIdx, float factor, float &scale) const;

    const AnimatedProperty<glm::quat, SlerpInterpolator> &orientation() const { return _orientation; }
    AnimatedProperty<glm::quat, SlerpInterpolator> &orientation() { return _orientation; }

    R_ANIMPROP(glm::vec3, position, _position)
    R_ANIMPROP(float, scale, _scale)

    R_ANIMPROP(glm::vec3, selfIllumColor, _selfIllumColor)
    R_ANIMPROP(float, alpha, _alpha)

    R_ANIMPROP(glm::vec3, color, _color)
    R_ANIMPROP(float, radius, _radius)
    R_ANIMPROP(float, shadowRadius, _shadowRadius)
    R_ANIMPROP(float, verticalDisplacement, _verticalDisplacement)
    R_ANIMPROP(float, multiplier, _multiplier)

    R_ANIMPROP(float, alphaEnd, _alphaEnd)
    R_ANIMPROP(float, alphaStart, _alphaStart)
    R_ANIMPROP(float, birthrate, _birthrate)
    R_ANIMPROP(float, bounceCo, _bounceCo)
    R_ANIMPROP(float, combineTime, _combineTime)
    R_ANIMPROP(float, drag, _drag)
    R_ANIMPROP(float, fps, _fps)
    R_ANIMPROP(float, frameEnd, _frameEnd)
    R_ANIMPROP(float, frameStart, _frameStart)
    R_ANIMPROP(float, grav, _grav)
    R_ANIMPROP(float, lifeExp, _lifeExp)
    R_ANIMPROP(float, mass, _mass)
    R_ANIMPROP(float, p2pBezier2, _p2pBezier2)
    R_ANIMPROP(float, p2pBezier3, _p2pBezier3)
    R_ANIMPROP(float, particleRot, _particleRot)
    R_ANIMPROP(float, randVel, _randVel)
    R_ANIMPROP(float, sizeStart, _sizeStart)
    R_ANIMPROP(float, sizeEnd, _sizeEnd)
    R_ANIMPROP(float, sizeStartY, _sizeStartY)
    R_ANIMPROP(float, sizeEndY, _sizeEndY)
    R_ANIMPROP(float, spread, _spread)
    R_ANIMPROP(float, threshold, _threshold)
    R_ANIMPROP(float, velocity, _velocity)
    R_ANIMPROP(float, xSize, _xSize)
    R_ANIMPROP(float, ySize, _ySize)
    R_ANIMPROP(float, blurLength, _blurLength)
    R_ANIMPROP(float, lightingDelay, _lightingDelay)
    R_ANIMPROP(float, lightingRadius, _lightingRadius)
    R_ANIMPROP(float, lightingScale, _lightingScale)
    R_ANIMPROP(float, lightingSubDiv, _lightingSubDiv)
    R_ANIMPROP(float, lightingZigZag, _lightingZigZag)
    R_ANIMPROP(float, alphaMid, _alphaMid)
    R_ANIMPROP(float, percentStart, _percentStart)
    R_ANIMPROP(float, percentMid, _percentMid)
    R_ANIMPROP(float, percentEnd, _percentEnd)
    R_ANIMPROP(float, sizeMid, _sizeMid)
    R_ANIMPROP(float, sizeMidY, _sizeMidY)
    R_ANIMPROP(float, randomBirthRate, _randomBirthRate)
    R_ANIMPROP(float, targetSize, _targetSize)
    R_ANIMPROP(float, numControlPts, _numControlPts)
    R_ANIMPROP(float, controlPtRadius, _controlPtRadius)
    R_ANIMPROP(float, controlPtDelay, _controlPtDelay)
    R_ANIMPROP(float, tangentSpread, _tangentSpread)
    R_ANIMPROP(float, tangentLength, _tangentLength)
    R_ANIMPROP(glm::vec3, colorMid, _colorMid)
    R_ANIMPROP(glm::vec3, colorEnd, _colorEnd)
    R_ANIMPROP(glm::vec3, colorStart, _colorStart)
    R_ANIMPROP(float, detonate, _detonate)

    // END Keyframes

private:
    uint16_t _number;
    std::string _name;
    ModelNode *_parent;
    bool _animated;

    uint16_t _flags {0};
    std::vector<std::shared_ptr<ModelNode>> _children;
    Controllers _controllers;

    // Transformations

    glm::vec3 _restPosition {0.0f};
    glm::quat _restOrientation {1.0f, 0.0f, 0.0f, 0.0f};
    glm::mat4 _localTransform {1.0f};
    glm::mat4 _absTransform {1.0f};
    glm::mat4 _absTransformInv {1.0f};

    // END Transformations

    // Specialization

    std::shared_ptr<TriangleMesh> _mesh;
    std::shared_ptr<Light> _light;
    std::shared_ptr<Emitter> _emitter;
    std::shared_ptr<Reference> _reference;

    // END Specialization

    // Keyframes

    AnimatedProperty<glm::vec3> _position;
    AnimatedProperty<glm::quat, SlerpInterpolator> _orientation;
    AnimatedProperty<float> _scale;

    AnimatedProperty<glm::vec3> _selfIllumColor;
    AnimatedProperty<float> _alpha;

    AnimatedProperty<glm::vec3> _color;
    AnimatedProperty<float> _radius;
    AnimatedProperty<float> _shadowRadius;
    AnimatedProperty<float> _verticalDisplacement;
    AnimatedProperty<float> _multiplier;

    AnimatedProperty<float> _alphaEnd;
    AnimatedProperty<float> _alphaStart;
    AnimatedProperty<float> _birthrate;
    AnimatedProperty<float> _bounceCo;
    AnimatedProperty<float> _combineTime;
    AnimatedProperty<float> _drag;
    AnimatedProperty<float> _fps;
    AnimatedProperty<float> _frameEnd;
    AnimatedProperty<float> _frameStart;
    AnimatedProperty<float> _grav;
    AnimatedProperty<float> _lifeExp;
    AnimatedProperty<float> _mass;
    AnimatedProperty<float> _p2pBezier2;
    AnimatedProperty<float> _p2pBezier3;
    AnimatedProperty<float> _particleRot;
    AnimatedProperty<float> _randVel;
    AnimatedProperty<float> _sizeStart;
    AnimatedProperty<float> _sizeEnd;
    AnimatedProperty<float> _sizeStartY;
    AnimatedProperty<float> _sizeEndY;
    AnimatedProperty<float> _spread;
    AnimatedProperty<float> _threshold;
    AnimatedProperty<float> _velocity;
    AnimatedProperty<float> _xSize;
    AnimatedProperty<float> _ySize;
    AnimatedProperty<float> _blurLength;
    AnimatedProperty<float> _lightingDelay;
    AnimatedProperty<float> _lightingRadius;
    AnimatedProperty<float> _lightingScale;
    AnimatedProperty<float> _lightingSubDiv;
    AnimatedProperty<float> _lightingZigZag;
    AnimatedProperty<float> _alphaMid;
    AnimatedProperty<float> _percentStart;
    AnimatedProperty<float> _percentMid;
    AnimatedProperty<float> _percentEnd;
    AnimatedProperty<float> _sizeMid;
    AnimatedProperty<float> _sizeMidY;
    AnimatedProperty<float> _randomBirthRate;
    AnimatedProperty<float> _targetSize;
    AnimatedProperty<float> _numControlPts;
    AnimatedProperty<float> _controlPtRadius;
    AnimatedProperty<float> _controlPtDelay;
    AnimatedProperty<float> _tangentSpread;
    AnimatedProperty<float> _tangentLength;
    AnimatedProperty<glm::vec3> _colorMid;
    AnimatedProperty<glm::vec3> _colorEnd;
    AnimatedProperty<glm::vec3> _colorStart;
    AnimatedProperty<float> _detonate;

    // END Keyframes

    void computeLocalTransform();
    void computeAbsoluteTransform();
};

} // namespace graphics

} // namespace reone
