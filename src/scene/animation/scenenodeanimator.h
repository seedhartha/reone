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

#include "../../render/model/animation.h"
#include "../../render/model/modelnode.h"

#include "channel.h"
#include "properties.h"

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
     * @param modelSceneNode scene node to apply animations to
     * @param skipNodes names of model nodes to ignore
     */
    SceneNodeAnimator(ModelSceneNode *modelSceneNode, std::set<std::string> ignoreNodes);

    void update(float dt);

    void playAnimation(const std::string &name, AnimationProperties properties = AnimationProperties());
    void playAnimation(std::shared_ptr<render::Animation> anim, AnimationProperties properties = AnimationProperties(), std::shared_ptr<render::LipAnimation> lipAnim = nullptr);
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

    ModelSceneNode *_modelSceneNode;
    std::set<std::string> _ignoreNodes;
    std::vector<AnimationChannel> _channels;

    CompositionMode _compositionMode { CompositionMode::Overlay };
    bool _transition { false }; /**< is there an animation transition going on? */
    std::unordered_map<uint16_t, glm::mat4> _transformByNodeNumber;

    std::string _defaultAnimName;
    AnimationProperties _defaultAnimProperties;

    void computeAbsoluteTransforms(render::ModelNode &modelNode, glm::mat4 parentTransform = glm::mat4(1.0f));
    void applyAbsoluteTransforms(render::ModelNode &modelNode);

    bool isInTransition() const;

    /**
     * Determines the composition mode by animation flags.
     */
    CompositionMode determineCompositionMode(int flags) const;
};

} // namespace scene

} // namespace reone
