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

#include <cstdint>
#include <set>
#include <string>
#include <unordered_map>

#include "glm/mat4x4.hpp"

namespace reone {

namespace render {

enum AnimationFlags {
    kAnimationLoop = 1,
    kAnimationPropagate = 2
};

class Animation;
class ModelNode;
class ModelSceneNode;

class SceneNodeAnimator {
public:
    SceneNodeAnimator(ModelSceneNode *modelSceneNode, const std::set<std::string> &skipNodes);

    void update(float dt);

    void playDefaultAnimation();
    void playAnimation(const std::string &name, int flags = 0, float speed = 1.0f);
    void playAnimation(int channel, const std::string &name, int flags = 0, float speed = 1.0f);

    bool isAnimationFinished() const;

    void setDefaultAnimation(const std::string &name);

private:
    static const int kChannelCount = 2;

    struct AnimationChannel {
        std::string animName;
        int animFlags { 0 };
        float animSpeed { 1.0f };
        float animTime { 0.0f };
        Animation *animation { nullptr };
        bool animFinished { false };
        std::unordered_map<uint16_t, glm::mat4> localTransforms;

        bool isSameAnimation(const std::string &name, int flags, float speed) const;
    };

    ModelSceneNode *_modelSceneNode { nullptr };
    std::set<std::string> _skipNodes;
    AnimationChannel _channels[kChannelCount];
    std::string _defaultAnim;

    void updateChannel(int channel, float dt);
    void advanceTime(AnimationChannel &channel, float dt);
    void updateAnimationTransforms(AnimationChannel &channel, ModelNode &animNode);
    void updateNodeTransforms(ModelNode &modelNode, const glm::mat4 &parentTransform = glm::mat4(1.0f));
    void stopAnimation(int channel);
};

} // namespace render

} // namespace reone
