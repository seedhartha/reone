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

#include "../../graphics/model/animation.h"
#include "../../graphics/model/modelnode.h"

#include "channel.h"
#include "properties.h"
#include "scenenodestate.h"

namespace reone {

namespace scene {

class ModelSceneNode;

/**
 * Responsible for applying animations to model scene nodes. Manages up to
 * kChannelCount animation channels, which it either blends or overlays depending
 * on the composition mode.
 */
class SceneNodeAnimator {
public:
    /**
     * @param sceneNode scene node to apply animations to
     * @param skipNodes names of model nodes to ignore
     */
    SceneNodeAnimator(ModelSceneNode *sceneNode, std::set<std::string> ignoreNodes);

    /**
     * @param dt frame delta time
     * @param visible whether the animated scene node is visible
     */
    void update(float dt, bool visible = true);

    void playAnimation(const std::string &name, AnimationProperties properties = AnimationProperties());
    void playAnimation(std::shared_ptr<graphics::Animation> anim, AnimationProperties properties = AnimationProperties(), std::shared_ptr<graphics::LipAnimation> lipAnim = nullptr);
    void playDefaultAnimation();

    bool isAnimationFinished() const;

    void setDefaultAnimation(std::string name, AnimationProperties properties = AnimationProperties());

private:
    static constexpr int kChannelCount = 8;

    enum class CompositionMode {
        Mono, /**< only animation on the first channel is being played */
        Overlay, /**< animations on all channels play simultaneously  */
        Blend /**< animation on the second channel is transitioned into animation on the first channel */
    };

    struct NodeState {
        glm::mat4 transform { 1.0f };
        float alpha { 1.0f };
        glm::vec3 selfIllumColor { 0.0f };
    };

    ModelSceneNode *_sceneNode;
    std::set<std::string> _ignoreNodes;
    std::vector<AnimationChannel> _channels;

    CompositionMode _compositionMode { CompositionMode::Overlay };
    bool _transition { false }; /**< is there an animation transition going on? */
    std::unordered_map<uint16_t, SceneNodeState> _stateByNumber;

    std::string _defaultAnimName;
    AnimationProperties _defaultAnimProperties;

    void computeSceneNodeStates(graphics::ModelNode &modelNode, glm::mat4 parentTransform = glm::mat4(1.0f));
    void applySceneNodeStates(graphics::ModelNode &modelNode);

    bool isInTransition() const;

    /**
     * Determines the composition mode by animation flags.
     */
    CompositionMode determineCompositionMode(int flags) const;
};

} // namespace scene

} // namespace reone
