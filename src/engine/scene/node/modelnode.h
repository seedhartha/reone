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

#include <cstdint>
#include <set>
#include <unordered_map>

#include "../../graphics/lip/animation.h"
#include "../../graphics/model/model.h"

#include "../animeventlistener.h"
#include "../animproperties.h"
#include "../types.h"

#include "dummynode.h"
#include "emitternode.h"
#include "lightnode.h"
#include "meshnode.h"

namespace reone {

namespace scene {

constexpr float kDefaultDrawDistance = 1024.0f;
constexpr int kNumAnimationChannels = 8;

class ModelSceneNode : public SceneNode {
public:
    ModelSceneNode(
        std::shared_ptr<graphics::Model> model,
        ModelUsage usage,
        SceneGraph *sceneGraph,
        IAnimationEventListener *animEventListener = nullptr);

    void update(float dt) override;

    void computeAABB();
    void signalEvent(const std::string &name);

    std::shared_ptr<ModelNodeSceneNode> getNodeByName(const std::string &name) const;

    std::shared_ptr<graphics::Model> model() const { return _model; }
    ModelUsage usage() const { return _usage; }
    float drawDistance() const { return _drawDistance; }

    void setDrawDistance(float distance) { _drawDistance = distance; }
    void setDiffuseTexture(std::shared_ptr<graphics::Texture> texture);
    void setAppliedForce(glm::vec3 force);

    // Animation

    void playAnimation(const std::string &name, AnimationProperties properties = AnimationProperties());
    void playAnimation(std::shared_ptr<graphics::Animation> anim, std::shared_ptr<graphics::LipAnimation> lipAnim = nullptr, AnimationProperties properties = AnimationProperties());

    bool isAnimationFinished() const;

    void setInanimateNodes(std::set<std::string> nodes) { _inanimateNodes = std::move(nodes); }

    // END Animation

    // Attachments

    void attach(const std::string &parentName, std::shared_ptr<SceneNode> node);

    std::shared_ptr<SceneNode> getAttachment(const std::string &parentName) const;

    // END Attachments

private:
    enum class AnimationBlendMode {
        Single,
        Blend,
        Overlay
    };

    struct AnimationStateFlags {
        static constexpr int transform = 1;
        static constexpr int alpha = 2;
        static constexpr int selfIllumColor = 4;
    };

    struct AnimationState {
        int flags { 0 };
        glm::mat4 transform { 1.0f };
        float alpha { 0.0f };
        glm::vec3 selfIllumColor { 0.0f };
    };

    struct AnimationChannel {
        std::shared_ptr<graphics::Animation> anim;
        std::shared_ptr<graphics::LipAnimation> lipAnim;
        AnimationProperties properties;
        float time { 0.0f };
        std::unordered_map<std::string, AnimationState> stateByName;

        // Flags

        bool finished { false }; /**< channel contains a fire-and-forget animation that has finished playing */
        bool transition { false }; /**< when computing states, use animation transition time as channel time */
        bool freeze { false }; /**< channel time is not to be updated */

        // END Flags
    };

    std::shared_ptr<graphics::Model> _model;
    ModelUsage _usage;
    IAnimationEventListener *_animEventListener;

    float _drawDistance { kDefaultDrawDistance };

    // Lookups

    std::unordered_map<std::string, std::shared_ptr<ModelNodeSceneNode>> _nodeByName;
    std::unordered_map<std::string, std::shared_ptr<SceneNode>> _attachments;

    // END Lookups

    // Animation

    AnimationChannel _animChannels[kNumAnimationChannels];
    AnimationBlendMode _animBlendMode { AnimationBlendMode::Single };
    std::set<std::string> _inanimateNodes; /**< names of nodes that are not to be animated */

    // END Animation

    void buildNodeTree(std::shared_ptr<graphics::ModelNode> node, SceneNode *parent);

    std::unique_ptr<DummySceneNode> newDummySceneNode(std::shared_ptr<graphics::ModelNode> node) const;
    std::unique_ptr<MeshSceneNode> newMeshSceneNode(std::shared_ptr<graphics::ModelNode> node) const;
    std::unique_ptr<LightSceneNode> newLightSceneNode(std::shared_ptr<graphics::ModelNode> node) const;
    std::unique_ptr<EmitterSceneNode> newEmitterSceneNode(std::shared_ptr<graphics::ModelNode> node) const;

    // Animation

    void resetAnimationChannel(AnimationChannel &channel, std::shared_ptr<graphics::Animation> anim = nullptr, std::shared_ptr<graphics::LipAnimation> lipAnim = nullptr, AnimationProperties properties = AnimationProperties());
    void updateAnimations(float dt);
    void updateAnimationChannel(AnimationChannel &channel, float dt);
    void computeAnimationStates(AnimationChannel &channel, float time, const graphics::ModelNode &modelNode);
    void applyAnimationStates(const graphics::ModelNode &modelNode);
    void computeBoneTransforms();

    static AnimationBlendMode getAnimationBlendMode(int flags);

    // END Animation
};

} // namespace scene

} // namespace reone
