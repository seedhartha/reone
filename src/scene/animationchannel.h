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

#include "../render/lip/lipanimation.h"
#include "../render/model/animation.h"
#include "../render/model/model.h"
#include "../render/model/modelnode.h"

#include "animationproperties.h"

namespace reone {

namespace scene {

class ModelSceneNode;

/**
 * Represents a single animation being played on a model. Multiple animation
 * channels can be blended or overlayed by SceneNodeAnimator.
 *
 * @see SceneNodeAnimator
 */
class AnimationChannel {
public:
    /**
     * @param modelSceneNode scene node to apply animations to
     * @param skipNodes list of model node names to ignore
     */
    AnimationChannel(ModelSceneNode *modelSceneNode, std::set<std::string> ignoreNodes);

    void reset();
    void reset(std::shared_ptr<render::Animation> anim, AnimationProperties properties, std::shared_ptr<render::LipAnimation> lipAnim = nullptr);
    void update(float dt);
    void freeze();

    bool isSameAnimation(const render::Animation &anim, const AnimationProperties &properties, std::shared_ptr<render::LipAnimation> lipAnim = nullptr) const;

    /**
     * @return true if this animation channel contains an animation that is not finished, false otherwise
     */
    bool isActive() const;

    /**
     * @return true if this animation channel contains that is past transition time, false otherwise
     */
    bool isPastTransitionTime() const;

    /**
     * @return true if this animation channel contains an animation that is finished, false otherwise
     */
    bool isFinished() const;

    bool getTransformByNodeNumber(uint16_t nodeNumber, glm::mat4 &transform) const;
    float getTransitionTime() const;

    float time() const { return _time; }

    void setTime(float time);

private:
    ModelSceneNode *_modelSceneNode;
    std::set<std::string> _ignoreNodes;

    std::shared_ptr<render::Animation> _animation;
    AnimationProperties _properties;
    std::shared_ptr<render::LipAnimation> _lipAnimation;
    float _time { 0.0f };
    bool _freeze { false };
    bool _finished { false };
    std::unordered_map<uint16_t, glm::mat4> _transformByNodeNumber;

    void computeLocalTransforms();
    void computeLocalTransform(const render::ModelNode &animNode);
};

} // namespace scene

} // namespace reone
