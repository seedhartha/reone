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

#include "camera.h"

namespace reone {

namespace game {

class FirstPersonCamera : public Camera {
public:
    FirstPersonCamera(render::SceneGraph *sceneGraph, float aspect, float fovy, float zNear = 0.1f, float zFar = 10000.0f);

    bool handle(const SDL_Event &event) override;
    void update(float dt) override;
    void clearUserInput() override;

    void setPosition(const glm::vec3 &position);
    void setHeading(float heading);

private:
    glm::vec3 _position { 0.0f };
    float _pitch { 0.0f };
    bool _moveForward { false };
    bool _moveLeft { false };
    bool _moveBackward { false };
    bool _moveRight { false };

    bool handleMouseMotion(const SDL_MouseMotionEvent &event);
    void updateSceneNode();
    bool handleKeyDown(const SDL_KeyboardEvent &event);
    bool handleKeyUp(const SDL_KeyboardEvent &event);
};

} // namespace game

} // namespace reone
