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

#include <functional>

#include "camera.h"

#include "../types.h"

namespace reone {

namespace render {

class ThirdPersonCamera : public Camera {
public:
    ThirdPersonCamera(float aspect, const CameraStyle &style, float zNear = 0.1f, float zFar = 10000.0f);

    bool handle(const SDL_Event &event) override;
    void update(float dt) override;
    void resetInput() override;

    void setTargetPosition(const glm::vec3 &position);
    void setHeading(float heading);

    void setFindObstacleFunc(const std::function<bool(const glm::vec3 &, const glm::vec3 &, glm::vec3 &)> &fn);

private:
    CameraStyle _style;
    glm::vec3 _targetPosition { 0.0f };
    bool _rotateCCW { false };
    bool _rotateCW { false };
    float _rotationSpeed { 0.0f };
    std::function<bool(const glm::vec3 &, const glm::vec3 &, glm::vec3 &)> _findObstacleFunc;

    void updateView();
    bool handleKeyDown(const SDL_KeyboardEvent &event);
    bool handleKeyUp(const SDL_KeyboardEvent &event);
};

} // namespace render

} // namespace reone
