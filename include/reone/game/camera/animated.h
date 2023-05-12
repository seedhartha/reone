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

#include "../camera.h"

namespace reone {

namespace graphics {

class Model;

}

namespace scene {

class ModelSceneNode;
class SceneGraph;

} // namespace scene

namespace game {

const float kDefaultAnimCamFOV = 55.0f;

class AnimatedCamera : public Camera {
public:
    AnimatedCamera(float aspect, scene::SceneGraph &sceneGraph);

    void update(float dt) override;

    void playAnimation(int animNumber);

    bool isAnimationFinished() const;

    void setModel(std::shared_ptr<graphics::Model> model);
    void setFieldOfView(float fovy);

private:
    float _aspect {1.0f};
    scene::SceneGraph &_sceneGraph;

    std::shared_ptr<scene::ModelSceneNode> _model;
    float _fovy {kDefaultAnimCamFOV};

    void updateProjection();
};

} // namespace game

} // namespace reone
