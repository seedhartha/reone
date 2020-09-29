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

#include <memory>

#include "SDL2/SDL_events.h"

#include "glm/vec3.hpp"

#include "../../render/scene/cameranode.h"

namespace reone {

namespace game {

class Camera {
public:
    virtual bool handle(const SDL_Event &event) = 0;
    virtual void update(float dt) = 0;
    virtual void clearUserInput() = 0;

    float heading() const;
    std::shared_ptr<render::CameraSceneNode> sceneNode() const;

protected:
    float _heading { 0.0f };
    std::shared_ptr<render::CameraSceneNode> _sceneNode;
};

} // namespace game

} // namespace reone
