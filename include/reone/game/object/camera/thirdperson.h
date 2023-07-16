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

#include "../../camerastyle.h"

#include "../camera.h"

namespace reone {

namespace game {

class ThirdPersonCamera : public Camera {
public:
    ThirdPersonCamera(
        uint32_t id,
        CameraStyle style,
        float aspect,
        std::string sceneName,
        Game &game,
        ServicesView &services) :
        Camera(
            id,
            std::move(sceneName),
            game,
            services),
        _style(std::move(style)),
        _aspect(aspect) {
    }

    void load();

    bool handle(const SDL_Event &event) override;
    void update(float dt) override;
    void stopMovement() override;

    void setTargetPosition(glm::vec3 position);
    void setFacing(float facing);
    void setStyle(CameraStyle style);

private:
    CameraStyle _style;
    float _aspect;

    glm::vec3 _targetPosition {0.0f};
    bool _rotateCCW {false};
    bool _rotateCW {false};
    float _rotationSpeed {0.0f};

    void updateSceneNode();

    bool handleKeyDown(const SDL_KeyboardEvent &event);
    bool handleKeyUp(const SDL_KeyboardEvent &event);
    bool handleMouseMotion(const SDL_MouseMotionEvent &event);
    bool handleMouseButtonDown(const SDL_MouseButtonEvent &event);
    bool handleMouseButtonUp(const SDL_MouseButtonEvent &event);
};

} // namespace game

} // namespace reone
