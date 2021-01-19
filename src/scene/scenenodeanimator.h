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
 * Class, responsible for applying model animations to scene nodes.
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
    void setDefaultAnimation(const std::shared_ptr<render::Animation> &anim);

private:
    static constexpr int kChannelCount = 2;

    struct AnimationChannel {
        std::shared_ptr<render::Animation> animation;
        int flags { 0 };
        float speed { 1.0f };
        float scale { 1.0f };
        float time { 0.0f };
        bool finished { false };
        bool transition { false };
        bool freeze { false };
        std::unordered_map<uint16_t, glm::mat4> localTransforms;

        bool isActive() const;
        bool isSameAnimation(const std::shared_ptr<render::Animation> &anim, int flags, float speed, float scale) const;

        void setAnimation(const std::shared_ptr<render::Animation> &anim, int flags = 0, float speed = 1.0f, float scale = 1.0f);

        void stopAnimation();
    };

    ModelSceneNode *_modelSceneNode { nullptr };
    std::set<std::string> _skipNodes;
    AnimationChannel _channels[kChannelCount];
    std::shared_ptr<render::Animation> _defaultAnimation;
    std::unordered_map<uint16_t, glm::mat4> _absTransforms;

    void updateChannel(int channel, float dt);
    void advanceTime(AnimationChannel &channel, float dt);
    void updateLocalTransforms(AnimationChannel &channel, render::ModelNode &animNode);
    void updateAbsoluteTransforms(render::ModelNode &modelNode, const glm::mat4 &parentTransform = glm::mat4(1.0f));
    void updateNodeTransforms(render::ModelNode &modelNode);
};

} // namespace scene

} // namespace reone
