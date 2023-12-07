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

#include "reone/graphics/lipanimation.h"
#include "reone/graphics/model.h"
#include "reone/graphics/types.h"

#include "../animeventlistener.h"
#include "../animproperties.h"
#include "../node.h"
#include "../types.h"

#include "dummy.h"
#include "emitter.h"
#include "light.h"
#include "mesh.h"

namespace reone {

namespace graphics {

class Textures;
class Uniforms;

} // namespace graphics

namespace scene {

class ModelSceneNode : public SceneNode {
public:
    enum class AnimationBlendMode {
        Single,
        Blend,
        Overlay
    };

    struct AnimationStateFlags {
        static constexpr int transform = 1;
        static constexpr int alpha = 2;
        static constexpr int selfIllumColor = 4;
        static constexpr int color = 8;
    };

    struct AnimationState {
        int flags {0};
        glm::mat4 transform {1.0f};
        float alpha {0.0f};
        glm::vec3 selfIllumColor {0.0f};
        glm::vec3 color {0.0f};
    };

    struct AnimationChannel {
        graphics::Animation *anim;
        graphics::LipAnimation *lipAnim;
        AnimationProperties properties;
        float time {0.0f};
        std::unordered_map<uint16_t, AnimationState> stateByNodeNumber;
        bool freeze {false};     /**< channel time is not to be updated */
        bool transition {false}; /**< when computing states, use animation transition time as channel time */
        bool finished {false};   /**< finished channels will be erased from the queue */

        AnimationChannel(graphics::Animation &anim, graphics::LipAnimation *lipAnim, AnimationProperties properties) :
            anim(&anim),
            lipAnim(lipAnim),
            properties(std::move(properties)) {
        }
    };

    ModelSceneNode(
        graphics::Model &model,
        ModelUsage usage,
        SceneGraph &sceneGraph,
        graphics::GraphicsServices &graphicsSvc,
        audio::AudioServices &audioSvc,
        resource::ResourceServices &resourceSvc);

    void init();

    void update(float dt) override;

    void drawLeafs(graphics::IRenderPass &pass, const std::vector<SceneNode *> &leafs) override;

    void drawAABB();

    void computeAABB();
    void signalEvent(const std::string &name);

    bool isPickable() const { return _pickable; }

    ModelNodeSceneNode *getNodeByNumber(uint16_t number);
    ModelNodeSceneNode *getNodeByName(const std::string &name);

    const graphics::Model &model() const { return *_model; }
    ModelUsage usage() const { return _usage; }
    float drawDistance() const { return _drawDistance; }

    void setModel(graphics::Model &model);
    void setDrawDistance(float distance) { _drawDistance = distance; }
    void setDiffuseMap(graphics::Texture *texture);
    void setEnvironmentMap(graphics::Texture *texture);
    void setPickable(bool pickable) { _pickable = pickable; }

    // Animation

    void playAnimation(const std::string &name, graphics::LipAnimation *lipAnim = nullptr, AnimationProperties properties = AnimationProperties());
    void playAnimation(graphics::Animation &anim, graphics::LipAnimation *lipAnim = nullptr, AnimationProperties properties = AnimationProperties());

    void pauseAnimation();
    void resumeAnimation();
    void setAnimationTime(float time);

    bool isAnimationFinished() const;

    std::string activeAnimationName() const;

    const std::deque<AnimationChannel> &animationChannels() const {
        return _animChannels;
    }

    // END Animation

    // Attachments

    void attach(const std::string &parentName, SceneNode &node);

    SceneNode *getAttachment(const std::string &parentName);

    // END Attachments

private:
    graphics::Model *_model;
    ModelUsage _usage;

    IAnimationEventListener *_animEventListener {nullptr};
    float _drawDistance {std::numeric_limits<float>::max()};

    // Lookups

    std::unordered_map<uint16_t, ModelNodeSceneNode *> _nodeByNumber;
    std::unordered_map<std::string, ModelNodeSceneNode *> _nodeByName;
    std::unordered_map<std::string, SceneNode *> _attachments;

    // END Lookups

    // Animation

    std::deque<AnimationChannel> _animChannels;
    AnimationBlendMode _animBlendMode {AnimationBlendMode::Single};

    // END Animation

    // Flags

    bool _pickable {false};

    // END Flags

    void buildNodeTree(graphics::ModelNode &node, SceneNode &parent);

    // Animation

    void updateAnimations(float dt);
    void updateAnimationChannel(AnimationChannel &channel, float dt);
    void computeAnimationStates(AnimationChannel &channel, float time, const graphics::ModelNode &modelNode);
    void applyAnimationStates(const graphics::ModelNode &modelNode);

    static AnimationBlendMode getAnimationBlendMode(int flags);

    // END Animation
};

} // namespace scene

} // namespace reone
