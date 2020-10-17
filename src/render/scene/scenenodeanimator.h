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

#include <set>
#include <string>

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

    bool isAnimationFinished() const;

    void setDefaultAnimation(const std::string &name);

private:
    ModelSceneNode *_modelSceneNode { nullptr };
    std::set<std::string> _skipNodes;
    std::string _animName;
    int _animFlags { 0 };
    float _animSpeed { 1.0f };
    float _animTime { 0.0f };
    Animation *_animation { nullptr };
    bool _animFinished { false };
    std::string _defaultAnim;

    void advanceTime(float dt);
    void updateModelNodes();
    void applyAnimationTransforms(ModelNode &animNode);
    void updateBoneTransforms(ModelNode &modelNode);
};

} // namespace render

} // namespace reone
