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
#include "keyframetrack.h"
#include "mesh.h"
#include "texture.h"

namespace reone {

namespace graphics {

class Model;

class ModelNode : boost::noncopyable {
public:
    template <class Value>
    using KeyframeTrackMap = std::unordered_map<ControllerType, KeyframeTrack<Value>>;

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

    // Keyframe Tracks

    inline bool positionAtTime(float time, glm::vec3 &position) const {
        return vectorValueAtTime(ControllerTypes::position, time, position);
    }

    inline bool orientationAtTime(float time, glm::quat &orientation) const {
        return quaternionValueAt(ControllerTypes::orientation, time, orientation);
    }

    inline bool scaleAtTime(float time, float &scale) const {
        return floatValueAtTime(ControllerTypes::scale, time, scale);
    }

    bool floatValueAtTime(ControllerType type, float time, float &value) const;
    bool vectorValueAtTime(ControllerType type, float time, glm::vec3 &value) const;
    bool quaternionValueAt(ControllerType type, float time, glm::quat &value) const;

    KeyframeTrackMap<float> &floatTracks() { return _floatTracks; }
    KeyframeTrackMap<glm::vec3> &vectorTracks() { return _vectorTracks; }
    KeyframeTrackMap<glm::quat> &quaternionTracks() { return _quaternionTracks; }

    // END Keyframe Tracks

private:
    uint16_t _number;
    std::string _name;
    ModelNode *_parent;
    bool _animated;

    uint16_t _flags {0};
    std::vector<std::shared_ptr<ModelNode>> _children;

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

    // Keyframe Tracks

    KeyframeTrackMap<glm::vec3> _vectorTracks;
    KeyframeTrackMap<glm::quat> _quaternionTracks;
    KeyframeTrackMap<float> _floatTracks;

    // END Keyframe Tracks

    void computeLocalTransform();
    void computeAbsoluteTransform();
};

} // namespace graphics

} // namespace reone
