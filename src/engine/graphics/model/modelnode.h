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

#include "../mesh/mesh.h"
#include "../texture/texture.h"

#include "animatedproperty.h"

namespace reone {

namespace graphics {

#define REO_DECL_ANIMPROP(a, b, c) \
    const AnimatedProperty<a> &b() const { return c; }; \
    AnimatedProperty<a> &b() { return c; };

class Model;

/**
 * Model or animation node. Can be specialized to represent a triangle mesh, a
 * light, an emitter and etc.
 */
class ModelNode : boost::noncopyable {
public:
    struct Skin {
        std::vector<std::string> boneNodeName; /**< node name per bone, used for skeletal animation */
        std::vector<float> boneMap; /**< bone index per node (DFS ordering) */
    };

    struct UVAnimation {
        glm::vec2 dir { 0.0f };
    };

    struct DanglyMeshConstraint {
        float multiplier { 0.0f };
        glm::vec3 position { 0.0f };
    };

    struct DanglyMesh {
        float displacement { 0.0f };
        float tightness { 0.0f };
        float period { 0.0f };
        std::vector<DanglyMeshConstraint> constraints;
    };

    struct AABBTree {
        enum class Plane {
            None = 0,
            PositiveX = 1,
            PositiveY = 2,
            PositiveZ = 4
        };

        int faceIndex { 0 };
        Plane mostSignificantPlane { Plane::None };
        AABB aabb;
        std::shared_ptr<AABBTree> left;
        std::shared_ptr<AABBTree> right;
    };

    struct TriangleMesh {
        std::shared_ptr<Mesh> mesh;
        std::unordered_map<uint32_t, std::vector<uint32_t>> materialFaces;
        UVAnimation uvAnimation;
        glm::vec3 diffuse { 1.0f };
        glm::vec3 ambient { 1.0f };
        int transparency { 0 };

        // Flags

        bool render { false };
        bool shadow { false };
        bool backgroundGeometry { false };

        // END Flags

        // Textures

        std::shared_ptr<Texture> diffuseMap;
        std::shared_ptr<Texture> lightmap;
        std::shared_ptr<Texture> bumpmap;

        // END Textures

        // Specialization

        std::shared_ptr<Skin> skin;
        std::shared_ptr<DanglyMesh> danglyMesh;
        std::shared_ptr<AABBTree> aabbTree;
        bool saber { false };

        // END Specialization
    };

    struct LensFlare {
        std::shared_ptr<Texture> texture;
        glm::vec3 colorShift { 0.0f };
        float position { 0.0f };
        float size { 0.0f };
    };

    struct Light {
        int priority { 0 };
        int dynamicType { 0 };
        bool ambientOnly { false };
        bool affectDynamic { false };
        bool shadow { false };
        bool fading { false };
        float flareRadius { 0.0f };
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
            Punch,
            Lighten
        };

        UpdateMode updateMode { UpdateMode::Invalid };
        RenderMode renderMode { RenderMode::Invalid };
        BlendMode blendMode { BlendMode::Invalid };
        std::shared_ptr<Texture> texture;
        glm::ivec2 gridSize { 0 };
        int renderOrder { 0 };
        bool loop { false };
        bool p2p { false };
        bool p2pBezier { false };
    };

    struct Reference {
        std::shared_ptr<Model> model;
        bool reattachable { false };
    };

    ModelNode(
        std::string name,
        glm::vec3 restPosition,
        glm::quat restOrientation,
        const ModelNode *parent = nullptr);

    void init();

    void addChild(std::shared_ptr<ModelNode> child);

    std::vector<uint32_t> getFacesByMaterial(uint32_t material) const;

    const std::string &name() const { return _name; }
    uint16_t flags() const { return _flags; }
    const ModelNode *parent() const { return _parent; }
    const std::vector<std::shared_ptr<ModelNode>> &children() const { return _children; }

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
    bool isDanglyMesh() const { return _mesh && _mesh->danglyMesh; }
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

    REO_DECL_ANIMPROP(glm::vec3, position, _position)
    REO_DECL_ANIMPROP(float, scale, _scale)

    REO_DECL_ANIMPROP(glm::vec3, selfIllumColor, _selfIllumColor)
    REO_DECL_ANIMPROP(float, alpha, _alpha)

    REO_DECL_ANIMPROP(glm::vec3, color, _color)
    REO_DECL_ANIMPROP(float, radius, _radius)
    REO_DECL_ANIMPROP(float, shadowRadius, _shadowRadius)
    REO_DECL_ANIMPROP(float, verticalDisplacement, _verticalDisplacement)
    REO_DECL_ANIMPROP(float, multiplier, _multiplier)

    REO_DECL_ANIMPROP(float, alphaEnd, _alphaEnd)
    REO_DECL_ANIMPROP(float, alphaStart, _alphaStart)
    REO_DECL_ANIMPROP(float, birthrate, _birthrate)
    REO_DECL_ANIMPROP(float, bounceCo, _bounceCo)
    REO_DECL_ANIMPROP(float, combineTime, _combineTime)
    REO_DECL_ANIMPROP(float, drag, _drag)
    REO_DECL_ANIMPROP(float, fps, _fps)
    REO_DECL_ANIMPROP(float, frameEnd, _frameEnd)
    REO_DECL_ANIMPROP(float, frameStart, _frameStart)
    REO_DECL_ANIMPROP(float, grav, _grav)
    REO_DECL_ANIMPROP(float, lifeExp, _lifeExp)
    REO_DECL_ANIMPROP(float, mass, _mass)
    REO_DECL_ANIMPROP(float, p2pBezier2, _p2pBezier2)
    REO_DECL_ANIMPROP(float, p2pBezier3, _p2pBezier3)
    REO_DECL_ANIMPROP(float, particleRot, _particleRot)
    REO_DECL_ANIMPROP(float, randVel, _randVel)
    REO_DECL_ANIMPROP(float, sizeStart, _sizeStart)
    REO_DECL_ANIMPROP(float, sizeEnd, _sizeEnd)
    REO_DECL_ANIMPROP(float, sizeStartY, _sizeStartY)
    REO_DECL_ANIMPROP(float, sizeEndY, _sizeEndY)
    REO_DECL_ANIMPROP(float, spread, _spread)
    REO_DECL_ANIMPROP(float, threshold, _threshold)
    REO_DECL_ANIMPROP(float, velocity, _velocity)
    REO_DECL_ANIMPROP(float, xSize, _xSize)
    REO_DECL_ANIMPROP(float, ySize, _ySize)
    REO_DECL_ANIMPROP(float, blurLength, _blurLength)
    REO_DECL_ANIMPROP(float, lightingDelay, _lightingDelay)
    REO_DECL_ANIMPROP(float, lightingRadius, _lightingRadius)
    REO_DECL_ANIMPROP(float, lightingScale, _lightingScale)
    REO_DECL_ANIMPROP(float, lightingSubDiv, _lightingSubDiv)
    REO_DECL_ANIMPROP(float, lightingZigZag, _lightingZigZag)
    REO_DECL_ANIMPROP(float, alphaMid, _alphaMid)
    REO_DECL_ANIMPROP(float, percentStart, _percentStart)
    REO_DECL_ANIMPROP(float, percentMid, _percentMid)
    REO_DECL_ANIMPROP(float, percentEnd, _percentEnd)
    REO_DECL_ANIMPROP(float, sizeMid, _sizeMid)
    REO_DECL_ANIMPROP(float, sizeMidY, _sizeMidY)
    REO_DECL_ANIMPROP(float, randomBirthRate, _randomBirthRate)
    REO_DECL_ANIMPROP(float, targetSize, _targetSize)
    REO_DECL_ANIMPROP(float, numControlPts, _numControlPts)
    REO_DECL_ANIMPROP(float, controlPtRadius, _controlPtRadius)
    REO_DECL_ANIMPROP(float, controlPtDelay, _controlPtDelay)
    REO_DECL_ANIMPROP(float, tangentSpread, _tangentSpread)
    REO_DECL_ANIMPROP(float, tangentLength, _tangentLength)
    REO_DECL_ANIMPROP(glm::vec3, colorMid, _colorMid)
    REO_DECL_ANIMPROP(glm::vec3, colorEnd, _colorEnd)
    REO_DECL_ANIMPROP(glm::vec3, colorStart, _colorStart)
    REO_DECL_ANIMPROP(float, detonate, _detonate)

    // END Keyframes

private:
    std::string _name;
    const ModelNode *_parent;

    uint16_t _flags { 0 };
    std::vector<std::shared_ptr<ModelNode>> _children;

    // Transformations

    glm::vec3 _restPosition { 0.0f };
    glm::quat _restOrientation { 1.0f, 0.0f, 0.0f, 0.0f };
    glm::mat4 _localTransform { 1.0f };
    glm::mat4 _absTransform { 1.0f };
    glm::mat4 _absTransformInv { 1.0f };

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
