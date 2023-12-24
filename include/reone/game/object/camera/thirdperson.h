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

#include "reone/input/event.h"

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

    bool handle(const input::Event &event) override;
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

    bool handleKeyDown(const input::KeyEvent &event);
    bool handleKeyUp(const input::KeyEvent &event);
    bool handleMouseMotion(const input::MouseMotionEvent &event);
    bool handleMouseButtonDown(const input::MouseButtonEvent &event);
    bool handleMouseButtonUp(const input::MouseButtonEvent &event);
};

} // namespace game

} // namespace reone
