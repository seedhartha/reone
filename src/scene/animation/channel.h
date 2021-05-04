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

#include "../../graphics/lip/lipanimation.h"
#include "../../graphics/model/animation.h"
#include "../../graphics/model/model.h"
#include "../../graphics/model/modelnode.h"

#include "properties.h"
#include "scenenodestate.h"

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
     * @param sceneNode scene node to apply animations to
     * @param skipNodes list of model node names to ignore
     */
    AnimationChannel(ModelSceneNode *sceneNode, std::set<std::string> ignoreNodes);

    void reset();
    void reset(std::shared_ptr<graphics::Animation> anim, AnimationProperties properties, std::shared_ptr<graphics::LipAnimation> lipAnim = nullptr);

    /**
     * @param dt frame delta time
     * @param visible whether the animated scene node is visible
     */
    void update(float dt, bool visible = true);

    void freeze();

    bool isSameAnimation(const graphics::Animation &anim, const AnimationProperties &properties, std::shared_ptr<graphics::LipAnimation> lipAnim = nullptr) const;

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

    float getTransitionTime() const;
    bool getSceneNodeStateByNumber(uint16_t nodeNumber, SceneNodeState &state) const;

    float time() const { return _time; }

    void setTime(float time);

private:
    ModelSceneNode *_sceneNode;
    std::set<std::string> _ignoreNodes;

    std::shared_ptr<graphics::Animation> _animation;
    AnimationProperties _properties;
    std::shared_ptr<graphics::LipAnimation> _lipAnimation;
    float _time { 0.0f };
    bool _freeze { false };
    bool _finished { false };
    std::unordered_map<uint16_t, SceneNodeState> _stateByNumber;

    void computeSceneNodeStates(const graphics::ModelNode &animNode);
};

} // namespace scene

} // namespace reone
