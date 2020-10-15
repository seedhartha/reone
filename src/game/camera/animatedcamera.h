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

#include <string>

#include "../../render/scene/modelnode.h"
#include "../../render/scene/scenegraph.h"

#include "../types.h"

#include "camera.h"

namespace reone {

namespace game {

class AnimatedCamera : public Camera {
public:
    AnimatedCamera(render::SceneGraph *sceneGraph, const CameraStyle &style, float aspect, float zNear = 0.1f, float zFar = 10000.0f);

    void update(float dt) override;

    void animate(int animNumber);

    void setModel(const std::string &resRef);

private:
    render::SceneGraph *_sceneGraph { nullptr };
    std::string _modelResRef;
    std::unique_ptr<render::ModelSceneNode> _model;
};

} // namespace game

} // namespace reone
