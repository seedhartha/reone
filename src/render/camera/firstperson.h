#pragma once

#include "camera.h"

namespace reone {

namespace render {

class FirstPersonCamera : public Camera {
public:
    FirstPersonCamera(float aspect, float fovy, float zNear = 0.1f, float zFar = 10000.0f);

    bool handle(const SDL_Event &event) override;
    void update(float dt) override;

    void setPosition(const glm::vec3 &position);
    void setHeading(float heading);

private:
    float _pitch { 0.0f };
    bool _moveForward { false };
    bool _moveLeft { false };
    bool _moveBackward { false };
    bool _moveRight { false };

    bool handleMouseMotion(const SDL_MouseMotionEvent &event);
    void updateView();
    bool handleKeyDown(const SDL_KeyboardEvent &event);
    bool handleKeyUp(const SDL_KeyboardEvent &event);
};

} // namespace render

} // namespace reone
