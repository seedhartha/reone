/*
 * Copyright (c) 2020 The reone project contributors
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

#include <string>

#include "../../scene/modelscenenode.h"
#include "../../scene/scenegraph.h"

#include "../types.h"

#include "camera.h"

namespace reone {

namespace game {

const float kDefaultAnimCamFOV = 55.0f;

class AnimatedCamera : public Camera {
public:
    AnimatedCamera(scene::SceneGraph *sceneGraph, float aspect);

    void update(float dt) override;

    void playAnimation(int animNumber);

    bool isAnimationFinished() const;

    void setModel(const std::string &resRef);
    void setFieldOfView(float fovy);

private:
    scene::SceneGraph *_sceneGraph { nullptr };
    float _aspect { 1.0f };
    std::string _modelResRef;
    std::unique_ptr<scene::ModelSceneNode> _model;
    float _fovy { kDefaultAnimCamFOV };
    float _zNear { 0.1f };
    float _zFar { 10000.0f };

    void updateProjection();
};

} // namespace game

} // namespace reone
