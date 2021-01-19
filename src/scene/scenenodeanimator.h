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
#include <memory>
#include <set>
#include <string>
#include <unordered_map>

#include "glm/mat4x4.hpp"

#include "../render/model/animation.h"
#include "../render/model/modelnode.h"

namespace reone {

namespace scene {

class ModelSceneNode;

/**
 * Class, responsible for applying model animations to scene nodes. Manages
 * up to 8 animation channels, which are either overlayed or blended together.
 */
class SceneNodeAnimator {
public:
    /**
     * @param modelSceneNode scene node to apply animations to
     * @param skipNodes names of model nodes, to which animations will not be applied
     */
    SceneNodeAnimator(ModelSceneNode *modelSceneNode, const std::set<std::string> &skipNodes);

    void update(float dt);

    void playDefaultAnimation();
    void playAnimation(const std::shared_ptr<render::Animation> &anim, int flags = 0, float speed = 1.0f, float scale = 1.0f);

    bool isAnimationFinished() const;

    /**
     * Sets animation to play when current animations finish playing.
     */
    void setDefaultAnimation(const std::shared_ptr<render::Animation> &anim, int flags = 0, float speed = 1.0f);

private:
    static constexpr int kChannelCount = 8;

    enum class Mode {
        Mono, /**< only animation on the first channel is being played */
        Overlay, /**< animations on all channels play simultaneously  */
        Blend /**< animation on the second channel is transitioned into animation on the first channel */
    };

    struct AnimationChannel {
        std::shared_ptr<render::Animation> animation;
        int flags { 0 };
        float speed { 1.0f };
        float scale { 1.0f };
        float time { 0.0f };
        bool finished { false };
        bool freeze { false }; /**< if true, animation on this channel should not advance */
        std::unordered_map<uint16_t, glm::mat4> localTransforms;

        bool isActive() const;
        bool isSameAnimation(const std::shared_ptr<render::Animation> &anim, int flags, float speed, float scale) const;

        void stopAnimation();

        void setAnimation(const std::shared_ptr<render::Animation> &anim, int flags = 0, float speed = 1.0f, float scale = 1.0f);
    };

    ModelSceneNode *_modelSceneNode;
    std::set<std::string> _skipNodes;

    Mode _mode { Mode::Overlay };
    bool _transition { false }; /**< is there an animation transition going on? */
    AnimationChannel _channels[kChannelCount];
    std::unordered_map<uint16_t, glm::mat4> _absTransforms;

    // Default animation

    std::shared_ptr<render::Animation> _defaultAnim;
    int _defaultAnimFlags { 0 };
    float _defaultAnimSpeed { 1.0f };

    // END Default animation

    void updateChannel(int channel, float dt);
    void advanceTime(AnimationChannel &channel, float dt);
    void updateLocalTransforms(AnimationChannel &channel, render::ModelNode &animNode);
    void updateAbsoluteTransforms(render::ModelNode &modelNode, const glm::mat4 &parentTransform = glm::mat4(1.0f));
    void applyAnimationTransforms(render::ModelNode &modelNode);

    bool isInTransition() const;

    /**
     * Determines the animation mode by animation flags.
     */
    Mode getMode(int animFlags) const;
};

} // namespace scene

} // namespace reone
